#include "../library.h"
#include "../includeAll.h"
#include "pageTest.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジ構造体
struct pageCartridgeTest{
	struct pageCartridge super;
	struct pageTestStatus *stat;

	struct popupManager popup;
	struct partsButtonBox btnSample1;
	struct partsButtonBox btnSample2;

	struct e3dTrans transRoot;
	struct e3dTrans transRotate;
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
static void init(struct pageCartridgeTest *this){
	// ボタン作成
	partsButtonBoxInit(&this->btnSample1);
	partsButtonBoxInit(&this->btnSample2);
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct pageCartridgeTest *this){
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_TEST1);
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				engineCtrlTouchOwn();
			}

			// テスト タッチ位置確認
			trace("myon %d %d", t->screen.x, t->screen.y);
		}else{
			// タッチ終了時
			engineCtrlTouchFree();
		}
	}
}

// サンプルポップアップ完了時
static void onClosePopupSample(struct pageCartridgeTest *this, struct popupCartridge *cartridge){
	int result = popupTestSampleResult(cartridge);
	trace("popup %d", result);
	// test もう一回ポップアップ
	if(result == 1){
		popupManagerPush(&this->popup, popupTestSample(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupSample);
	}
}

// ボタン計算
static bool calcButton(struct pageCartridgeTest *this){
	// ボタン1押下確認
	if(this->btnSample1.super.trigger){
		this->btnSample1.super.trigger = false;
		// ポップアップ展開
		popupManagerPush(&this->popup, popupTestSample(this->stat), (void(*)(void*, struct popupCartridge*))onClosePopupSample);
		return true;
	}

	// ボタン2押下確認
	if(this->btnSample2.super.trigger){
		this->btnSample2.super.trigger = false;
		// ページ遷移
		pageRollInitTest();
		this->super.exist = false;
		return true;
	}

	// 戻るボタン押下確認
	if(global.key.bk.triggerInactive){
		global.key.bk.triggerInactive = false;
		// 終了
		platformMainSurfaceExit();
	}

	return false;
}

// 計算
static void calc(struct pageCartridgeTest *this){
	// メモリートレースボタン計算
	partsButtonMemoryCalc();

	// ポップアップ処理
	if(popupManagerCalc(&this->popup, this)){return;}

	// タッチ処理
	partsButtonBoxCalc(&this->btnSample1);
	partsButtonBoxCalc(&this->btnSample2);
	calcTouch(this);

	// ボタン処理
	if(calcButton(this)){return;}

	this->step++;
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct pageCartridgeTest *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transRotate);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transRotate, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		this->transRotate.parent = &this->transRoot;
		// 変形描画構造体登録準備
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// テスト画像作成
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transRotate, 0);
		e3dImageCreateArray(&transImage->image, TEXSIZ_SYSTEM_WH, 128 * -0.5, 128 * -0.5, 128, 128, TEXPOS_SYSTEM_TEST_XYWH);

		// ボタン作成
		partsButtonBoxCreateArray(&this->btnSample1, global.screen.w - 5 - 100, global.screen.h - 35 * 2, 100, 30, "テスト1", 1.0);
		partsButtonBoxCreateArray(&this->btnSample2, global.screen.w - 5 - 100, global.screen.h - 35 * 1, 100, 30, "テスト2", 1.0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnSample1.trans, &this->transRoot, 0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnSample2.trans, &this->transRoot, 0);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct pageCartridgeTest *this){
	engineGraphicEngineClearAll();

	// 描画準備
	createBuffer(this);

	// 射影行列作成
	engineMathMat4Ortho(&e3dMatOrtho, -global.screen.offset.x, global.screen.w + global.screen.offset.x, global.screen.h + global.screen.offset.y, -global.screen.offset.y, -1, 1);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetRotate(&this->transRotate, 0.0, 0.0, CONST_PI * 0.01 * this->step);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// ポップアップ描画
	popupManagerDraw(&this->popup);

	// メモリートレースボタン描画
	partsButtonMemoryDraw();

	engineGraphicEngineFlush();
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct pageCartridgeTest *this){
	popupManagerPause(&this->popup);
}

// 破棄
static void dispose(struct pageCartridgeTest *this){
	popupManagerDispose(&this->popup);
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->stat);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ページカートリッジ作成
static struct pageCartridge *cartridgeTest(struct pageTestStatus *stat){
	struct pageCartridgeTest *this = (struct pageCartridgeTest*)engineUtilMemoryCalloc(1, sizeof(struct pageCartridgeTest));
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
void pageTestInit(){
	struct pageTestStatus *stat = (struct pageTestStatus*)engineUtilMemoryCalloc(1, sizeof(struct pageTestStatus));
	pageCartridgePush(cartridgeTest(stat));
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

