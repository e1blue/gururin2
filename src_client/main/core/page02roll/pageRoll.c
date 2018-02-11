#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジ構造体
struct pageCartridgeRoll{
	struct pageCartridge super;
	struct pageRollStatus *stat;

	struct popupManager popup;
	struct partsButtonHex btnPause;

	struct e3dTrans transRoot;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;
	struct e3dTransListManager transManagerEffect;

	struct e3dImage imgMask;
	struct e3dImage imgWallpaper1;
	struct e3dImage imgWallpaper2;
	struct e3dImage imgItem[2][2];
	struct e3dImage imgHeart;
	struct e3dImage imgRing;
	struct e3dImage imgMarker;
	struct e3dImage imgHitArea;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	struct{
		double x;
	}heartDraw[12];
	int damage;

	// パペット解放スコア
	int *openStarPuppetList;
	int openStarPuppetLength;

	// 処理落ち防止の効果音カウント
	struct{
		int coin;
		int attack;
		int damage;
		int dash;
	} soundSeCount;

	bool isStart;
	int reqIdStage;
	int reqIdProgress;
	int step;
	double paramButton;
	int shakeStep;
	double shakePowerx;
	double shakePowery;
};

// ----------------------------------------------------------------

// 読み込み開始
static void load(struct pageCartridgeRoll *this){
	objectRollParserReset(this->stat);
	this->isStart = false;
	// 通信開始
	engineDataRequestFreeAll();
	if(utilEditedExist()){
		this->reqIdStage = 0;
	}else{
		char path[32];
		snprintf(path, sizeof(path), "dat/stage%04d.json", this->stat->id);
		this->reqIdStage = engineDataRequestLocal(path);
	}
	this->reqIdProgress = engineDataRequestLocal("dat/progress.json");
}

// パペット解放データ解釈
static void parseProgress(struct pageCartridgeRoll *this, byte *json, size_t size){
	// json準備
	struct engineDataJsonValue jsonProgress;
	engineDataJsonParse(&jsonProgress, json, size);

	// ステージindex確認
	int selectIndex = 0;
	int stageIndex = 0;
	int storyIndex = 0;
	int lockedSelectIndex = utilLockedSelectIndex();
	struct engineDataJsonValue *jsonSelectList = engineDataJsonObjectGetValue(&jsonProgress, "selectList");
	struct engineDataJsonArray *selectArr = (jsonSelectList->type == ENGINEDATAJSONTYPE_ARRAY) ? jsonSelectList->jArray : NULL;
	while(selectArr != NULL){
		char *type = engineDataJsonGetString(engineDataJsonObjectGetValue(&selectArr->value, "type"), NULL);
		int id = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "id"), 1);
		bool isLock = (0 < lockedSelectIndex && lockedSelectIndex <= selectIndex);
		if(!strcmp(type, "stage")){
			// メニュー画面での選択位置を設定
			bool isOpen = (this->stat->progress.openStage[stageIndex] & 0x01);
			if(this->stat->id == id && isOpen){if(settingSelectSet(selectIndex)){storageCommit();}}
			// 次ステージのindexをidに変換
			if(this->stat->openStageIndex == stageIndex && !isLock){this->stat->nextStageId = id;}
			stageIndex++;
		}else if(!strcmp(type, "story")){
			// 次ストーリーのindexをidに変換 解放されていない場合のみ
			bool isOpen = (this->stat->progress.openStory[storyIndex] & 0x01);
			if(this->stat->openStoryIndex == storyIndex && !isLock && !isOpen){this->stat->nextStoryId = id;}
			storyIndex++;
		}

		selectIndex++;
		selectArr = selectArr->next;
	}

	// パペットデータの数を数える
	struct engineDataJsonValue *jsonPuppetList = engineDataJsonObjectGetValue(&jsonProgress, "puppetList");
	this->openStarPuppetLength = engineDataJsonArrayGetLength(jsonPuppetList);
	this->openStarPuppetList = (int*)engineUtilMemoryCalloc(this->openStarPuppetLength, sizeof(int));
	// パペット解放スコア読み込み
	int puppetIndex = 0;
	struct engineDataJsonArray *puppetArr = (jsonPuppetList->type == ENGINEDATAJSONTYPE_ARRAY) ? jsonPuppetList->jArray : NULL;
	while(puppetArr != NULL){
		int openStar = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&puppetArr->value, "openStar"), 0);
		this->openStarPuppetList[puppetIndex++] = openStar;
		puppetArr = puppetArr->next;
	}
	this->openStarPuppetLength = puppetIndex;

	// jsonの解放
	//engineDataJsonTrace(&jsonProgress);
	engineDataJsonFree(&jsonProgress);
}

// 読み込み完了
static int loaded(struct pageCartridgeRoll *this){
	// 読み込み確認
	size_t sizeStage;
	size_t sizeProgress;
	byte *loadedStage;
	byte *loadedProgress;
	bool isErrorStage = true;
	bool isErrorProgress = true;
	if(utilEditedExist()){
		char *edited = utilEditedGet();
		sizeStage = strlen(edited);
		loadedStage = (byte*)edited;
		isErrorStage = false;
	}else{
		isErrorStage = !engineDataRequestResponse(this->reqIdStage, (void**)&loadedStage, &sizeStage);
		isErrorStage = isErrorStage || (loadedStage == NULL);
	}
	isErrorProgress = !engineDataRequestResponse(this->reqIdProgress, (void**)&loadedProgress, &sizeProgress);
	isErrorProgress = isErrorProgress || (loadedProgress == NULL);
	if(isErrorStage || isErrorProgress){return PARTSPOPUPLOADING_LOADED_RETRY;}

	// 読み込み成功 ステージデータ解釈
	objectRollParserParse(this->stat, loadedStage, sizeStage);
	if(utilEditedExist()){engineUtilMemoryFree(loadedStage);}else{engineDataRequestFree(this->reqIdStage);}
	// パペット解放データ解釈
	parseProgress(this, loadedProgress, sizeProgress);
	engineDataRequestFree(this->reqIdProgress);

	// 体力設定
	int healthMax = sizeof(this->heartDraw) / sizeof(*this->heartDraw);
	if(this->stat->health > healthMax){this->stat->health = healthMax;}
	for(int i = 0; i < healthMax; i++){
		this->heartDraw[i].x = 1.1;
	}
	this->damage = 0;

	// 画面振動のリセット
	this->shakeStep = 0;
	this->shakePowerx = 0;
	this->shakePowery = 0;

	return PARTSPOPUPLOADING_LOADED_OK;
}

