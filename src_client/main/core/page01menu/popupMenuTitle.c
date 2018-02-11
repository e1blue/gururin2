#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeMenuTitle{
	struct popupCartridge super;
	struct pageMenuStatus *stat;

	struct partsButtonHex btnHex1;
	struct partsButtonHex btnHex2;
	struct partsButtonHex btnHex3;

	struct e3dTrans transRoot;
	struct e3dTrans transWindow;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeMenuTitle *this){
	// ボタン作成
	partsButtonHexInit(&this->btnHex1);
	partsButtonHexInit(&this->btnHex2);
	partsButtonHexInit(&this->btnHex3);
}

// ----------------------------------------------------------------

// ボタン計算
static bool calcButton(struct popupCartridgeMenuTitle *this){
	// ボタン押下確認
	if(this->btnHex1.super.trigger){
		this->btnHex1.super.trigger = false;
		soundSePlayOK();
		// 選択ポップアップ展開
		popupMenuSelectOpen(this->stat);
		// ポップアップ閉じる
		this->super.exist = false;
		return true;
	}

	// ボタン押下確認
	if(this->btnHex2.super.trigger){
		this->btnHex2.super.trigger = false;
		soundSePlayOK();
		// キャラクタポップアップ展開
		popupMenuPlayerOpen(this->stat);
		// ポップアップ閉じる
		this->super.exist = false;
		return true;
	}

	// ボタン押下確認
	if(this->btnHex3.super.trigger){
		this->btnHex3.super.trigger = false;
		soundSePlayOK();
		// 設定ポップアップ展開
		popupMenuSettingOpen(this->stat);
		// ポップアップ閉じる
		this->super.exist = false;
		return true;
	}

	// 戻るボタン押下確認
	if(global.key.bk.triggerInactive){
		global.key.bk.triggerInactive = false;
		soundSePlayNG();
		// 終了
		engineUtilSystemExit();
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeMenuTitle *this){
	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnHex1);
	partsButtonHexCalc(&this->btnHex2);
	partsButtonHexCalc(&this->btnHex3);

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeMenuTitle *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transWindow);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transWindow, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		this->transWindow.parent = &this->transRoot;
		// 変形描画構造体登録準備
		//int imgPoolIndex = 0;
		//struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnHex1, 55 * CONST_COS240, 55 * CONST_SIN240, 50, TEXPOS_SYSTEM_ICONSELECT_XYWH);
		partsButtonHexCreateArray(&this->btnHex2, 55 * CONST_COS000, 55 * CONST_SIN000, 50, TEXPOS_SYSTEM_ICONPUPPET_XYWH);
		partsButtonHexCreateArray(&this->btnHex3, 55 * CONST_COS120, 55 * CONST_SIN120, 50, TEXPOS_SYSTEM_ICONSETTING_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnHex1.trans, &this->transWindow, 0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnHex2.trans, &this->transWindow, 0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnHex3.trans, &this->transWindow, 0);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeMenuTitle *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetScale(&this->transWindow, this->super.openSize, this->super.openSize, 1.0);
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeMenuTitle *this){
}

// 破棄
static void dispose(struct popupCartridgeMenuTitle *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
static struct popupCartridge *popupMenuTitle(struct pageMenuStatus *stat){
	struct popupCartridgeMenuTitle *this = (struct popupCartridgeMenuTitle*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeMenuTitle));
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

// ポップアップ登録
void popupMenuTitleOpen(struct pageMenuStatus *stat){
	popupManagerPush(&stat->popup, popupMenuTitle(stat), NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

