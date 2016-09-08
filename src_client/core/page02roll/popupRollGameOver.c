#include "../library.h"
#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeRollGameOver{
	struct popupCartridge super;
	struct pageRollStatus *stat;

	struct partsButtonHex btnRetry;
	struct partsButtonHex btnBack;
	struct partsEmblem emblem;

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTrans transButton;
	struct e3dTrans transEmblem;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int step;
	int result;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeRollGameOver *this){
	this->super.openStepMax = 0;
	// ボタン作成
	partsButtonHexInit(&this->btnRetry);
	partsButtonHexInit(&this->btnBack);
	// ステージエンブレム初期化
	partsEmblemInitStage(&this->emblem, this->stat->icon, this->stat->mark, 0, false, false);
	// BGMをちいさくする
	soundBgmToneDown();

}

// ----------------------------------------------------------------

// 減衰計算
static void calcDamper(struct popupCartridgeRollGameOver *this){
	// カメラ位置の減衰計算
	this->stat->camerax -= 0.1 * (this->stat->camerax - this->stat->cameraxNext);
	this->stat->cameray -= 0.1 * (this->stat->cameray - this->stat->camerayNext);
	// カメラ回転角度の減衰計算
	this->stat->cameraRot = utilCalcRotateDamper(this->stat->cameraRot, this->stat->cameraRotNext, 0.2);
}

// ボタン計算
static bool calcButton(struct popupCartridgeRollGameOver *this){
	bool isPlayable = (this->step > 160);
	
	// 最初からボタン押下確認
	if(this->btnRetry.super.trigger){
		this->btnRetry.super.trigger = false;
		if(isPlayable){
			soundSePlayOK();
			// ポップアップ閉じる
			this->result = 0;
			this->super.exist = false;
			return true;
		}
	}

	// 戻るボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		if(isPlayable){
			soundSePlayNG();
			// ポップアップ閉じる
			this->result = -1;
			this->super.exist = false;
			return true;
		}
	}

	return false;
}

// キャラクター計算
static void calcChara(struct popupCartridgeRollGameOver *this, struct pageRollStatusChara *chara){
	if(this->step == 1){chara->countDamage = 0;}

	if(this->step > 30){
		// 水平軸角度計算
		double drh = chara->roth - this->stat->cameraRot;
		while(drh < -CONST_PI){drh += CONST_PI * 2;}
		while(drh >  CONST_PI){drh -= CONST_PI * 2;}
		if(utilMathAbs(drh) > 0.01){chara->roth -= drh * 0.1;}
	}

	if(this->step == 50){chara->pose = HKNWCHARAPOSESID_BREAK1;}

	if(this->step > 60){
		// 中心から離れすぎていた場合に位置調整
		double x = chara->data.x - this->stat->camerax;
		double y = chara->data.y - this->stat->cameray;
		double rr = x * x + y * y;
		double rmin = 30;
		if(rr > rmin * rmin){
			double r0 = utilMathSqrt(rr);
			double r1 = r0 - (r0 - rmin) * 0.1;
			chara->data.x = (x * r1 / r0) + this->stat->camerax;
			chara->data.y = (y * r1 / r0) + this->stat->cameray;
		}
	}

	// ぐるぐる
	if(60 <= this->step && this->step < 150){
		double param = (double)(this->step - 60) / (150 - 60);
		chara->rotv += CONST_DEG2RAD * 12 * (1 - param * param);
	}

	if(this->step == 160){chara->pose = HKNWCHARAPOSESID_BREAK2;}
}

// 計算
static void calc(struct popupCartridgeRollGameOver *this){
	calcDamper(this);
	pageRollDamperCamera(this->stat);

	// タッチ処理
	partsButtonHexCalc(&this->btnRetry);
	partsButtonHexCalc(&this->btnBack);

	// ボタン処理
	if(calcButton(this)){return;}

	// プレイヤー計算
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		calcChara(this, chara);
	}

	this->step++;
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeRollGameOver *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transBack);
		e3dTransInit(&this->transButton);
		e3dTransInit(&this->transEmblem);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transButton, 0.0, global.screen.h, 0.0);
		this->transBack.parent = &this->transRoot;
		this->transButton.parent = &this->transRoot;
		this->transEmblem.parent = &this->transRoot;
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
		partsButtonHexCreateArray(&this->btnRetry, global.screen.w * 1.0 - 40, -36, 35, TEXPOS_SYSTEM_ICONRETRY_XYWH);
		partsButtonHexCreateArray(&this->btnBack,  global.screen.w * 0.0 + 40, -36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnRetry.trans, &this->transButton, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transButton, 1);
		// ボタン表示条件設定
		this->btnRetry.trans.isVisible = !this->stat->isReplayStorage;
		this->btnBack.trans.isVisible = !utilEditedExist() && !this->stat->isReplayWeb;
		// ボタン有効設定
		this->btnRetry.super.inactive = !this->btnRetry.trans.isVisible;
		this->btnBack.super.inactive = !this->btnBack.trans.isVisible;

		// ステージエンブレム作成
		partsEmblemCreateArray(&this->emblem, 70, 50, 0.0);
		e3dTransListManagerPushProperty(&this->transManager, &this->emblem.trans, &this->transEmblem, 1);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeRollGameOver *this){
	// 描画準備
	createBuffer(this);

	double param1 = (double)(this->step - 0) / (30 - 0);
	double param2 = (double)(this->step - 50) / (80 - 50);
	double param3 = (double)(this->step - 150) / (180 - 150);
	if(param1 < 0){param1 = 0;}else if(param1 > 1){param1 = 1;}
	if(param2 < 0){param2 = 0;}else if(param2 > 1){param2 = 1;}
	if(param3 < 0){param3 = 0;}else if(param3 > 1){param3 = 1;}

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetTranslate(&this->btnRetry.trans,  100 * (1 - param3) * (1 - param3), 0.0, 0.0);
	e3dTransSetTranslate(&this->btnBack.trans, -100 * (1 - param3) * (1 - param3), 0.0, 0.0);
	e3dTransSetTranslate(&this->transEmblem, 0.0, -100 * (1 - param2) * (1 - param2), 0.0);
	e3dTransSetColor(&this->btnRetry.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->btnBack.trans, 1.0, 1.0, 1.0, param3);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, param1);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// プレイヤー描画
	engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
	engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
	engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
	engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		pageRollDrawChara(this->stat, chara, &this->stat->cameraMat, 1.0);
	}	
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeRollGameOver *this){
}

// 破棄
static void dispose(struct popupCartridgeRollGameOver *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *popupRollGameOver(struct pageRollStatus *stat){
	struct popupCartridgeRollGameOver *this = (struct popupCartridgeRollGameOver*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeRollGameOver));
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
int popupRollGameOverResult(struct popupCartridge *that){
	struct popupCartridgeRollGameOver *this = (struct popupCartridgeRollGameOver*)that;
	return this->result;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