// ----------------------------------------------------------------

// 読込ポップアップ後の処理挿入
static void onClosePopupNothingLoading(struct pageCartridgeRoll *this, struct popupCartridge *cartridge){
	this->isStart = true;
}

// ステージ読み込み開始
static void loadStage(struct pageCartridgeRoll *this){
	storageStageGet(&this->stat->storage, this->stat->id);
	popupManagerPush(&this->popup, partsPopupLoading(this, (void(*)(void*))load, (int(*)(void*))loaded, NULL), NULL);
	// 処理挿入ポップアップ展開
	popupManagerPush(&this->popup, partsPopupNothing(), (void(*)(void*, struct popupCartridge*))onClosePopupNothingLoading);
	// 開始演出ポップアップ展開
	popupManagerPush(&this->popup, popupRollStart(this->stat), NULL);
}

// ----------------------------------------------------------------

// 初期化
static void init(struct pageCartridgeRoll *this){
	// ボタン作成
	partsButtonHexInit(&this->btnPause);
	// エフェクト初期化
	objectRollEffectManagerInit(&this->stat->effectManager, this->stat, &this->transManagerEffect);
	// 進捗情報獲得
	storageProgressGet(&this->stat->progress);
	// ステージ読み込み開始
	loadStage(this);
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct pageCartridgeRoll *this){
	static int prevsx;
	static int prevsy;
	// タッチ状態の確認
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_TEST1);
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				prevsx = t->screen.x;
				prevsy = t->screen.y;
				engineCtrlTouchOwn();
			}

			if(t->mv){
				// フィールド回転処理
				double cx = global.screen.w * 0.5;
				double cy = global.screen.h * 0.5;
				double x0 = prevsx - cx;
				double y0 = prevsy - cy;
				double r0 = utilMathSqrt(x0 * x0 + y0 * y0);
				double x1 = t->screen.x - cx;
				double y1 = t->screen.y - cy;
				double r1 = utilMathSqrt(x1 * x1 + y1 * y1);
				if(r0 > 20 && r1 > 20){
					double cos = (x0 * x1 + y0 * y1) / (r0 * r1);
					if(cos > 1){cos = 1;}else if(cos < -1){cos = -1;}
					this->stat->cameraRotNext -= 0.8 * utilMathAcos(cos) * ((x0 * y1 - y0 * x1 > 0) ? 1 : -1);
				}
			}
			prevsx = t->screen.x;
			prevsy = t->screen.y;
		}else{
			// タッチ終了時
			engineCtrlTouchFree();
		}
	}
}

// ページを戻る
static void backPage(struct pageCartridgeRoll *this){
	if(utilEditedExist()){
		// 終了
		engineUtilSystemExit();
	}else{
		// ページ遷移 メニューに戻る
		pageMenuInitSelect();
		this->super.exist = false;
	}
}

// 停止ポップアップ完了時
static void onClosePopupPause(struct pageCartridgeRoll *this, struct popupCartridge *cartridge){
	int result = popupRollPauseResult(cartridge);
	if(result < 0){
		// ページを戻る
		backPage(this);
	}else if(result > 0){
		// リトライ
		loadStage(this);
	}
}

