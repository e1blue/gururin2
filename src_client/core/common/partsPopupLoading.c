#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// 子ポップアップ 状態表示

// 子ポップアップカートリッジ構造体
struct partsPopupLoadingDisplay{
	struct popupCartridge super;

	struct partsButtonHex btnBack;	

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTrans transWindow;
	struct e3dTrans transRotate;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	bool isCancelable;
	bool isCancel;
	int step;
	int stepRotate;
};

// ----------------------------------------------------------------

// 初期化
static void child1Init(struct partsPopupLoadingDisplay *this){
	// ボタン作成
	partsButtonHexInit(&this->btnBack);
}

// ----------------------------------------------------------------

// ボタン計算
static bool child1CalcButton(struct partsPopupLoadingDisplay *this){
	// 戻るボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		if(this->isCancelable){
			soundSePlayNG();
			// ポップアップ閉じる
			this->isCancel = true;
			this->super.exist = false;
			return true;
		}
	}

	return false;
}

// 計算
static void child1Calc(struct partsPopupLoadingDisplay *this){
	this->stepRotate++;

	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnBack);

	// ボタン処理
	if(child1CalcButton(this)){return;}

	this->super.exist = (this->step++ < 30) || platformPluginUtilIsLoading();
}

// ----------------------------------------------------------------

