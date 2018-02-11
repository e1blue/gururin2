#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#define POPUPMENUSETTING_MIN -50
#define POPUPMENUSETTING_MAX  90

// ポップアップカートリッジ構造体
struct popupCartridgeMenuSetting{
	struct popupCartridge super;
	struct pageMenuStatus *stat;

	struct partsButtonHex btnBack;
	struct partsButtonBox btnSlide[2];
	struct partsButton btnBorder[2];

	struct e3dTrans transRoot;
	struct e3dTrans transWindow;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int select;
	double volume[2];
	bool isSetting;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeMenuSetting *this){
	// ボタン作成
	partsButtonHexInit(&this->btnBack);
	partsButtonBoxInit(&this->btnSlide[0]);
	partsButtonBoxInit(&this->btnSlide[1]);
	partsButtonInit(&this->btnBorder[0]);
	partsButtonInit(&this->btnBorder[1]);
	// ボリューム取得
	this->volume[0] = settingBgmVolumeGet();
	this->volume[1] = settingSeVolumeGet();
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct popupCartridgeMenuSetting *this){
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_TEST1);
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				this->select = -1;
				if(partsButtonIsInner(&this->btnBorder[0], t)){this->select = 0;}
				if(partsButtonIsInner(&this->btnBorder[1], t)){this->select = 1;}
				if(this->select >= 0){this->btnSlide[this->select].super.active = true;}
				engineCtrlTouchOwn();
			}

			if(this->select >= 0){
				// 音量の設定
				double sx = t->screen.x - (global.screen.w * 0.5);
				double max = POPUPMENUSETTING_MAX;
				double min = POPUPMENUSETTING_MIN;
				if(sx < min){sx = min;}
				if(sx > max){sx = max;}
				this->volume[this->select] = (sx - min) / (max - min);
			}
		}else{
			// タッチ終了時
			if(this->select == 0){if(settingBgmVolumeSet(this->volume[0])){this->isSetting = true;}}
			if(this->select == 1){if(settingSeVolumeSet(this->volume[1])){this->isSetting = true;}}
			if(this->select >= 0){this->btnSlide[this->select].super.active = false;}
			this->select = -1;
			engineCtrlTouchFree();
		}
	}
}

// ボタン計算
static bool calcButton(struct popupCartridgeMenuSetting *this){
	// ボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		soundSePlayNG();
		// タイトルポップアップ展開
		popupMenuTitleOpen(this->stat);
		// ポップアップ閉じる
		this->super.exist = false;
		return true;
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeMenuSetting *this){
	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnBack);
	calcTouch(this);

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// スライドバー作成
static void createArraySlideBar(struct popupCartridgeMenuSetting *this, struct e3dTransImage *transImage, double y){
	e3dTransImageInit(transImage);
	e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 0);

	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int faceNum = 0;

	engineGraphicBufferPushVert(-60 + 2 * CONST_COS090, y + 2 * CONST_SIN090, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-60 + 2 * CONST_COS150, y + 2 * CONST_SIN150, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-60 + 2 * CONST_COS210, y + 2 * CONST_SIN210, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-60 + 2 * CONST_COS270, y + 2 * CONST_SIN270, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(100 + 8 * CONST_COS270, y + 8 * CONST_SIN270, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(100 + 8 * CONST_COS330, y + 8 * CONST_SIN330, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(100 + 8 * CONST_COS030, y + 8 * CONST_SIN030, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(100 + 8 * CONST_COS090, y + 8 * CONST_SIN090, 0.0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushFace(vertIndex, 0, 1, 3); faceNum++;
	engineGraphicBufferPushFace(vertIndex, 1, 2, 3); faceNum++;
	engineGraphicBufferPushFace(vertIndex, 0, 3, 4); faceNum++;
	engineGraphicBufferPushFace(vertIndex, 4, 7, 0); faceNum++;
	engineGraphicBufferPushFace(vertIndex, 4, 5, 7); faceNum++;
	engineGraphicBufferPushFace(vertIndex, 5, 6, 7); faceNum++;

	transImage->image.faceIndex = faceIndex;
	transImage->image.faceNum = faceNum;
}

// バッファ作成
static void createBuffer(struct popupCartridgeMenuSetting *this){
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
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// スライドバー作成
		createArraySlideBar(this, &this->imgPool[imgPoolIndex++], -30);
		createArraySlideBar(this, &this->imgPool[imgPoolIndex++],  30);
		// アイコン作成
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 0);
		e3dImageCreateArray(&transImage->image, TEXSIZ_SYSTEM_WH, -90 - 48 * 0.5, -30 - 48 * 0.5, 48, 48, TEXPOS_SYSTEM_ICONBGM_XYWH);
		// アイコン作成
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 0);
		e3dImageCreateArray(&transImage->image, TEXSIZ_SYSTEM_WH, -90 - 48 * 0.5,  30 - 48 * 0.5, 48, 48, TEXPOS_SYSTEM_ICONSE_XYWH);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnBack, 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 0);

		// スライドボタン作成
		partsButtonBoxCreateArray(&this->btnSlide[0], 0, -30 - 30 * 0.5 - 1, 20, 30, "", 1.0);
		partsButtonBoxCreateArray(&this->btnSlide[1], 0,  30 - 30 * 0.5 - 1, 20, 30, "", 1.0);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnSlide[0].trans, &this->transWindow, 1);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnSlide[1].trans, &this->transWindow, 1);
		partsButtonPosition(&this->btnBorder[0], global.screen.w * 0.5 - 62, global.screen.h * 0.5 - 30 - 20, 170, 40);
		partsButtonPosition(&this->btnBorder[1], global.screen.w * 0.5 - 62, global.screen.h * 0.5 + 30 - 20, 170, 40);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeMenuSetting *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// スライドバーの移動
	double max = POPUPMENUSETTING_MAX;
	double min = POPUPMENUSETTING_MIN;
	this->btnSlide[0].super.x = min + this->volume[0] * (max - min);
	this->btnSlide[1].super.x = min + this->volume[1] * (max - min);

	// リスト設定
	double param = this->super.openParam * this->super.openParam;
	e3dTransSetTranslate(&this->btnBack.trans, -200 * param, 0.0, 0.0);
	e3dTransSetScale(&this->transWindow, this->super.openSize, this->super.openSize, 1.0);
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeMenuSetting *this){
	if(this->isSetting){
		// 設定保存
		storageCommit();
	}
}

// 破棄
static void dispose(struct popupCartridgeMenuSetting *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
static struct popupCartridge *popupMenuSetting(struct pageMenuStatus *stat){
	struct popupCartridgeMenuSetting *this = (struct popupCartridgeMenuSetting*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeMenuSetting));
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
void popupMenuSettingOpen(struct pageMenuStatus *stat){
	popupManagerPush(&stat->popup, popupMenuSetting(stat), NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