// ボタン計算
static bool calcButton(struct pageCartridgeRoll *this){
	// 停止ボタン押下確認
	if(this->btnPause.super.trigger || global.key.bk.triggerInactive){
		this->btnPause.super.trigger = false;
		global.key.bk.triggerInactive = false;
		soundSePlayOK();
		// 停止ポップアップ展開
		popupManagerPush(&this->popup, popupRollPause(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupPause);
		return true;
	}

	return false;
}

// 減衰計算
static void calcDamper1(struct pageCartridgeRoll *this){
	// ボタン演出計算
	bool isButton = (this->popup.popupCartridgeList == NULL && !this->stat->isReplayWeb);
	this->paramButton -= 0.1 * (this->paramButton - (isButton ? 1.0 : 0.0));
}

// 減衰計算
static void calcDamper2(struct pageCartridgeRoll *this){
	// 表示用重力方向の減衰計算
	this->stat->dispDeviceRot = utilCalcRotateDamper(this->stat->dispDeviceRot, this->stat->deviceRot, 0.2);
	this->stat->dispGravityRot = utilCalcRotateDamper(this->stat->dispGravityRot, this->stat->gravityRot, 0.2);

	// 体力描画情報の減衰計算
	int healthMax = sizeof(this->heartDraw) / sizeof(*this->heartDraw);
	int num = (int)utilMathCeil(this->stat->health);
	for(int i = 0; i < healthMax; i++){
		double x = (1.0 / (num * 2)) * (i * 2 + 1);
		this->heartDraw[i].x -= 0.1 * (this->heartDraw[i].x - x);
	}

	// 画面振動の減衰計算
	if(this->shakeStep > 0){
		double shakePowerMax = this->shakeStep;
		this->shakePowerx = shakePowerMax * 1.0 * ((int)(utilRandomGet() % 201) - 100) * 0.01;
		this->shakePowery = shakePowerMax * 1.0 * ((int)(utilRandomGet() % 201) - 100) * 0.01;
		this->shakeStep--;
	}else{
		this->shakePowerx = 0;
		this->shakePowery = 0;
	}
}

// カメラ減衰計算
void pageRollDamperCamera(struct pageRollStatus *this){
	// カメラ位置をプレイヤー位置
	double playerx = 0;
	double playery = 0;
	int playerNum = 0;
	for(int i = 0; i < this->charaLength; i++){
		struct pageRollStatusChara *chara = &this->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		playerx += chara->data.x;
		playery += chara->data.y;
		playerNum++;
	}
	this->cameraxNext = (playerNum > 0) ? (playerx / playerNum) : 0;
	this->camerayNext = (playerNum > 0) ? (playery / playerNum) : 0;
	// プレイヤー距離によりカメラを離す
	double playerRr = 0;
	for(int i = 0; i < this->charaLength; i++){
		struct pageRollStatusChara *chara = &this->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		double x = chara->data.x - this->camerax;
		double y = chara->data.y - this->cameray;
		double rr = x * x + y * y;
		if(playerRr < rr){playerRr = rr;}
	}
	double playerR = (playerRr > 0) ? utilMathSqrt(playerRr) : 0;
	this->cameraScaleNext = (playerR > 100) ? (100 / playerR) : 1.0;

	// カメラ位置の減衰計算
	this->camerax -= 0.1 * (this->camerax - this->cameraxNext);
	this->cameray -= 0.1 * (this->cameray - this->camerayNext);
	// カメラ回転角度の減衰計算
	this->cameraRot = utilCalcRotateDamper(this->cameraRot, this->cameraRotNext, 0.2);
	// カメラ大きさの減衰計算
	this->cameraScale -= 0.1 * (this->cameraScale - this->cameraScaleNext);
}

// 結果演出ポップアップ完了時
static void onClosePopupGameFinish(struct pageCartridgeRoll *this, int result){
	if(result < 0){
		// ページを戻る
		backPage(this);
	}else if(result > 0){
		if(this->stat->nextStoryId > 0){
			// ページ遷移 次のストーリーへ
			pageStoryInit(this->stat->nextStoryId, this->stat->nextStageId);
			this->super.exist = false;
		}else if(this->stat->nextStageId > 0){
			// 次のステージへ
			this->stat->id = this->stat->nextStageId;
			loadStage(this);
		}
	}else{
		// リトライ
		loadStage(this);
	}
}
static void onClosePopupGameClear(struct pageCartridgeRoll *this, struct popupCartridge *cartridge){onClosePopupGameFinish(this, popupRollGameClearResult(cartridge));}
static void onClosePopupGameOver(struct pageCartridgeRoll *this, struct popupCartridge *cartridge){onClosePopupGameFinish(this, popupRollGameOverResult(cartridge));}

// ゲームクリア時計算
static void calcGameClear(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara){
	// スコア計算
	int score = 0;
	score += this->stat->getCoin * 50;
	score += this->stat->getStar * 1000;
	score += this->stat->getPlayerStar * 10;
	score += this->stat->getEnemyStar * 20;
	score += (int)utilMathFloor(this->stat->health * 100);
	this->stat->getHealth = (int)utilMathCeil(this->stat->health);
	// ゲームクリアステータス
	this->stat->score = score;
	this->stat->damage = this->damage;
	this->stat->replayLength = this->stat->gravityListIndex;
	this->stat->replayId = -1;
	this->stat->isHighScore = false;

	bool isReplay = (this->stat->isReplayWeb || this->stat->isReplayStorage);
	if(!utilEditedExist() && !isReplay){
		// 編集ステージやリプレイモード以外での保存処理
		bool isCommit = false;

		if(this->stat->score > (int)this->stat->storage.score){
			// ハイスコア更新したのでデータ記録
			this->stat->isHighScore = true;
			this->stat->progress.totalStar += (int)utilMathFloor(this->stat->score / 1000) - (int)utilMathFloor(this->stat->storage.score / 1000);
			this->stat->storage.score = this->stat->score;
			this->stat->storage.replayId = -1;
			this->stat->storage.replaySeed[0] = this->stat->replaySeed[0];
			this->stat->storage.replaySeed[1] = this->stat->replaySeed[1];
			this->stat->storage.replaySeed[2] = this->stat->replaySeed[2];
			this->stat->storage.replaySeed[3] = this->stat->replaySeed[3];
			this->stat->storage.replayPuppet = this->stat->playerKind;
			this->stat->storage.replayLength = this->stat->replayLength;
			storageStageSet(&this->stat->storage, this->stat->gravityList);
			isCommit = true;
		}

		// ステージとか解放
		signed char *openStage = this->stat->openStageIndex >= 0 ? &this->stat->progress.openStage[this->stat->openStageIndex] : NULL;
		signed char *openStory = this->stat->openStoryIndex >= 0 ? &this->stat->progress.openStory[this->stat->openStoryIndex] : NULL;
		signed char *openPuppet = this->stat->openPuppetIndex >= 0 ? &this->stat->progress.openPuppet[this->stat->openPuppetIndex] : NULL;
		bool isStage = (openStage != NULL && !(*openStage & 0x01));
		bool isStory = (openStory != NULL && !(*openStory & 0x01));
		bool isPuppet = (openPuppet != NULL && !(*openPuppet & 0x01));
		if(isStage || isStory || isPuppet){
			if(isStage){*openStage = 0x01;}
			if(isStory){*openStory = 0x01;}
			if(isPuppet){*openPuppet = 0x01;}
			isCommit = true;
		}

		// スコアによるパペット解放
		for(int i = 0; i < this->openStarPuppetLength; i++){
			signed char *openPuppet = &this->stat->progress.openPuppet[i];
			int openStar = this->openStarPuppetList[i];
			if(!(*openPuppet & 0x01) && 0 < openStar && openStar <= (int)this->stat->progress.totalStar){
				*openPuppet = 0x01;
				isCommit = true;
			}
		}

		if(isCommit){
			storageProgressSet(&this->stat->progress);
			storageCommit();
		}
	}else if(this->stat->isReplayStorage){
		// ストレージ上のリプレイは必然的にハイスコア リプレイ再生用IDを同期する
		this->stat->isHighScore = true;
		this->stat->replayId = this->stat->storage.replayId;
	}

	// クリア演出用に最後の一人以外は消す
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara1 = &this->stat->charaList[i];
		if(chara1->team == 0 && chara1->hp > 0){chara1->hp = 0;}
	}
	chara->hp = 1;

	// ポップアップ展開 ゲームクリア
	popupManagerPush(&this->popup, popupRollGameClear(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupGameClear);
}

// 重力方向計算
static void calcGravity(struct pageCartridgeRoll *this){
	if(this->stat->gravityListIndex < this->stat->gravityListLength){
		// 端末傾きによる重力方向の変化
		if(100 * global.acc.x * global.acc.x + global.acc.y * global.acc.y > global.acc.z * global.acc.z){this->stat->deviceRot = utilMathAtan2(global.acc.y, global.acc.x) - CONST_PI * 0.5;}

		// 重力方向データ獲得
		bool isReplay = (this->stat->isReplayWeb || this->stat->isReplayStorage);
		short gravity = (short)((this->stat->deviceRot - this->stat->cameraRot) * 0x7ff);
		if(isReplay){gravity = this->stat->gravityList[this->stat->gravityListIndex];}
		//if(isReplay){trace("gravity %d/%d", (this->stat->gravityListIndex + 1), this->stat->gravityListLength);}
		else{this->stat->gravityList[this->stat->gravityListIndex] = gravity;}

		this->stat->gravityRot = (double)gravity / 0x7ff;
		this->stat->gravityCos = utilMathCos(this->stat->gravityRot);
		this->stat->gravitySin = utilMathSin(this->stat->gravityRot);

		this->stat->gravityListIndex++;
	}else{
		// リプレイバッファオーバーラン ここに来ることは無いはず
		//if(this->stat->gravityListIndex == this->stat->gravityListLength){
		//	char *test = (char*)engineUtilMemoryCalloc(3 + 9 * this->stat->gravityListLength, sizeof(char));
		//	test[0] = '{';
		//	for(int i = 0; i < this->stat->gravityListLength; i++){sprintf(test + 1 + i * 9, "% 8d,", this->stat->gravityList[i]);}
		//	test[1 + this->stat->gravityListLength * 9] = '}';
		//	test[1 + this->stat->gravityListLength * 9 + 1] = '\0';
		//	trace("%s", test);
		//	engineUtilMemoryFree(test);
		//	this->stat->gravityListIndex++;
		//}
		// とりあえずポップアップ展開 ゲームオーバーにしておく
		popupManagerPush(&this->popup, popupRollGameOver(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupGameOver);
	}
}

// プレイヤー計算
static void calcPlayer(struct pageCartridgeRoll *this){
	if(this->stat->health > 0){
		// 時間で体力を減らす 1体力1200フレーム
		this->stat->health -= (double)1 / 1200;
	}else{
		// 体力がなくなったらポップアップ展開 ゲームオーバー
		popupManagerPush(&this->popup, popupRollGameOver(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupGameOver);
	}
}

// アイテム獲得計算
static void calcItem(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara, struct pageRollStatusItem *item){
	if(!chara->isExist || !item->isExist){return;}
	if(chara->hp <= 0){return;}

	double x = chara->data.x - item->x;
	double y = chara->data.y - item->y;
	double rr = x * x + y * y;
	double r0 = chara->data.rad;
	if(rr >= r0 * r0){return;}

	// アイテム取得効果音
	if(this->soundSeCount.coin <= 0){this->soundSeCount.coin = 5; soundSePlayCoin();}

	int coinType = 0;
	int starType = 1;
	item->isExist = false;

	if(item->type == coinType){
		// 得点アイテム所得
		this->stat->getCoin += 1;
	}else if(item->type == starType){
		// 星アイテム取得
		this->stat->getStar += 1;
		// 星アイテム存在確認
		bool isStar0 = false;
		bool isStar1 = false;
		for(int i = 0; i < this->stat->itemLength; i++){
			struct pageRollStatusItem *item = &this->stat->itemList[i];
			if(item->type == starType){isStar0 = true; if(item->isExist){isStar1 = true; break;}}
		}
		// すべての星アイテムを取ったらクリア
		if(isStar0 && !isStar1){calcGameClear(this, chara);}
	}
}

// プレイヤーへのダメージ
static void calcDamagePlayer(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara){
	if(chara->team != 0 || chara->hp <= 0){return;}

	// 痛恨効果音
	if(this->soundSeCount.damage <= 0){this->soundSeCount.damage = 5; soundSePlayDamage();}

	chara->hp--;
	this->stat->health = (int)utilMathCeil(this->stat->health - 1);
	this->damage++;
	this->shakeStep = 10;
	if(chara->hp <= 0){
		// プレイヤーやられる
		chara->countDamage = 0;
		chara->data.vx = -3.0 * this->stat->gravitySin;
		chara->data.vy = -3.0 * this->stat->gravityCos;

		// プレイヤー存在確認
		bool isPlayer = false;
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageRollStatusChara *chara1 = &this->stat->charaList[i];
			if(chara1->team == 0 && chara1->hp > 0){isPlayer = true; break;}
		}
		if(!isPlayer){
			// プレイヤーがいないのでポップアップ展開 ゲームオーバー 演出用に最後の一人だけ生き返らす
			chara->hp = 1;
			popupManagerPush(&this->popup, popupRollGameOver(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupGameOver);
		}
	}
}

// エネミーへのダメージ
static void calcDamageEnemy(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara){
	if(chara->team == 0 || chara->hp <= 0){return;}

	// 会心効果音
	if(this->soundSeCount.attack <= 0){this->soundSeCount.attack = 5; soundSePlayAttack();}

	chara->hp--;
	if(chara->hp <= 0){
		// エネミー撃破
		chara->data.vx = -3.0 * this->stat->gravitySin;
		chara->data.vy = -3.0 * this->stat->gravityCos;
	}
}

// キャラクター相互作用計算
static void calcChara1(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara1, struct pageRollStatusChara *chara2){
	if(!chara1->isExist || !chara2->isExist){return;}
	if(chara1->hp <= 0 || chara2->hp <= 0){return;}

	// 衝突反発
	double x = chara1->data.x - chara2->data.x;
	double y = chara1->data.y - chara2->data.y;
	double rr = x * x + y * y;
	double r0 = chara1->data.rad + chara2->data.rad;
	if(rr <= 0 || r0 * r0 <= rr){return;}
	double ir = 1 / utilMathSqrt(rr);
	x *= ir;
	y *= ir;
	double vx = x * r0 * 0.05;
	double vy = y * r0 * 0.05;
	chara1->data.vx += vx;
	chara1->data.vy += vy;
	chara2->data.vx -= vx;
	chara2->data.vy -= vy;

	// 上からのキック攻撃
	if(chara1->team == chara2->team){return;}
	bool isKick1 = ((+x * chara1->data.nx + +y * chara1->data.ny) > (CONST_SQRT2 * 0.5));
	bool isKick2 = ((-x * chara2->data.nx + -y * chara2->data.ny) > (CONST_SQRT2 * 0.5));
	if(isKick1 && !isKick2 && chara2->countDamage <= 0){
		chara2->countDamage = 60;
		calcDamagePlayer(this, chara2);
		calcDamageEnemy(this, chara2);
		objectRollEffectManagerPushStar(&this->stat->effectManager, (chara2->team == 0) ? 1 : 2, chara2->data.x, chara2->data.y);
		*(chara2->team == 0 ? &this->stat->getPlayerStar : &this->stat->getEnemyStar) += 2;
	}
	if(isKick2 && !isKick1 && chara1->countDamage <= 0){
		chara1->countDamage = 60;
		calcDamagePlayer(this, chara1);
		calcDamageEnemy(this, chara1);
		objectRollEffectManagerPushStar(&this->stat->effectManager, (chara1->team == 0) ? 1 : 2, chara1->data.x, chara1->data.y);
		*(chara1->team == 0 ? &this->stat->getPlayerStar : &this->stat->getEnemyStar) += 2;
	}
}

// キャラクター速度位置姿勢計算
static void calcChara2(struct pageCartridgeRoll *this, struct pageRollStatusChara *chara){
	if(!chara->isExist){return;}

	// 重力による加速計算
	chara->data.vx += this->stat->gravityPower * this->stat->gravitySin;
	chara->data.vy += this->stat->gravityPower * this->stat->gravityCos;

	if(chara->hp > 0){
		// 衝突計算
		struct objectRollMap *map = this->stat->mapList[chara->mapIndex];
		objectRollMapCalcHex(map, &chara->data);
		if(chara->countDamage <= 0){
			if(objectRollMapCalcTrapNeedle(map, &chara->data)){
				// 棘によるダメージ
				chara->countDamage = 60;
				if(chara->team == 0){
					// ダメージはプレイヤーのみ受ける
					calcDamagePlayer(this, chara);
					objectRollEffectManagerPushStar(&this->stat->effectManager, 1, chara->data.x, chara->data.y);
					this->stat->getPlayerStar += 2;
				}
			}
		}
		if(objectRollMapCalcTrapAccelerator(map, &chara->data)){
			// 加速効果音
			if(this->soundSeCount.dash <= 0){this->soundSeCount.dash = 10; soundSePlayDash();}
		}
		chara->isGround = objectRollMapCalcCollision(map, &chara->data);

		// 接地確認
		if(chara->isGround){chara->countGround++;}
		else{chara->countGround = 0;}

		// ダメージ無敵確認
		if(chara->countDamage > 0){chara->countDamage--;}
	}

	// 最大速度をオーバーしない
	double vmax = chara->data.rad * 0.9;
	double vrr = chara->data.vx * chara->data.vx + chara->data.vy * chara->data.vy;
	if(vrr > vmax * vmax){
		double ir = vmax / utilMathSqrt(vrr);
		chara->data.vx *= ir;
		chara->data.vy *= ir;
	}

	// 位置計算
	chara->data.x += chara->data.vx;
	chara->data.y += chara->data.vy;

	if(chara->hp > 0){
		// 水平垂直方向の速度確認
		double vh = chara->data.vx * this->stat->gravityCos - chara->data.vy * this->stat->gravitySin;
		double vv = chara->data.vx * this->stat->gravitySin + chara->data.vy * this->stat->gravityCos;

		// 垂直軸角度計算
		if(utilMathAbs(vh) > 0.3){
			double drv = chara->rotv - ((vh > 0) ? 0 : CONST_PI);
			while(drv < -CONST_PI){drv += CONST_PI * 2;}
			while(drv >  CONST_PI){drv -= CONST_PI * 2;}
			if(utilMathAbs(drv) > 0.01){chara->rotv -= drv * (chara->isGround ? 0.1 : 0.05);}
		}

		// 水平軸角度計算
		double drh = chara->roth + this->stat->gravityRot;
		while(drh < -CONST_PI){drh += CONST_PI * 2;}
		while(drh >  CONST_PI){drh -= CONST_PI * 2;}
		if(utilMathAbs(drh) > 0.01){chara->roth -= drh * 0.05;}
		// 垂直方向単位ベクトル計算
		chara->data.nx = utilMathCos(chara->roth - CONST_PI * 0.5);
		chara->data.ny = utilMathSin(chara->roth - CONST_PI * 0.5);

		// 態勢確認
		chara->pose = HKNWCHARAPOSESID_STAND;
		if(chara->countDamage > 0){
			// ダメージ無敵状態の姿勢
			chara->pose = HKNWCHARAPOSESID_FALL;
		}else if(chara->countGround <= 1){
			// 落下中の姿勢
			chara->pose = (vv < 0) ? HKNWCHARAPOSESID_JUMP : HKNWCHARAPOSESID_FALL;
		}else if(chara->countGround <= 20){
			// 着地時の姿勢
			chara->pose = HKNWCHARAPOSESID_SQUAT;
		}else if(utilMathAbs(vh) > 0.3){
			// 歩行の姿勢
			chara->pose = HKNWCHARAPOSESID_WALK;
		}
	}
	
	if(chara->hp <= 0){
		// 退場計算 消滅判定
		double x = (chara->data.x - this->stat->camerax) * this->stat->cameraScale;
		double y = (chara->data.y - this->stat->cameray) * this->stat->cameraScale;
		double s = (global.screen.w + global.screen.h) * 0.5 + chara->data.rad;
		chara->isExist = (-s < x && x < s && -s < y && y < s);
	}

	chara->step++;
}

// 計算
static void calc(struct pageCartridgeRoll *this){
	// メモリートレースボタン計算
	partsButtonMemoryCalc();

	// ポップアップ処理と減衰処理
	calcDamper1(this);
	if(popupManagerCalc(&this->popup, this)){return;}

	// リプレイ機能のため、このブロックの間に外的要因で処理を中断してはならない
	{
		calcDamper2(this);
		pageRollDamperCamera(this->stat);

		// タッチ処理
		partsButtonHexCalc(&this->btnPause);
		calcTouch(this);

		// 重力処理
		calcGravity(this);

		// プレイヤー計算
		calcPlayer(this);
		// キャラクター相互作用計算
		for(int i = 0; i < this->stat->charaLength; i++){
			for(int j = i + 1; j < this->stat->charaLength; j++){
				calcChara1(this, &this->stat->charaList[i], &this->stat->charaList[j]);
			}
			if(this->stat->charaList[i].team == 0){
				for(int j = 0; j < this->stat->itemLength; j++){
					calcItem(this, &this->stat->charaList[i], &this->stat->itemList[j]);
				}
			}
		}
		// キャラクター速度位置姿勢計算
		for(int i = 0; i < this->stat->charaLength; i++){calcChara2(this, &this->stat->charaList[i]);}

		// エフェクト処理
		objectRollEffectManagerCalc(&this->stat->effectManager);

		// 処理落ち防止の効果音カウント
		this->soundSeCount.coin--;
		this->soundSeCount.attack--;
		this->soundSeCount.damage--;
		this->soundSeCount.dash--;

		this->step++;
	}

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// グルリンリング作成
static void createArrayRing(struct pageCartridgeRoll *this){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int divide = 64;
	double r = (global.screen.w < global.screen.h ? global.screen.w : global.screen.h) * 0.5 * 1.2; // グルリンリングサイズ
	for(int i = 0; i < divide; i++){
		bool isReplay = (this->stat->isReplayWeb || this->stat->isReplayStorage);
		double t1 = CONST_PI * 2 * (i - 0.5) / divide;
		double t2 = CONST_PI * 2 * (i + 0.5) / divide;
		double c1 = utilMathCos(t1); double s1 = utilMathSin(t1);
		double c2 = utilMathCos(t2); double s2 = utilMathSin(t2);
		double r1 = r - 10 * (i % 2);
		double r2 = isReplay ? ((global.screen.w + global.screen.h) * 0.5) : (r + 10);
		engineGraphicBufferPushVert(r1 * c1, r1 * s1, 0.0);
		engineGraphicBufferPushVert(r2 * c1, r2 * s1, 0.0);
		engineGraphicBufferPushVert(r2 * c2, r2 * s2, 0.0);
		engineGraphicBufferPushVert(r1 * c2, r1 * s2, 0.0);
		engineGraphicBufferPushTexcWhite(); engineGraphicBufferPushTexcWhite();
		engineGraphicBufferPushTexcWhite(); engineGraphicBufferPushTexcWhite();
		engineGraphicBufferPushFace(vertIndex, i * 4 + 0, i * 4 + 1, i * 4 + 2);
		engineGraphicBufferPushFace(vertIndex, i * 4 + 2, i * 4 + 3, i * 4 + 0);
	}
	this->imgRing.faceIndex = faceIndex;
	this->imgRing.faceNum = divide * 2;
}

// グルリンマーカー作成
static void createArrayMarker(struct pageCartridgeRoll *this){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();

	engineGraphicBufferPushVert(  0.0,  0.0, 0.0);
	engineGraphicBufferPushVert( 10.0, 10.0, 0.0);
	engineGraphicBufferPushVert( 10.0, 20.0, 0.0);
	engineGraphicBufferPushVert(-10.0, 20.0, 0.0);
	engineGraphicBufferPushVert(-10.0, 10.0, 0.0);
	engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushFace(vertIndex, 0, 1, 4);
	engineGraphicBufferPushFace(vertIndex, 1, 2, 3);
	engineGraphicBufferPushFace(vertIndex, 3, 4, 1);

	this->imgMarker.faceIndex = faceIndex;
	this->imgMarker.faceNum = 3;
}

// あたり判定範囲作成
static void createArrayHitArea(struct pageCartridgeRoll *this){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int faceNum = 32;
	engineGraphicBufferPushVert(0.0, 0.0, 0.0);
	engineGraphicBufferPushTexcWhite();
	for(int i = 0; i < faceNum; i++){
		double t = CONST_PI * 2 * i / faceNum;
		engineGraphicBufferPushVert(utilMathCos(t), utilMathSin(t), 0.0);
		engineGraphicBufferPushTexcWhite();
		engineGraphicBufferPushFace(vertIndex, 0, 1 + i, 1 + (i + 1) % faceNum);
	}
	this->imgHitArea.faceIndex = faceIndex;
	this->imgHitArea.faceNum = faceNum;
}

// バッファ作成
static void createBuffer(struct pageCartridgeRoll *this){
	bool isStart = (this->isStart && this->stat->mapLength > 0);
	int status = 1 | ((isStart ? 0x01 : 0x00) << 1);
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		// 変形描画構造体登録準備
		//int imgPoolIndex = 0;
		//struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		if(isStart){
			// ボタン作成
			partsButtonHexCreateArray(&this->btnPause, 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONSTOP_XYWH);
			e3dTransListManagerPushProperty(&this->transManager, &this->btnPause.trans, &this->transRoot, 0);		

			// 表示マスク作成
			e3dImageWhiteCreateArray(&this->imgMask, 0, 0, global.screen.w, global.screen.h);

			// 壁紙作成
			partsWallpaperCreateArray(&this->imgWallpaper1, this->stat->wall1, NULL, NULL);
			partsWallpaperCreateArray(&this->imgWallpaper2, this->stat->wall2, NULL, NULL);

			// アイテム作成
			e3dImageCreateArray(&this->imgItem[0][0], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_ITEMCOIN1_XYWH);
			e3dImageCreateArray(&this->imgItem[0][1], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_ITEMCOIN2_XYWH);
			e3dImageCreateArray(&this->imgItem[1][0], TEXSIZ_SYSTEM_WH, -18, -18, 36, 36, TEXPOS_SYSTEM_ITEMSTAR1_XYWH);
			e3dImageCreateArray(&this->imgItem[1][1], TEXSIZ_SYSTEM_WH, -18, -18, 36, 36, TEXPOS_SYSTEM_ITEMSTAR2_XYWH);
			// ハート作成
			e3dImageCreateArray(&this->imgHeart, TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_PARAMHEART_XYWH);

			// エフェクト作成
			objectRollEffectManagerCreateArray(&this->stat->effectManager);

			// グルリンリング作成
			createArrayRing(this);
			createArrayMarker(this);

			// あたり判定範囲作成
			createArrayHitArea(this);

			// マップ作成
			objectRollMapCreateArray(this->stat->mapList[0]);
		}

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// キャラクター描画
void pageRollDrawChara(struct pageRollStatus *this, struct pageRollStatusChara *chara, struct engineMathMatrix44 *mat, double alpha){
	struct engineMathMatrix44 tempmat1;
	struct hknwChara *draw = hknwCharaManagerGet(&this->charaManager, chara->kind);
	if(!chara->isExist){return;}
	if(chara->countDamage > 0 && (chara->countDamage / 3) % 2 != 0){return;}
	// キャラクターポーズステップ
	int step = 0;
	switch(chara->pose){
		case HKNWCHARAPOSESID_WALK: step = (chara->step / 12) % hknwCharaGetPoseStepNum(draw, chara->pose); break;
		case HKNWCHARAPOSESID_HAPPY: step = (chara->step / 8) % hknwCharaGetPoseStepNum(draw, chara->pose); break;
		default: break;
	}
	// キャラクター描画
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, chara->data.x, chara->data.y, 0.0);
	engineMathMat4RotateZ(&tempmat1, chara->roth);
	engineMathMat4Translate(&tempmat1, 0.0, chara->data.rad, 0.0);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, alpha);
	hknwCharaDrawChara2D(draw, &tempmat1, chara->rotv, chara->scale, chara->pose, step);
}

// グルリンリング描画
static void drawRing(struct pageCartridgeRoll *this){
	bool isReplay = (this->stat->isReplayWeb || this->stat->isReplayStorage);
	struct engineMathMatrix44 tempmat1;
	double r = (global.screen.w < global.screen.h ? global.screen.w : global.screen.h) * 0.5 * 1.2;
	double s = 1;
	if(this->step < 10){
		double param = ((double)this->step / 10);
		r *= (2 - param);
		s *= (2 - param);
	}
	// グルリンリング
	engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
	engineMathMat4Translate(&tempmat1, global.screen.w * 0.5, global.screen.h * 0.5, 0.0);
	engineMathMat4Translate(&tempmat1, -this->shakePowerx, -this->shakePowery, 0.0);
	engineMathMat4RotateZ(&tempmat1, -this->stat->dispGravityRot);
	engineMathMat4Scale(&tempmat1, s, s, 1);
	engineGraphicEngineSetMatrix(&tempmat1);
	engineGraphicEngineSetColorRgba(isReplay ? 0.0 : 1.0, isReplay ? 0.0 : 1.0, isReplay ? 0.0 : 1.0, 0.5);
	e3dImageDraw(&this->imgRing);
	// グルリンマーカー
	double rot = isReplay ? (this->stat->dispGravityRot + this->stat->cameraRot) : this->stat->dispDeviceRot;
	double x = global.screen.w * 0.5 + r * utilMathCos(-rot + CONST_DEG2RAD * 90);
	double y = global.screen.h * 0.5 + r * utilMathSin(-rot + CONST_DEG2RAD * 90);
	if(x < 0){x = 0;}else if(x > global.screen.w){x = global.screen.w;}
	if(y < 0){y = 0;}else if(y > global.screen.h){y = global.screen.h;}
	engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
	engineMathMat4Translate(&tempmat1, this->shakePowerx, this->shakePowery, 0.0);
	engineMathMat4Translate(&tempmat1, x, y, 0.0);
	engineMathMat4RotateZ(&tempmat1, -rot + CONST_DEG2RAD * 180);
	engineGraphicEngineSetMatrix(&tempmat1);
	engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 0.5);
	e3dImageDraw(&this->imgMarker);
}

// 体力描画
static void drawHeart(struct pageCartridgeRoll *this){
	struct engineMathMatrix44 tempmat1;
	int healthMax = sizeof(this->heartDraw) / sizeof(*this->heartDraw);
	//int num = (int)utilMathCeil(this->stat->health);
	double rot = 0.5 * utilMathSin(this->step * 0.1);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	for(int i = 0; i < healthMax; i++){
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, this->heartDraw[i].x * global.screen.w, 15, 0.0);
		engineMathMat4RotateZ(&tempmat1, rot);
		engineGraphicEngineSetMatrix(&tempmat1);
		e3dImageDraw(&this->imgHeart);
	}
}

