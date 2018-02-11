#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#define POPUPMENUPLAYER_RADIUS 50
#define POPUPMENUPLAYER_WIDTH (POPUPMENUPLAYER_RADIUS * CONST_SQRT3 + 10)

// ポップアップカートリッジ構造体
struct popupCartridgeMenuPlayer{
	struct popupCartridge super;
	struct pageMenuStatus *stat;

	struct partsButtonHex btnBack;

	struct e3dTrans transRoot;
	struct e3dTrans transPuppetRoot;
	struct e3dTrans transPuppetNew;
	struct e3dTrans transPuppetOld;
	struct e3dTrans transSelectRing;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	struct e3dImage imgSelectRing;
	struct e3dImage imgStarOff;
	struct e3dImage imgStarOn;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	// パペットボタンデータ
	struct popupCartridgeMenuPlayerPuppet{
		struct partsButtonHex btnHex;
		bool isNew;
		double y;
	} *puppetList;
	int puppetLength;

	int step;
	int select;
	enum hknwCharaKind kind;
	// ベルト傾き
	double theta;
	double cosTheta;
	double sinTheta;
	// ベルト移動
	double slide;
	double slideNext;
	double slideMin;
	double slideMax;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeMenuPlayer *this){
	// ボタン作成
	partsButtonHexInit(&this->btnBack);

	this->select = -1;
	this->kind = settingPuppetGet();

	// パペットデータ作成
	int select = -1;
	bool isNew = false;
	this->puppetList = (struct popupCartridgeMenuPlayerPuppet*)engineUtilMemoryCalloc(this->stat->puppetLength, sizeof(struct popupCartridgeMenuPlayerPuppet));
	for(int i = 0; i < this->stat->puppetLength; i++){
		struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		partsButtonHexInit(&puppet1->btnHex);
		// 選択確認
		if(this->kind == puppet0->kind){this->select = i;}
		// 新パペットフラグ確認
		signed char *openPuppet = &this->stat->progress.openPuppet[i];
		if((*openPuppet & 0x01) && !(*openPuppet & 0x02)){
			*openPuppet = *openPuppet | 0x02;
			puppet1->isNew = isNew = true;
			select = i;
		}
	}

	if(isNew){
		// 新ステージフラグ保存
		storageProgressSet(&this->stat->progress);
		storageCommit();
	}

	if(this->select < 0){this->select = 0;}
	if(select < 0){select = this->select;}

	// ベルト移動
	this->slideNext = select * POPUPMENUPLAYER_WIDTH;
	this->slide = this->slideNext;
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct popupCartridgeMenuPlayer *this){
	static int prevsx;
	static int prevsy;
	// タッチ状態の確認
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_SCREEN);
	// ベルト内パペットボタンの確認
	bool clickable = (t != NULL && !t->mv);
	for(int i = 0; i < this->stat->puppetLength; i++){
		//struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		partsButtonHexSubCalc(&puppet1->btnHex, t, clickable);
	}
	// ベルト移動の確認
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				prevsx = t->screen.x;
				prevsy = t->screen.y;
				engineCtrlTouchOwn();
			}

			// 画面移動処理
			if(t->mv){
				double x = (prevsx - t->screen.x) * this->cosTheta;
				double y = (prevsy - t->screen.y) * this->sinTheta;
				this->slideNext += 1.0 * (x + y);
			}
			prevsx = t->screen.x;
			prevsy = t->screen.y;
		}else{
			// タッチ終了時
			engineCtrlTouchFree();
		}
	}
}

// 減衰計算
static void calcDamper(struct popupCartridgeMenuPlayer *this){
	// ベルト移動の減衰計算
	double ds = this->slide - this->slideNext;
	if(utilMathAbs(ds) > 0.01){this->slide -= ds * 0.2;}
}