// バッファ作成
static void child1CreateBuffer(struct partsPopupLoadingDisplay *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		//int ww = 150;
		//int wh = 100;
		//int wx = ww * -0.5;
		//int wy = wh * -0.5;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transBack);
		e3dTransInit(&this->transWindow);
		e3dTransInit(&this->transRotate);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transWindow, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		this->transBack.parent = &this->transRoot;
		this->transWindow.parent = &this->transRoot;
		this->transRotate.parent = &this->transWindow;
		// 変形描画構造体登録準備
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// 暗幕
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transBack, 0);
		e3dImageWhiteCreateArray(&transImage->image, 0, 0, global.screen.w, global.screen.h);
		e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);
		//// ウインドウ
		//e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		//e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 1);
		//e3dImageBoxDefaultCreateArray(&transImage->image, wx, wy, ww, wh);
		//// メッセージ
		//e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		//e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 2);
		//e3dImageTextCreateArray(&transImage->image, wx + 10, wy + 10, "ロードちゅう", 1.0, 1, 1);
		//e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);
		// 回転
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transRotate, 1);
		e3dImageCreateArray(&transImage->image, TEXSIZ_SYSTEM_WH, -30, -30, 60, 60, TEXPOS_SYSTEM_ICONLOADING_XYWH);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnBack, global.screen.w * 0.0 + 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 1);
		// ボタン表示条件設定
		this->btnBack.trans.isVisible = this->isCancelable;
		// ボタン有効設定
		this->btnBack.super.inactive = !this->btnBack.trans.isVisible;

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void child1Draw(struct partsPopupLoadingDisplay *this){
	// 描画準備
	child1CreateBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetTranslate(&this->btnBack.trans, -200 * this->super.backParam * this->super.backParam, 0.0, 0.0);
	e3dTransSetRotate(&this->transRotate, 0.0, 0.0, CONST_PI * 0.02 * this->stepRotate);
	e3dTransSetScale(&this->transWindow, this->super.openSize, this->super.openSize, 1.0);
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, this->super.backAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void child1Pause(struct partsPopupLoadingDisplay *this){
}

// 破棄
static void child1Dispose(struct partsPopupLoadingDisplay *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// 子ポップアップ作成
static struct popupCartridge *partsPopupLoadingDisplay(bool isCancelable){
	struct partsPopupLoadingDisplay *this = (struct partsPopupLoadingDisplay*)engineUtilMemoryCalloc(1, sizeof(struct partsPopupLoadingDisplay));
	this->super.exist = true;
	this->isCancelable = isCancelable;

	struct popupCartridge *cartridge = (struct popupCartridge*)this;
	cartridge->init = (void(*)(struct popupCartridge*))child1Init;
	cartridge->calc = (void(*)(struct popupCartridge*))child1Calc;
	cartridge->draw = (void(*)(struct popupCartridge*))child1Draw;
	cartridge->pause = (void(*)(struct popupCartridge*))child1Pause;
	cartridge->dispose = (void(*)(struct popupCartridge*))child1Dispose;
	return cartridge;
}

// 子ポップアップ結果
static bool partsPopupLoadingDisplayResultIsCancel(struct popupCartridge *that){
	struct partsPopupLoadingDisplay *this = (struct partsPopupLoadingDisplay*)that;
	return this->isCancel;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// 子ポップアップ 再読込確認

// 子ポップアップカートリッジ構造体
struct partsPopupLoadingRetry{
	struct popupCartridge super;

	//struct partsButtonBox btnRetry;
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

	bool isCancelable;
	bool isCancel;
};

// ----------------------------------------------------------------

// 初期化
static void child2Init(struct partsPopupLoadingRetry *this){
	// ボタン作成
	//partsButtonBoxInit(&this->btnRetry);
	partsButtonHexInit(&this->btnRetry);
	partsButtonHexInit(&this->btnBack);
}

// ----------------------------------------------------------------

// ボタン計算
static bool child2CalcButton(struct partsPopupLoadingRetry *this){
	// ボタン押下確認
	if(this->btnRetry.super.trigger){
		this->btnRetry.super.trigger = false;
		soundSePlayOK();
		// ポップアップ閉じる
		this->super.exist = false;
		return true;
	}

	// 戻るボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		if(this->isCancelable){
			soundSePlayNG();
			// ポップアップ閉じる
			this->isCancel = true;
			this->super.exist = false;
			return true;
		}
	}

	return false;
}

// 計算
static void child2Calc(struct partsPopupLoadingRetry *this){
	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	//partsButtonBoxCalc(&this->btnRetry);
	partsButtonHexCalc(&this->btnRetry);
	partsButtonHexCalc(&this->btnBack);

	// ボタン処理
	if(child2CalcButton(this)){return;}
}

// ----------------------------------------------------------------

// バッファ作成
static void child2CreateBuffer(struct partsPopupLoadingRetry *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		//int ww = 150;
		//int wh = 100;
		//int wx = ww * -0.5;
		//int wy = wh * -0.5;

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
		//// ウインドウ
		//e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		//e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 1);
		//e3dImageBoxDefaultCreateArray(&transImage->image, wx, wy, ww, wh);
		//// メッセージ
		//e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		//e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 2);
		//e3dImageTextCreateArray(&transImage->image, wx + 10, wy + 10, "ロードしっぱいです!\nリトライしてください", 1.0, 1, 1);
		//e3dTransSetColor(&transImage->super, 0.0, 0.0, 0.0, 1.0);
		// エラーアイコン
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transWindow, 1);
		e3dImageCreateArray(&transImage->image, TEXSIZ_SYSTEM_WH, -30 - 40, -30 - 20, 60, 60, TEXPOS_SYSTEM_ICONLOADFAIL_XYWH);

		// ボタン作成
		//int bw = 60;
		//int bh = 30;
		//partsButtonBoxCreateArray(&this->btnRetry, wx + (ww - bw) * 0.5, wy + wh - 5 - bh, bw, bh, "リトライ", 1.0);
		partsButtonHexCreateArray(&this->btnRetry, 40, 20, 50, TEXPOS_SYSTEM_ICONRELOAD_XYWH);
		partsButtonHexCreateArray(&this->btnBack, global.screen.w * 0.0 + 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnRetry.trans, &this->transWindow, 2);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 1);
		// ボタン表示条件設定
		this->btnBack.trans.isVisible = this->isCancelable;
		// ボタン有効設定
		this->btnBack.super.inactive = !this->btnBack.trans.isVisible;

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void child2Draw(struct partsPopupLoadingRetry *this){
	// 描画準備
	child2CreateBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	e3dTransSetTranslate(&this->btnBack.trans, -200 * this->super.backParam * this->super.backParam, 0.0, 0.0);
	e3dTransSetScale(&this->transWindow, this->super.openSize, this->super.openSize, 1.0);
	e3dTransSetColor(&this->transWindow, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transBack, 1.0, 1.0, 1.0, this->super.backAlpha);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void child2Pause(struct partsPopupLoadingRetry *this){
}

// 破棄
static void child2Dispose(struct partsPopupLoadingRetry *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// 子ポップアップ作成
static struct popupCartridge *partsPopupLoadingRetry(bool isCancelable){
	struct partsPopupLoadingRetry *this = (struct partsPopupLoadingRetry*)engineUtilMemoryCalloc(1, sizeof(struct partsPopupLoadingRetry));
	this->super.exist = true;
	this->isCancelable = isCancelable;

	struct popupCartridge *cartridge = (struct popupCartridge*)this;
	cartridge->init = (void(*)(struct popupCartridge*))child2Init;
	cartridge->calc = (void(*)(struct popupCartridge*))child2Calc;
	cartridge->draw = (void(*)(struct popupCartridge*))child2Draw;
	cartridge->pause = (void(*)(struct popupCartridge*))child2Pause;
	cartridge->dispose = (void(*)(struct popupCartridge*))child2Dispose;
	return cartridge;
}

// 子ポップアップ結果
static bool partsPopupLoadingRetryResultIsCancel(struct popupCartridge *that){
	struct partsPopupLoadingRetry *this = (struct partsPopupLoadingRetry*)that;
	return this->isCancel;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct partsPopupLoading{
	struct popupCartridge super;

	struct popupManager popup;

	void *stat;
	void(*load)(void*);
	int(*loaded)(void*);
	void(*cancel)(void*);

	int step;
	bool isRetry;
	bool isFinish;
	bool isCancel;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct partsPopupLoading *this){
}

// ----------------------------------------------------------------

// 読込確認
static void calcIsLoading(struct partsPopupLoading *this){
	if(this->isRetry){return;}
	if(this->isFinish){return;}
	if(this->isCancel){return;}
	if(!platformPluginUtilIsLoading()){
		switch(this->loaded(this->stat)){
			case PARTSPOPUPLOADING_LOADED_PENDING:
				// ロード継続
				break;
			case PARTSPOPUPLOADING_LOADED_RETRY:
				// ロード失敗
				this->isRetry = true;
				break;
			case PARTSPOPUPLOADING_LOADED_OK:
			default:
				// ロード成功
				this->isFinish = true;
				break;
		}
	}
}

// プロトタイプ宣言
static void onClosePopupRetry(struct partsPopupLoading *this, struct popupCartridge *cartridge);
static void onClosePopupDisplay(struct partsPopupLoading *this, struct popupCartridge *cartridge);

// 再読込確認ポップアップ完了時
static void onClosePopupRetry(struct partsPopupLoading *this, struct popupCartridge *cartridge){
	if(partsPopupLoadingRetryResultIsCancel(cartridge)){
		// キャンセル
		this->isCancel = true;
	}else{
		this->isRetry = false;
		this->load(this->stat);
		// 状態表示ポップアップ展開
		popupManagerPush(&this->popup, partsPopupLoadingDisplay(this->cancel != NULL), (void(*)(void*, struct popupCartridge*))onClosePopupDisplay);
	}
}

// 状態表示ポップアップ完了時
static void onClosePopupDisplay(struct partsPopupLoading *this, struct popupCartridge *cartridge){
	if(partsPopupLoadingDisplayResultIsCancel(cartridge)){
		// キャンセル
		this->isCancel = true;
	}else{
		calcIsLoading(this);
		if(this->isRetry){
			// 再読込確認ポップアップ展開
			popupManagerPush(&this->popup, partsPopupLoadingRetry(this->cancel != NULL), (void(*)(void*, struct popupCartridge*))onClosePopupRetry);
		}
	}
}

// 計算
static void calc(struct partsPopupLoading *this){
	// 閉遷移中の処理スキップ
	if(this->super.openStep > 0){return;}

	// ポップアップ処理
	if(popupManagerCalc(&this->popup, this)){return;}

	calcIsLoading(this);

	this->super.exist = !this->isFinish && !this->isCancel;

	// キャンセル処理
	if(this->isCancel && this->cancel != NULL){this->cancel(this->stat);}

	if(this->super.exist && this->step++ > 15){
		// 状態表示ポップアップ展開
		popupManagerPush(&this->popup, partsPopupLoadingDisplay(this->cancel != NULL), (void(*)(void*, struct popupCartridge*))onClosePopupDisplay);
	}

	// 戻るボタン押下確認
	if(global.key.bk.triggerInactive){
		global.key.bk.triggerInactive = false;
		// 何も起きない
	}
}

// ----------------------------------------------------------------

// 描画
static void draw(struct partsPopupLoading *this){
	// ポップアップ描画
	popupManagerDraw(&this->popup);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct partsPopupLoading *this){
	popupManagerPause(&this->popup);
}

// 破棄
static void dispose(struct partsPopupLoading *this){
	popupManagerDispose(&this->popup);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *partsPopupLoading(void *stat, void(*load)(void*), int(*loaded)(void*), void(*cancel)(void*)){
	struct partsPopupLoading *this = (struct partsPopupLoading*)engineUtilMemoryCalloc(1, sizeof(struct partsPopupLoading));
	this->super.exist = true;
	this->stat = stat;
	this->load = load;
	this->loaded = loaded;
	this->cancel = cancel;
	// 読み込みを開始
	load(stat);

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

