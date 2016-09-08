#include "../library.h"
#include "../includeAll.h"
#include "pageRoll.h"
//#include "../stringData.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeRollGameClear{
	struct popupCartridge super;
	struct pageRollStatus *stat;

	struct popupManager popup;
	struct partsButtonHex btnRetry;
	struct partsButtonHex btnBack;
	struct partsButtonHex btnNext;
	struct partsButtonHex btnPostBig;
	struct partsButtonHex btnPostSmall;
	struct partsButton btnWindow;
	struct partsEmblem emblem1;
	struct partsEmblem emblem2;

	struct objectRollReword reword;

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTrans transEmblem1;
	struct e3dTrans transEmblem2;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int step;
	int result;
	int reqIdSave;
};

// ----------------------------------------------------------------

// jsonリクエスト作成
static char *saveRequest(struct popupCartridgeRollGameClear *this){
	struct engineDataJsonValue root;
	root.type = ENGINEDATAJSONTYPE_NULL;

	engineDataJsonSetString(engineDataJsonObjectCreateValue(&root, "version"), "0003");
	engineDataJsonSetString(engineDataJsonObjectCreateValue(&root, "platform"), platformPluginUtilPlatformGet());
	engineDataJsonSetString(engineDataJsonObjectCreateValue(&root, "user_id"), platformPluginUtilUidGet());
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "stage_id"), this->stat->id);
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "stage_index"), this->stat->mark);
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "puppet_id"), this->stat->playerKind);
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "score"), this->stat->score);
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "damage"), this->stat->damage);

	struct engineDataJsonValue *seed = engineDataJsonObjectCreateValue(&root, "seed");
	char str1[16]; sprintf(str1, "%lu", this->stat->replaySeed[0]); engineDataJsonSetString(engineDataJsonArrayCreateValue(seed), str1);
	char str2[16]; sprintf(str2, "%lu", this->stat->replaySeed[1]); engineDataJsonSetString(engineDataJsonArrayCreateValue(seed), str2);
	char str3[16]; sprintf(str3, "%lu", this->stat->replaySeed[2]); engineDataJsonSetString(engineDataJsonArrayCreateValue(seed), str3);
	char str4[16]; sprintf(str4, "%lu", this->stat->replaySeed[3]); engineDataJsonSetString(engineDataJsonArrayCreateValue(seed), str4);

	byte *repList = (byte*)this->stat->gravityList;
	size_t repSize = this->stat->replayLength * sizeof(short);
	char *gravityList = engineDataBase64encode(repList, repSize);
	engineDataJsonSetString(engineDataJsonObjectCreateValue(&root, "gravityList"), gravityList);
	engineDataJsonSetInt(engineDataJsonObjectCreateValue(&root, "gravityLength"), this->stat->replayLength);

	char *json = engineDataJsonStringify(&root);
	engineDataJsonFree(&root);

	return json;
}

// 読み込み開始
static void load(struct popupCartridgeRollGameClear *this){
	if(this->stat->replayId < 0){
		// リプレイ保存通信開始
		engineDataRequestFreeAll();
		char *request = saveRequest(this);
		this->reqIdSave = engineDataRequestHttp("replay_save", request);
		engineUtilMemoryFree(request);
	}else{
		// SNS投稿開始
		char post[140];
		snprintf(post, sizeof(post), "Gururin!! %s/replay/%lld", gamePluginUtilUrlGet(), this->stat->replayId);
		utilTwitter(post);
	}
}

// 読み込み完了
static int loaded(struct popupCartridgeRollGameClear *this){
	if(this->stat->replayId < 0){
		// 読み込み確認
		size_t size;
		byte *loadedSave;
		bool isErrorSave = !engineDataRequestResponse(this->reqIdSave, (void**)&loadedSave, &size);
		isErrorSave = isErrorSave || (loadedSave == NULL);
		if(isErrorSave){return PARTSPOPUPLOADING_LOADED_RETRY;}

		// 読み込み成功 json化
		struct engineDataJsonValue jsonSave;
		engineDataJsonParse(&jsonSave, loadedSave, size);
		engineDataRequestFree(this->reqIdSave);

		// リプレイ再生用ID
		char *key = engineDataJsonGetString(engineDataJsonObjectGetValue(&jsonSave, "key"), "");
		this->stat->replayId = strtoull(key, NULL, 0);
		if(this->stat->isHighScore){
			// ハイスコア時はリプレイ再生用IDを保存
			this->stat->storage.replayId = this->stat->replayId;
			storageStageSet(&this->stat->storage, NULL);
			storageCommit();
		}

		//engineDataJsonTrace(&jsonSave);
		engineDataJsonFree(&jsonSave);

		// リプレイ保存完了からのSNS投稿開始
		load(this);
		return PARTSPOPUPLOADING_LOADED_PENDING;
	}else{
		// SNS投稿完了
		return PARTSPOPUPLOADING_LOADED_OK;
	}
}