// ボタン計算
static bool calcButton(struct popupCartridgeMenuPlayer *this){
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

	// パペットボタン押下確認
	for(int i = 0; i < this->stat->puppetLength; i++){
		struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		if(puppet1->btnHex.super.trigger){
			puppet1->btnHex.super.trigger = false;
			soundSePlayOK();
			// 選択変更
			this->select = i;
			this->kind = puppet0->isActive ? puppet0->kind : settingPuppetGet();
			this->slideNext = this->slide + puppet1->btnHex.super.x;
			return true;
		}
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeMenuPlayer *this){
	// 減衰処理
	calcDamper(this);

	// パペットボタン計算
	for(int i = 0; i < this->stat->puppetLength; i++){
		//struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		puppet1->btnHex.select = (this->select == i);
		puppet1->y -= 0.3 * (puppet1->y - (puppet1->btnHex.select ? -20 : 0));
	}

	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnBack);
	calcTouch(this);

	// ボタン処理
	if(calcButton(this)){return;}

	this->step++;
}

// ----------------------------------------------------------------

// 選択リング描画
static void drawSelectRing_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct popupCartridgeMenuPlayer *this = (struct popupCartridgeMenuPlayer*)((char*)that - offsetof(struct popupCartridgeMenuPlayer, transSelectRing));

	// バッファ登録
	e3dTransBindBuffer(&this->transSelectRing);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(&this->transSelectRing, &tempmat1);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(&this->transSelectRing, r, g, b, a);

	e3dImageDraw(&this->imgSelectRing);
}