// 描画
static void draw(struct pageCartridgeRoll *this){
	engineGraphicEngineClearAll();
	engineGraphicStencilClear();
	engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_NONE);

	// 描画準備
	createBuffer(this);
	hknwCharaManagerCreateBuffer(&this->stat->charaManager);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	bool isStart = (this->isStart && this->stat->mapLength > 0);
	if(isStart){
		struct engineMathMatrix44 tempmat1;
		struct objectRollMap *map = this->stat->mapList[0];
		// 射影行列作成
		engineMathMat4Ortho(&e3dMatOrtho, -global.screen.offset.x, global.screen.w + global.screen.offset.x, global.screen.h + global.screen.offset.y, -global.screen.offset.y, -1, 1);
		// カメラ行列作成
		engineMathMat4Copy(&this->stat->cameraMat, &e3dMatOrtho);
		engineMathMat4Translate(&this->stat->cameraMat, global.screen.w * 0.5, global.screen.h * 0.5, 0.0);
		engineMathMat4RotateZ(&this->stat->cameraMat, -this->stat->cameraRot);
		engineMathMat4Scale(&this->stat->cameraMat, this->stat->cameraScale, this->stat->cameraScale, 1);
		engineMathMat4Translate(&this->stat->cameraMat, -this->stat->camerax, -this->stat->cameray, 0.0);
		engineMathMat4Translate(&this->stat->cameraMat, this->shakePowerx, this->shakePowery, 0.0);

		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
		engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);

		// 全体マスク作成
		engineGraphicEngineSetMatrix(&e3dMatOrtho);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_MASK_1);
		e3dImageDraw(&this->imgMask);

		engineGraphicEngineSetMatrix(&this->stat->cameraMat);
		engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 1.0);
		// 背景マスク作成
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1_MASK_2);
		e3dImageDraw(&map->imgBack);
		// 壁描画
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1_WRITE_0);
		e3dImageDraw(&map->imgWall);
		// 背景描画
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, -((this->step / 2) % 32), -((this->step / 8) % 32), 0.0);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1);
		e3dImageDraw(&this->imgWallpaper2);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ2);
		e3dImageDraw(&this->imgWallpaper1);

		// 地形トラップ描画
		engineGraphicEngineSetMatrix(&this->stat->cameraMat);
		engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
		e3dImageDraw(&map->imgTrap[(this->step / 10) % 4]);

		// アイテム描画
		for(int i = 0; i < this->stat->itemLength; i++){
			struct pageRollStatusItem *item = &this->stat->itemList[i];
			if(!item->isExist){continue;}
			engineMathMat4Copy(&tempmat1, &this->stat->cameraMat);
			engineMathMat4Translate(&tempmat1, item->x, item->y, 0.0);
			engineMathMat4RotateZ(&tempmat1, this->stat->cameraRot);
			engineGraphicEngineSetMatrix(&tempmat1);
			e3dImageDraw(&this->imgItem[item->type][(this->step / 10) % 2]);
		}

