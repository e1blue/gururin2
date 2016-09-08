#include "../library.h"
#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeRollStart{
	struct popupCartridge super;
	struct pageRollStatus *stat;

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int step;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeRollStart *this){
	this->super.openStepMax = 0;
}

// ----------------------------------------------------------------

// 計算
static void calc(struct popupCartridgeRollStart *this){
	// ゲーム開始準備が完了していなければスキップ
	if(this->stat->mapList == NULL){return;}

	pageRollDamperCamera(this->stat);

	// プレイヤー計算
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		chara->pose = HKNWCHARAPOSESID_FALL;
	}

	this->super.exist = (this->step++ < 90);

	// 戻るボタン押下確認
	if(global.key.bk.triggerInactive){
		global.key.bk.triggerInactive = false;
		// 何も起きない
	}
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeRollStart *this){
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
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		this->transBack.parent = &this->transRoot;
		// 変形描画構造体登録準備
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// 暗幕
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transBack, 0);
		e3dImageWhiteCreateArray(&transImage->image, 0, 0, global.screen.w, global.screen.h);
		e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeRollStart *this){
	// 描画準備
	createBuffer(this);

	struct engineMathMatrix44 tempmat1;
	double param1 = (double)(this->step -  0) / (40 - 0);
	double param2 = (double)(this->step - 60) / (90 - 60);
	if(param1 < 0){param1 = 0;}else if(param1 > 1){param1 = 1;}
	if(param2 < 0){param2 = 0;}else if(param2 > 1){param2 = 1;}

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, 1 - param2);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// プレイヤー描画
	engineMathMat4Copy(&tempmat1, &this->stat->cameraMat);
	engineMathMat4Translate(&tempmat1, 0.0, -100 * (1 - param1 * param1), 0.0);
	engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
	engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
	engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
	engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
	for(int i = 0; i < this->stat->charaLength; i++){
		struct pageRollStatusChara *chara = &this->stat->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		pageRollDrawChara(this->stat, chara, &tempmat1, param1);
	}	
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeRollStart *this){
}

// 破棄
static void dispose(struct popupCartridgeRollStart *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *popupRollStart(struct pageRollStatus *stat){
	struct popupCartridgeRollStart *this = (struct popupCartridgeRollStart*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeRollStart));
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

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