// 読み込み中止
static void cancel(struct popupCartridgeRollGameClear *this){
	engineDataRequestFreeAll();
}

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeRollGameClear *this){
	this->super.openStepMax = 0;
	// ボタン作成
	partsButtonHexInit(&this->btnRetry);
	partsButtonHexInit(&this->btnBack);
	partsButtonHexInit(&this->btnNext);
	partsButtonHexInit(&this->btnPostBig);
	partsButtonHexInit(&this->btnPostSmall);
	partsButtonInit(&this->btnWindow);
	// ステージエンブレム初期化
	partsEmblemInitStage(&this->emblem1, this->stat->icon, this->stat->mark, 0, false, false);
	partsEmblemInitStage(&this->emblem2, 0, 0, this->stat->score, false, false);
	// 報酬初期化
	objectRollRewordInit(&this->reword, this->stat);
	// BGMをちいさくする
	soundBgmToneDown();
}

// ----------------------------------------------------------------

// 減衰計算
static void calcDamper(struct popupCartridgeRollGameClear *this){
	// カメラ位置の減衰計算
	this->stat->camerax -= 0.1 * (this->stat->camerax - this->stat->cameraxNext);
	this->stat->cameray -= 0.1 * (this->stat->cameray - this->stat->camerayNext);
	// カメラ回転角度の減衰計算
	this->stat->cameraRot = utilCalcRotateDamper(this->stat->cameraRot, this->stat->cameraRotNext, 0.2);
}

// ボタン計算
static bool calcButton(struct popupCartridgeRollGameClear *this){
	bool isSkipable = (this->step < 350);

	// 最初からボタン押下確認
	if(this->btnRetry.super.trigger){
		this->btnRetry.super.trigger = false;
		soundSePlayOK();
		// ポップアップ閉じる
		this->result = 0;
		this->super.exist = false;
		return true;
	}

	// 戻るボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		if(!isSkipable){
			soundSePlayNG();
			// ポップアップ閉じる
			this->result = -1;
			this->super.exist = false;
			return true;
		}
	}

	// 次へボタン押下確認
	if(this->btnNext.super.trigger){
		this->btnNext.super.trigger = false;
		soundSePlayOK();
		// ポップアップ閉じる
		this->result = 1;
		this->super.exist = false;
		return true;
	}

	// 投稿ボタン押下確認
	if(this->btnPostBig.super.trigger || this->btnPostSmall.super.trigger){
		this->btnPostBig.super.trigger = false;
		this->btnPostSmall.super.trigger = false;
		soundSePlayOK();
		// 読み込み開始
		popupManagerPush(&this->popup, partsPopupLoading(this, (void(*)(void*))load, (int(*)(void*))loaded, (void(*)(void*))cancel), NULL);
		return true;
	}

	// 画面押下確認
	if(this->btnWindow.trigger){
		this->btnWindow.trigger = false;
		if(isSkipable){
			// スキップ
			this->step = 350;
		}
	}

	return false;
}

// キャラクター計算
static void calcChara(struct popupCartridgeRollGameClear *this, struct pageRollStatusChara *chara){
	if(this->step == 1){chara->countDamage = 0;}

	if(this->step > 30){
		// 水平軸角度計算
		double drh = chara->roth - this->stat->cameraRot;
		while(drh < -CONST_PI){drh += CONST_PI * 2;}
		while(drh >  CONST_PI){drh -= CONST_PI * 2;}
		if(utilMathAbs(drh) > 0.01){chara->roth -= drh * 0.1;}
	}

	if(this->step == 60){chara->pose = HKNWCHARAPOSESID_SQUAT;}
	if(this->step == 80){chara->pose = HKNWCHARAPOSESID_STAND;}

	if(this->step > 100){
		// 垂直軸角度計算
		double drv = chara->rotv - CONST_PI * ((this->step < 300) ? 0 : 0.5);
		while(drv < -CONST_PI){drv += CONST_PI * 2;}
		while(drv >  CONST_PI){drv -= CONST_PI * 2;}
		if(utilMathAbs(drv) > 0.01){chara->rotv -= drv * 0.1;}
	}

	if(this->step == 350){chara->step = 0; chara->pose = HKNWCHARAPOSESID_HAPPY;}
	if(this->step > 350){chara->step++;}
}