//		if(false){
//			// あたり判定描画
//			for(int i = 0; i < this->stat->charaLength; i++){
//				struct pageRollStatusChara *chara = &this->stat->charaList[i];
//				if(!chara->isExist){continue;}
//				engineMathMat4Copy(&tempmat1, &this->stat->cameraMat);
//				engineMathMat4Translate(&tempmat1, chara->data.x, chara->data.y, 0.0);
//				engineMathMat4Scale(&tempmat1, chara->data.rad, chara->data.rad, 1);
//				engineGraphicEngineSetMatrix(&tempmat1);
//				e3dImageDraw(&this->imgHitArea);
//			}
//		}

		// 生存キャラクター描画
		engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
		engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
		engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageRollStatusChara *chara = &this->stat->charaList[i];
			if(chara->hp <= 0){continue;}
			pageRollDrawChara(this->stat, chara, &this->stat->cameraMat, 1.0);
		}
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);

		// 全体マスク再描画
		engineGraphicEngineSetMatrix(&e3dMatOrtho);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_MASK_1);
		e3dImageDraw(&this->imgMask);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1);

		// 退場キャラクター描画
		engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
		engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
		engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageRollStatusChara *chara = &this->stat->charaList[i];
			if(chara->hp > 0){continue;}
			pageRollDrawChara(this->stat, chara, &this->stat->cameraMat, 1.0);
		}
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);

		// エフェクトリスト描画
		e3dTransListManagerDraw(&this->transManagerEffect, &this->stat->cameraMat, 1.0, 1.0 ,1.0, 1.0);

		// グルリンリング描画
		drawRing(this);
		// 体力描画
		drawHeart(this);

		// リスト設定
		e3dTransSetTranslate(&this->btnPause.trans, -100 * (1 - this->paramButton), 0.0, 0.0);

		// リスト描画
		e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

		// ステンシルマスク解除
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_NONE);
	}

	// ポップアップ描画
	popupManagerDraw(&this->popup);

	// メモリートレースボタン描画
	partsButtonMemoryDraw();

	engineGraphicEngineFlush();
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct pageCartridgeRoll *this){
	popupManagerPause(&this->popup);
}

