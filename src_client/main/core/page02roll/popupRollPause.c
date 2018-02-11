#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeRollPause{
	struct popupCartridge super;
	struct pageRollStatus *stat;

	struct partsButtonHex btnResume;
	struct partsButtonHex btnRetry;
	struct partsButtonHex btnBack;

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTrans transWindow;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int result;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeRollPause *this){
	// ボタン作成
	partsButtonHexInit(&this->btnResume);
	partsButtonHexInit(&this->btnRetry);
	partsButtonHexInit(&this->btnBack);
}

// ----------------------------------------------------------------

// ボタン計算
static bool calcButton(struct popupCartridgeRollPause *this){
	// 再開ボタン押下確認
	if(this->btnResume.super.trigger){
		this->btnResume.super.trigger = false;
		soundSePlayOK();
		// ポップアップ閉じる
		this->result = 0;
		this->super.exist = false;
		return true;
	}

	// 最初からボタン押下確認
	if(this->btnRetry.super.trigger){
		this->btnRetry.super.trigger = false;
		soundSePlayOK();
		// ポップアップ閉じる
		this->result = 1;
		this->super.exist = false;
		return true;
	}

	// 戻るボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		soundSePlayNG();
		// ポップアップ閉じる
		this->result = -1;
		this->super.exist = false;
		return true;
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeRollPause *this){
	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnResume);
	partsButtonHexCalc(&this->btnRetry);
	partsButtonHexCalc(&this->btnBack);

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeRollPause *this){
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
		e3dTransInit(&this->transWindow);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transWindow, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		this->transBack.parent = &this->transRoot;
		this->transWindow.parent = &this->transRoot;
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
		partsButtonHexCreateArray(&this->btnResume,  30,   0, 65, TEXPOS_SYSTEM_ICONPLAY_XYWH);
		partsButtonHexCreateArray(&this->btnRetry, -60, -35, 35, TEXPOS_SYSTEM_ICONRETRY_XYWH);
		partsButtonHexCreateArray(&this->btnBack, -60,  35, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnResume.trans, &this->transWindow, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnRetry.trans, &this->transWindow, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transWindow, 1);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeRollPause *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	double param = this->super.openParam;
	if(utilEditedExist()){
		e3dTransSetTranslate(&this->btnResume.trans, -100 * param * param, 173 * param * param, 0.0);
		e3dTransSetTranslate(&this->btnRetry.trans, 100 * param * param, -173 * param * param + 5, 0.0);
		this->btnBack.trans.isVisible = false;
	}else if(this->stat->isReplayStorage){
		e3dTransSetTranslate(&this->btnResume.trans, -100 * param * param, -173 * param * param, 0.0);
		e3dTransSetTranslate(&this->btnBack.trans, 100 * param * param, 173 * param * param - 5, 0.0);
		this->btnRetry.trans.isVisible = false;
	}else{
		e3dTransSetTranslate(&this->btnResume.trans, 200 * param * param, 0.0, 0.0);
		e3dTransSetTranslate(&this->btnRetry.trans, -100 * param * param, -173 * param * param, 0.0);
		e3dTransSetTranslate(&this->btnBack.trans, -100 * param * param, 173 * param * param, 0.0);
	}
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, this->super.backAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeRollPause *this){
}

// 破棄
static void dispose(struct popupCartridgeRollPause *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *popupRollPause(struct pageRollStatus *stat){
	struct popupCartridgeRollPause *this = (struct popupCartridgeRollPause*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeRollPause));
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
int popupRollPauseResult(struct popupCartridge *that){
	struct popupCartridgeRollPause *this = (struct popupCartridgeRollPause*)that;
	return this->result;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