// 計算
static void calc(struct popupCartridgeRollGameClear *this){
	// ポップアップ処理
	if(popupManagerCalc(&this->popup, this)){return;}

	calcDamper(this);
	pageRollDamperCamera(this->stat);	

	// タッチ処理
	partsButtonHexCalc(&this->btnRetry);
	partsButtonHexCalc(&this->btnBack);
	partsButtonHexCalc(&this->btnNext);
	partsButtonHexCalc(&this->btnPostBig);
	partsButtonHexCalc(&this->btnPostSmall);
	partsButtonCalc(&this->btnWindow);

	// ボタン処理
	if(calcButton(this)){return;}

	// プレイヤー計算
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		calcChara(this, chara);
	}

	// 報酬計算
	objectRollRewordCalc(&this->reword);

	this->step++;
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeRollGameClear *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// 画面全体ボタン
		partsButtonPosition(&this->btnWindow, 0, 0, global.screen.w, global.screen.h);

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transBack);
		e3dTransInit(&this->transEmblem1);
		e3dTransInit(&this->transEmblem2);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		this->transBack.parent = &this->transRoot;
		this->transEmblem1.parent = &this->transRoot;
		this->transEmblem2.parent = &this->transRoot;
		// 変形描画構造体登録準備
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// 暗幕
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transBack, 0);
		e3dImageWhiteCreateArray(&transImage->image, 0, 0, global.screen.w, global.screen.h);
		e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnRetry,     global.screen.w * 1.0 - 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONRETRY_XYWH);
		partsButtonHexCreateArray(&this->btnBack,      global.screen.w * 0.0 + 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		partsButtonHexCreateArray(&this->btnNext,      global.screen.w * 0.5     , global.screen.h - 70, 50, TEXPOS_SYSTEM_ICONNEXT_XYWH);
		partsButtonHexCreateArray(&this->btnPostBig,   global.screen.w * 1.0 - 55,          51, 50, TEXPOS_SYSTEM_ICONTWIT1_XYWH);
		partsButtonHexCreateArray(&this->btnPostSmall, global.screen.w * 1.0 - 30,          26, 25, TEXPOS_SYSTEM_ICONTWIT2_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnRetry.trans, &this->transRoot, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnNext.trans, &this->transRoot, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnPostBig.trans, &this->transRoot, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnPostSmall.trans, &this->transRoot, 1);
		// ボタン表示条件設定
		this->btnRetry.trans.isVisible = !this->stat->isReplayStorage;
		this->btnBack.trans.isVisible = !utilEditedExist() && !this->stat->isReplayWeb;
		this->btnNext.trans.isVisible = !utilEditedExist() && !this->stat->isReplayWeb && !this->stat->isReplayStorage && (this->stat->nextStageId > 0 || this->stat->nextStoryId > 0);
		this->btnPostBig.trans.isVisible = !utilEditedExist() && !this->stat->isReplayWeb && (!this->stat->isReplayStorage && this->stat->isHighScore);
		this->btnPostSmall.trans.isVisible = !utilEditedExist() && !this->stat->isReplayWeb && !(!this->stat->isReplayStorage && this->stat->isHighScore);
		// ボタン有効設定
		this->btnRetry.super.inactive = !this->btnRetry.trans.isVisible;
		this->btnBack.super.inactive = !this->btnBack.trans.isVisible;
		this->btnNext.super.inactive = !this->btnNext.trans.isVisible;
		this->btnPostBig.super.inactive = !this->btnPostBig.trans.isVisible;
		this->btnPostSmall.super.inactive = !this->btnPostSmall.trans.isVisible;

		// ステージエンブレム作成
		partsEmblemCreateArray(&this->emblem1, 70, 50, 0.0);
		partsEmblemCreateArray(&this->emblem2, 70, 50, 0.0);
		e3dTransListManagerPushProperty(&this->transManager, &this->emblem1.trans, &this->transEmblem1, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->emblem2.trans, &this->transEmblem2, 1);

		// 報酬作成
		objectRollRewordCreateArray(&this->reword);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeRollGameClear *this){
	// 描画準備
	createBuffer(this);

	struct engineMathMatrix44 tempmat1;
	double param1 = (double)(this->step - 0) / (30 - 0);
	double param2 = (double)(this->step - 50) / (80 - 50);
	double param3 = (double)(this->step - 320) / (350 - 320);
	if(param1 < 0){param1 = 0;}else if(param1 > 1){param1 = 1;}
	if(param2 < 0){param2 = 0;}else if(param2 > 1){param2 = 1;}
	if(param3 < 0){param3 = 0;}else if(param3 > 1){param3 = 1;}

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetTranslate(&this->btnRetry.trans,  100 * (1 - param3) * (1 - param3), 0.0, 0.0);
	e3dTransSetTranslate(&this->btnBack.trans, -100 * (1 - param3) * (1 - param3), 0.0, 0.0);
	e3dTransSetTranslate(&this->btnNext.trans, 0.0, 160 * (1 - param3) * (1 - param3), 0.0);
	e3dTransSetTranslate(&this->btnPostBig.trans, 0.0, -115 * (1 - param3) * (1 - param3), 0.0);
	e3dTransSetTranslate(&this->btnPostSmall.trans, 0.0, -90 * (1 - param3) * (1 - param3), 0.0);
	e3dTransSetTranslate(&this->transEmblem1, 0.0, -100 * (1 - param2) * (1 - param2), 0.0);
	e3dTransSetTranslate(&this->transEmblem2, 0.0, -200 * (1 - param3) * (1 - param3), 0.0);
	e3dTransSetColor(&this->btnRetry.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->btnBack.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->btnNext.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->btnPostBig.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->btnPostSmall.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->transEmblem2, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, param1);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// プレイヤー描画
	engineMathMat4Copy(&tempmat1, &this->stat->cameraMat);
	double x = -100 * (1 - (1 - param2) * (1 - param2));
	double y =   30 * (1 - (1 - param2) * (1 - param2));
	double c = utilMathCos(this->stat->cameraRot);
	double s = utilMathSin(this->stat->cameraRot);
	engineMathMat4Translate(&tempmat1, x * c - y * s, x * s + y * c, 0.0);
	engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
	engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
	engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
	engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		pageRollDrawChara(this->stat, chara, &tempmat1, 1.0);
	}

	// 報酬描画
	engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
	engineGraphicEngineBindVertVBO(this->e3dIdVert);
	engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
	engineGraphicEngineBindFaceIBO(this->e3dIdFace);
	engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
	engineMathMat4Translate(&tempmat1, global.screen.w * 0.5 + 130 + x, global.screen.h * 0.5 + 10 + y, 0.0);
	objectRollRewordDraw(&this->reword, &tempmat1, param2);

	// ポップアップ描画
	popupManagerDraw(&this->popup);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeRollGameClear *this){
	popupManagerPause(&this->popup);
}

// 破棄
static void dispose(struct popupCartridgeRollGameClear *this){
	popupManagerDispose(&this->popup);
	e3dTransListManagerDispose(&this->transManager);
	objectRollRewordDispose(&this->reword);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *popupRollGameClear(struct pageRollStatus *stat){
	struct popupCartridgeRollGameClear *this = (struct popupCartridgeRollGameClear*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeRollGameClear));
	this->super.exist = true;
	this->stat = stat;

	struct popupCartridge *cartridge = (struct popupCartridge*)this;
	cartridge->init = (void(*)(struct popupCartridge*))init;
	cartridge->calc = (void(*)(struct popupCartridge*))calc;
	cartridge->draw = (void(*)(struct popupCartridge*))draw;
	cartridge->pause = (void(*)(struct popupCartridge*))pause;
	cartridge->dispose = (void(*)(struct popupCartridge*))dispose;
	return cartridge;
}

// ポップアップ結果
int popupRollGameClearResult(struct popupCartridge *that){
	struct popupCartridgeRollGameClear *this = (struct popupCartridgeRollGameClear*)that;
	return this->result;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