// 破棄
static void dispose(struct pageCartridgeRoll *this){
	// 乱数を任意の値に変更していた場合は開始前の乱数に戻す
	if(this->stat->isSetSeed){utilRandomSetSeed(this->stat->beforeSeed); this->stat->isSetSeed = false;}

	popupManagerDispose(&this->popup);
	objectRollParserDispose(this->stat);
	e3dTransListManagerDispose(&this->transManager);
	e3dTransListManagerDispose(&this->transManagerEffect);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->openStarPuppetList);
	engineUtilMemoryFree(this->stat);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ページカートリッジ作成
struct pageCartridge *cartridgeRoll(struct pageRollStatus *stat){
	struct pageCartridgeRoll *this = (struct pageCartridgeRoll*)engineUtilMemoryCalloc(1, sizeof(struct pageCartridgeRoll));
	this->super.exist = true;
	this->stat = stat;

	struct pageCartridge *cartridge = (struct pageCartridge*)this;
	cartridge->init = (void(*)(struct pageCartridge*))init;
	cartridge->calc = (void(*)(struct pageCartridge*))calc;
	cartridge->draw = (void(*)(struct pageCartridge*))draw;
	cartridge->pause = (void(*)(struct pageCartridge*))pause;
	cartridge->dispose = (void(*)(struct pageCartridge*))dispose;
	return cartridge;
}

// ページ状態初期化
void pageRollInit(int id, bool isReplay){
	struct pageRollStatus *stat = (struct pageRollStatus*)engineUtilMemoryCalloc(1, sizeof(struct pageRollStatus));

	if(utilReplayExist()){
		// webリプレイモード
		stat->id = utilReplayGetStageId();
		stat->isReplayWeb = true;
		stat->isReplayStorage = false;
	}else{
		// 通常モードもしくはローカルリプレイモード
		stat->id = id;
		stat->isReplayWeb = false;
		stat->isReplayStorage = isReplay;
	}

	pageCartridgePush(cartridgeRoll(stat));
}
void pageRollInitTest(){pageRollInit(1, false);}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

