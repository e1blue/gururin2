#include "../includeAll.h"
#include "pageTest.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeTestSample{
	struct popupCartridge super;
	struct pageTestStatus *stat;

	struct partsButtonBox btnOk;
	struct partsButtonBox btnClose;
	struct partsButton btnOuter;

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
static void init(struct popupCartridgeTestSample *this){
	// ボタン作成
	partsButtonBoxInit(&this->btnOk);
	partsButtonBoxInit(&this->btnClose);
	partsButtonInit(&this->btnOuter);
	this->btnOuter.inner = false;
}

// ----------------------------------------------------------------

// ボタン計算
static bool calcButton(struct popupCartridgeTestSample *this){
	// ボタン押下確認
	if(this->btnOk.super.trigger){
		this->btnOk.super.trigger = false;
		// ポップアップ閉じる
		this->result = 1;
		this->super.exist = false;
		return true;
	}

	// ボタン押下確認
	if(this->btnClose.super.trigger || this->btnOuter.trigger || global.key.bk.triggerInactive){
		this->btnClose.super.trigger = false;
		this->btnOuter.trigger = false;
		global.key.bk.triggerInactive = false;
		// ポップアップ閉じる
		this->result = 0;
		this->super.exist = false;
		return true;
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeTestSample *this){
	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonBoxCalc(&this->btnOk);
	partsButtonBoxCalc(&this->btnClose);
	partsButtonCalc(&this->btnOuter);

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeTestSample *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		int ww = 150;
		int wh = 100;
		int wx = ww * -0.5;
		int wy = wh * -0.5;

		// ウインドウ外ボタン位置
		partsButtonPosition(&this->btnOuter, (global.screen.w - ww) * 0.5, (global.screen.h - wh) * 0.5, ww, wh);

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
		// ウインドウ
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 1);
		e3dImageBoxDefaultCreateArray(&transImage->image, wx, wy, ww, wh);
		// メッセージ
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 2);
		e3dImageTextCreateArray(&transImage->image, wx + 10, wy + 10, "ウインドウですよ!\nプロデューサーさん!", 1.0, 1, 1);
		e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);

		// ボタン作成
		int bw = 60;
		int bh = 30;
		int sp = (ww - 2 * bw) / 3;
		partsButtonBoxCreateArray(&this->btnOk, wx + sp + bw + sp, wy + wh - 5 - bh, bw, bh, "OK", 1.0);
		partsButtonBoxCreateArray(&this->btnClose, wx + sp, wy + wh - 5 - bh, bw, bh, "とじる", 1.0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnOk.trans, &this->transWindow, 2);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnClose.trans, &this->transWindow, 2);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeTestSample *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetScale(&this->transWindow, this->super.openSize, this->super.openSize, 1.0);
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, this->super.backAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeTestSample *this){
}

// 破棄
static void dispose(struct popupCartridgeTestSample *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *popupTestSample(struct pageTestStatus *stat){
	struct popupCartridgeTestSample *this = (struct popupCartridgeTestSample*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeTestSample));
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
int popupTestSampleResult(struct popupCartridge *that){
	struct popupCartridgeTestSample *this = (struct popupCartridgeTestSample*)that;
	return this->result;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

