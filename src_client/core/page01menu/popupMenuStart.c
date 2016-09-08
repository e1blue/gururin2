#include "../library.h"
#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeMenuStart{
	struct popupCartridge super;
	struct pageMenuStatus *stat;

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
static void init(struct popupCartridgeMenuStart *this){
	this->super.openStepMax = 0;
}

// ----------------------------------------------------------------

// 計算
static void calc(struct popupCartridgeMenuStart *this){
	this->super.exist = (this->step++ < 10);

	// 戻るボタン押下確認
	if(global.key.bk.triggerInactive){
		global.key.bk.triggerInactive = false;
		// 何も起きない
	}
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct popupCartridgeMenuStart *this){
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
static void draw(struct popupCartridgeMenuStart *this){
	// 描画準備
	createBuffer(this);

	double param = (double)this->step / 10;

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, param * param);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeMenuStart *this){
}

// 破棄
static void dispose(struct popupCartridgeMenuStart *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
static struct popupCartridge *popupMenuStart(struct pageMenuStatus *stat){
	struct popupCartridgeMenuStart *this = (struct popupCartridgeMenuStart*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeMenuStart));
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

// ステージ開始待ちポップアップ登録
void popupMenuStartStageOpen(struct pageMenuStatus *stat, int id, bool isReplay){
	pageRollInit(id, isReplay);
	popupManagerPush(&stat->popup, popupMenuStart(stat), NULL);
}

// ストーリー開始待ちポップアップ登録
void popupMenuStartStoryOpen(struct pageMenuStatus *stat, int id){
	pageStoryInit(id, 0);
	popupManagerPush(&stat->popup, popupMenuStart(stat), NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