// 選択リング作成
static void createArraySelectRing(struct popupCartridgeMenuPlayer *this){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int faceNum = 0;

	engineGraphicBufferPushVert(  0, -55, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert( 10, -75, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-10, -75, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushFace(vertIndex, 0, 1, 2); faceNum++;

	this->imgSelectRing.faceIndex = faceIndex;
	this->imgSelectRing.faceNum = faceNum;

}

// バッファ作成
static void createBuffer(struct popupCartridgeMenuPlayer *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// ベルト傾き設定
		this->theta = CONST_PI * -1 / 12;
		this->cosTheta = utilMathCos(this->theta);
		this->sinTheta = utilMathSin(this->theta);

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transPuppetRoot);
		e3dTransInit(&this->transPuppetNew);
		e3dTransInit(&this->transPuppetOld);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transPuppetRoot, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		e3dTransSetRotate(&this->transPuppetRoot, 0, 0, this->theta);
		this->transPuppetRoot.parent = &this->transRoot;
		this->transPuppetNew.parent = &this->transPuppetRoot;
		this->transPuppetOld.parent = &this->transPuppetRoot;
		// 変形描画構造体登録準備
		//int imgPoolIndex = 0;
		//struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnBack, 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 0);

		// パペットボタン作成
		for(int i = 0; i < this->stat->puppetLength; i++){
			//struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
			struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
			struct e3dTrans *parent = puppet1->isNew ? &this->transPuppetNew : &this->transPuppetOld;
			partsButtonHexCreateArray(&puppet1->btnHex, 0, 0, POPUPMENUPLAYER_RADIUS, 0, 0, 0, 0);
			e3dTransListManagerPushProperty(&this->transManager, &puppet1->btnHex.trans, parent, 0);
			puppet1->btnHex.rotInner = -this->theta;
			puppet1->btnHex.rotOuter = CONST_PI * 0.5;
		}

		// 選択リング作成
		createArraySelectRing(this);
		e3dTransInit(&this->transSelectRing);
		this->transSelectRing.draw = drawSelectRing_trans;
		e3dTransListManagerPushProperty(&this->transManager, &this->transSelectRing, &this->transPuppetOld, 1);

		// 解放条件スター作成
		e3dImageCreateArray(&this->imgStarOff, TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_PARAMELEMSTAR2_XYWH);
		e3dImageCreateArray(&this->imgStarOn, TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_PARAMELEMSTAR1_XYWH);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeMenuPlayer *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
	struct engineMathMatrix44 tempmat1;

	// リスト設定
	double param = this->super.openParam * this->super.openParam;
	e3dTransSetTranslate(&this->btnBack.trans, -200 * param, 0.0, 0.0);
	e3dTransSetTranslate(&this->transPuppetNew, 0.0, this->super.isOpening ? (-200 * param) : 0.0, 0.0);
	e3dTransSetColor(&this->transPuppetNew, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transPuppetOld, 1.0, 1.0, 1.0, this->super.openAlpha);

	// パペットボタン移動
	double slide = -this->slide;
	if(this->super.isOpening){slide += 100 * param;}
	if(this->super.isClosing){slide -= 100 * param;}
	double max = this->stat->puppetLength * POPUPMENUPLAYER_WIDTH;
	double border = ((0.5 * global.screen.w) + 50) / this->cosTheta;
	for(int i = 0; i < this->stat->puppetLength; i++){
		struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		double x = slide + i * POPUPMENUPLAYER_WIDTH;
		while(x >  max * 0.5){x -= max;}
		while(x < -max * 0.5){x += max;}
		puppet1->btnHex.super.x = x;
		puppet1->btnHex.super.y = puppet1->y;
		puppet1->btnHex.trans.isVisible = (-border < x && x < border);
		puppet1->btnHex.super.inactive = !puppet1->btnHex.trans.isVisible;
		double color = puppet0->isActive ? 1.0 : 0.5;
		e3dTransSetColor(&puppet1->btnHex.trans, color, color, color, 1.0);
		// 選択リングを選択中のパペットボタンのところへ移動
		if(this->kind == puppet0->kind){e3dTransSetTranslate(&this->transSelectRing, puppet1->btnHex.super.x, puppet1->btnHex.super.y, 0);}
	}

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// 解放条件描画
	engineGraphicEngineBindVertVBO(this->e3dIdVert);
	engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
	engineGraphicEngineBindFaceIBO(this->e3dIdFace);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[this->select];
	//struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[this->select];
	if(!puppet0->isActive){
		for(int i = 0; i < puppet0->openStar; i++){
			int row = 15;
			int col = 5;
			double x = global.screen.w - 10 - 15 * ((row - 1) - (i % row));
			double y = global.screen.h - 10 - 15 * ((col - 1) - utilMathFloor((double)i / row));
			engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
			engineMathMat4Translate(&tempmat1, x, y, 0.0);
			engineGraphicEngineSetMatrix(&tempmat1);
			e3dImageDraw(i < (int)this->stat->progress.totalStar ? &this->imgStarOn : &this->imgStarOff);
		}
	}

	// パペット描画
	engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
	engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
	engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
	for(int i = 0; i < this->stat->puppetLength; i++){
		struct pageMenuStatusPuppet *puppet0 = &this->stat->puppetList[i];
		struct popupCartridgeMenuPlayerPuppet *puppet1 = &this->puppetList[i];
		struct e3dTrans *trans = puppet1->isNew ? &this->transPuppetNew : &this->transPuppetOld;
		struct hknwChara *draw = hknwCharaManagerGet(&this->stat->charaManager, puppet0->kind);
		enum hknwCharaPosesId pose = HKNWCHARAPOSESID_STAND;
		int step = (this->step / 8) % hknwCharaGetPoseStepNum(draw, pose);
		double r = CONST_DEG2RAD * (90 + (puppet0->isActive ? this->step : 0));
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, 0.0, 15.0, 0.0);
		e3dTransMultMatrix(trans, &tempmat1);
		engineMathMat4Translate(&tempmat1, puppet1->btnHex.super.x, puppet1->btnHex.super.y, 0.0);
		engineMathMat4RotateZ(&tempmat1, -this->theta);
		if(puppet0->isActive){engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);}else{engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 1.0);}
		hknwCharaDrawChara2D(draw, &tempmat1, r, 1.5, pose, step);
	}
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeMenuPlayer *this){
	// 設定保存
	if(settingPuppetSet(this->kind)){storageCommit();}
}

// 破棄
static void dispose(struct popupCartridgeMenuPlayer *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->puppetList);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
static struct popupCartridge *popupMenuPlayer(struct pageMenuStatus *stat){
	struct popupCartridgeMenuPlayer *this = (struct popupCartridgeMenuPlayer*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeMenuPlayer));
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
void popupMenuPlayerOpen(struct pageMenuStatus *stat){
	popupManagerPush(&stat->popup, popupMenuPlayer(stat), NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

