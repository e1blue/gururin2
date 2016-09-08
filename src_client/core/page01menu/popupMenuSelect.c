#include "../library.h"
#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridgeMenuSelect{
	struct popupCartridge super;
	struct pageMenuStatus *stat;

	struct partsButtonHex btnBack;

	struct e3dTrans transRoot;
	struct e3dTrans transStageRoot;
	struct e3dTrans transStageNew;
	struct e3dTrans transStageOld;
	struct e3dTrans transScrollBar;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	struct e3dImage imgScrollBar;
	struct e3dImage imgScrollTop;
	struct e3dImage imgScrollBottom;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	// ステージストーリーデータ
	struct popupCartridgeMenuSelectSelect{
		struct partsEmblem emblem;
		bool isNew;
		int y;
	} *selectList;

	// ベルト傾き
	double theta;
	double cosTheta;
	double sinTheta;
	// ベルト移動
	double slide;
	double slideNext;
	double slideMin;
	double slideMax;
	// スライドバー
	double barRad;
	double slideView;
	double slideArea;
	double barHeight;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct popupCartridgeMenuSelect *this){
	// ボタン作成
	partsButtonHexInit(&this->btnBack);

	// ステージストーリーデータ作成
	int y0 = 0;
	int y1 = 0;
	int select = -1;
	int lockedSelectIndex = utilLockedSelectIndex();
	bool isNew = false;
	this->selectList = (struct popupCartridgeMenuSelectSelect*)engineUtilMemoryCalloc(this->stat->selectLength, sizeof(struct popupCartridgeMenuSelectSelect));
	for(int i = 0; i < this->stat->selectLength; i++){
		struct pageMenuStatusSelect *select0 = &this->stat->selectList[i];
		struct popupCartridgeMenuSelectSelect *select1 = &this->selectList[i];
		if(select0->isStage){
			partsEmblemInitStage(&select1->emblem, select0->stage.icon, select0->stage.mark, select0->stage.storage.score, true, select0->stage.storage.replayLength > 0);
			select1->y = y0 = (y1 == 0) ? 0 : (y1 = y1 + 40 + 8); y1 = y1 + 40 + 8;
		}else{
			partsEmblemInitStage(&select1->emblem, select0->story.icon, 0, 0, true, false);
			select1->y = y0 = (y1 == 0) ? 0 : (y1 = y1 + 25 + 8); y1 = y1 + 25 + 8;
		}
		// 選択確認
		if(!isNew && select0->index == settingSelectGet()){select = i;}
		// 新ステージフラグ確認
		signed char *openStage = select0->isStage ? &this->stat->progress.openStage[select0->stage.index] : &this->stat->progress.openStory[select0->story.index];
		if((*openStage & 0x01) && !(*openStage & 0x02)){
			*openStage = *openStage | 0x02;
			select1->isNew = isNew = true;
			select = i;
		}
		// 体験版ロック
		select1->emblem.isLock = (0 < lockedSelectIndex && lockedSelectIndex <= select0->index);
	}

	if(isNew){
		// 新ステージフラグ保存
		storageProgressSet(&this->stat->progress);
		storageCommit();
	}

	if(select < 0){select = 0;}

	// ベルト移動
	this->slideNext = this->selectList[select].y;
	this->slide = this->slideNext;
	this->slideMin = 0;
	this->slideMax = y0;
	// スライドバー
	this->barRad = 3;
	this->slideView = global.screen.h * 0.35 * 2;
	this->slideArea = this->slideMax - this->slideMin;
	this->barHeight = global.screen.h * this->slideView / (this->slideArea + this->slideView);
	if(this->barHeight < this->barRad * 3){this->barHeight = this->barRad * 3;}
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct popupCartridgeMenuSelect *this){
	static int prevsx;
	static int prevsy;
	static int isScroll;
	// タッチ状態の確認
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_SCREEN);
	// ベルト内ステージストーリーボタンの確認
	bool clickable = (t != NULL && !t->mv && !isScroll);
	for(int i = 0; i < this->stat->selectLength; i++){
		//struct pageMenuStatusSelect *select0 = &this->stat->selectList[i];
		struct popupCartridgeMenuSelectSelect *select1 = &this->selectList[i];
		if(select1->emblem.isStage){
			partsButtonHexSubCalc(&select1->emblem.btnStart, t, clickable);
			if(select1->emblem.isReplay){
				partsButtonHexSubCalc(&select1->emblem.btnReplay, t, clickable);
			}
		}else{
			partsButtonHexSubCalc(&select1->emblem.btnStart, t, clickable);
		}
	}
	// ベルト移動の確認
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				prevsx = t->screen.x;
				prevsy = t->screen.y;
				isScroll = (t->screen.x > global.screen.w - this->barRad * 2);
				engineCtrlTouchOwn();
			}

			// 画面移動処理
			if(isScroll){
				// TODO バーの長さ考慮
				double min = this->slideMin - this->barHeight * 0.5;
				double max = this->slideMax + this->barHeight * 0.5;
				this->slideNext = min + ((max - min) * t->screen.y / global.screen.h);
			}else if(t->mv){
				double x = (prevsx - t->screen.x) * this->cosTheta;
				double y = (prevsy - t->screen.y) * this->sinTheta;
				this->slideNext += 1.0 * (x + y);
			}
			if(this->slideNext < this->slideMin){this->slideNext = this->slideMin;}
			if(this->slideNext > this->slideMax){this->slideNext = this->slideMax;}
			prevsx = t->screen.x;
			prevsy = t->screen.y;
		}else{
			// タッチ終了時
			isScroll = false;
			engineCtrlTouchFree();
		}
	}
}

// 減衰計算
static void calcDamper(struct popupCartridgeMenuSelect *this){
	// ベルト移動の減衰計算
	double ds = this->slide - this->slideNext;
	if(utilMathAbs(ds) > 0.01){this->slide -= ds * 0.2;}
}

// ボタン計算
static bool calcButton(struct popupCartridgeMenuSelect *this){
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

	// ステージストーリーボタン押下確認
	for(int i = 0; i < this->stat->selectLength; i++){
		struct pageMenuStatusSelect *select0 = &this->stat->selectList[i];
		struct popupCartridgeMenuSelectSelect *select1 = &this->selectList[i];
		if(select1->emblem.isStage){
			// ステージ開始ボタン押下確認
			if(select1->emblem.btnStart.super.trigger){
				select1->emblem.btnStart.super.trigger = false;
				soundSePlayOK();
				// 選択の記憶
				if(settingSelectSet(select0->index)){storageCommit();}
				// 開始ポップアップ展開
				popupMenuStartStageOpen(this->stat, select0->stage.id, false);
				// ポップアップ閉じる
				this->super.exist = false;
				return true;
			}

			if(select1->emblem.isReplay){
				// ステージリプレイボタン押下確認
				if(select1->emblem.btnReplay.super.trigger){
					select1->emblem.btnReplay.super.trigger = false;
					soundSePlayOK();
					// 選択の記憶
					if(settingSelectSet(select0->index)){storageCommit();}
					// 開始ポップアップ展開
					popupMenuStartStageOpen(this->stat, select0->stage.id, true);
					// ポップアップ閉じる
					this->super.exist = false;
					return true;
				}
			}
		}else{
			// ストーリー開始ボタン押下確認
			if(select1->emblem.btnStart.super.trigger){
				select1->emblem.btnStart.super.trigger = false;
				soundSePlayOK();
				// 選択の記憶
				if(settingSelectSet(select0->index)){storageCommit();}
				// 開始ポップアップ展開
				popupMenuStartStoryOpen(this->stat, select0->story.id);
				// ポップアップ閉じる
				this->super.exist = false;
				return true;
			}
		}
	}

	return false;
}

// 計算
static void calc(struct popupCartridgeMenuSelect *this){
	// 減衰処理
	calcDamper(this);

	// 遷移中の処理スキップ
	if(this->super.openStep != 0){return;}

	// タッチ処理
	partsButtonHexCalc(&this->btnBack);
	calcTouch(this);

	// ボタン処理
	if(calcButton(this)){return;}
}

// ----------------------------------------------------------------

// スクロールバー描画
static void drawScrollBar_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct popupCartridgeMenuSelect *this = (struct popupCartridgeMenuSelect*)((char*)that - offsetof(struct popupCartridgeMenuSelect, transScrollBar));

	if(global.screen.h > this->barHeight){
		// バッファ登録
		e3dTransBindBuffer(&this->transScrollBar);
		// 行列登録
		struct engineMathMatrix44 tempmat1;
		struct engineMathMatrix44 tempmat2;
		engineMathMat4Copy(&tempmat1, mat);
		e3dTransMultMatrix(&this->transScrollBar, &tempmat1);
		engineGraphicEngineSetMatrix(&tempmat1);
		// 色登録
		e3dTransBindColor(&this->transScrollBar, r, g, b, a * 0.5);

		double barPositon = (global.screen.h - this->barHeight) * ((this->slide - this->slideMin) / this->slideArea);
		engineMathMat4Copy(&tempmat2, &tempmat1);
		engineMathMat4Translate(&tempmat2, 0, barPositon + this->barRad, 0);
		engineMathMat4Scale(&tempmat2, 1, this->barHeight - this->barRad * 2, 1);
		engineGraphicEngineSetMatrix(&tempmat2);
		e3dImageDraw(&this->imgScrollBar);

		engineMathMat4Copy(&tempmat2, &tempmat1);
		engineMathMat4Translate(&tempmat2, 0, barPositon + this->barRad, 0);
		engineGraphicEngineSetMatrix(&tempmat2);
		e3dImageDraw(&this->imgScrollTop);

		engineMathMat4Copy(&tempmat2, &tempmat1);
		engineMathMat4Translate(&tempmat2, 0, barPositon - this->barRad + this->barHeight, 0);
		engineGraphicEngineSetMatrix(&tempmat2);
		e3dImageDraw(&this->imgScrollBottom);
	}
}

// スクロールバー作成
static void createArrayScrollBar(struct popupCartridgeMenuSelect *this){
	e3dImageWhiteCreateArray(&this->imgScrollBar, global.screen.w - this->barRad * 2, 0, this->barRad * 2, 1.0);

	for(int i = 0; i < 2; i++){
		int vertIndex = engineGraphicBufferVretIndexGet();
		int faceIndex = engineGraphicBufferFaceIndexGet();
		int faceNum = 3;
		double x0 = global.screen.w - this->barRad;
		engineGraphicBufferPushVert(x0, 0.0, 0.0);
		engineGraphicBufferPushTexcWhite();
		for(int j = 0; j < faceNum + 1; j++){
			double t = CONST_PI * (i + (double)j / faceNum);
			engineGraphicBufferPushVert(x0 + this->barRad * utilMathCos(t), this->barRad * utilMathSin(t), 0.0);
			engineGraphicBufferPushTexcWhite();
			if(j == 0){continue;}
			engineGraphicBufferPushFace(vertIndex, 0, 1 + j - 1, 1 + j + 0);
		}
		switch(i){
			case 0: this->imgScrollBottom.faceIndex = faceIndex; this->imgScrollBottom.faceNum = faceNum; break;
			case 1: this->imgScrollTop.faceIndex = faceIndex; this->imgScrollTop.faceNum = faceNum; break;
		}
	}
}

// バッファ作成
static void createBuffer(struct popupCartridgeMenuSelect *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// ベルト傾き設定
		this->theta = utilMathAtan2(global.screen.h * 2, global.screen.w) - CONST_PI * 0.5;
		this->cosTheta = utilMathCos(this->theta + CONST_PI * 0.5);
		this->sinTheta = utilMathSin(this->theta + CONST_PI * 0.5);

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transStageRoot);
		e3dTransInit(&this->transStageNew);
		e3dTransInit(&this->transStageOld);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		e3dTransSetTranslate(&this->transStageRoot, global.screen.w * 0.5, global.screen.h * 0.5, 0);
		e3dTransSetRotate(&this->transStageRoot, 0, 0, this->theta);
		this->transStageRoot.parent = &this->transRoot;
		this->transStageNew.parent = &this->transStageRoot;
		this->transStageOld.parent = &this->transStageRoot;
		// 変形描画構造体登録準備
		//int imgPoolIndex = 0;
		///struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// ボタン作成
		partsButtonHexCreateArray(&this->btnBack, 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
		e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 0);

		// スクロールバー作成
		createArrayScrollBar(this);
		e3dTransInit(&this->transScrollBar);
		this->transScrollBar.draw = drawScrollBar_trans;
		e3dTransListManagerPushProperty(&this->transManager, &this->transScrollBar, &this->transRoot, 2);

		// ステージストーリー作成
		for(int i = 0; i < this->stat->selectLength; i++){
			//struct pageMenuStatusSelect *select0 = &this->stat->selectList[i];
			struct popupCartridgeMenuSelectSelect *select1 = &this->selectList[i];
			struct e3dTrans *parent = select1->isNew ? &this->transStageNew : &this->transStageOld;
			partsEmblemCreateArray(&select1->emblem, 0, select1->y, -this->theta);
			e3dTransListManagerPushProperty(&this->transManager, &select1->emblem.trans, parent, 1);
		}

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct popupCartridgeMenuSelect *this){
	// 描画準備
	createBuffer(this);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);

	// リスト設定
	double slide = -this->slide;
	double param = this->super.openParam * this->super.openParam;
	if(this->super.isOpening){slide -= 100 * param;}
	if(this->super.isClosing){slide += 100 * param;}
	e3dTransSetTranslate(&this->transStageNew, this->super.isOpening ? (200 * param) : 0.0, slide, 0.0);
	e3dTransSetTranslate(&this->transStageOld, 0.0, slide, 0);
	e3dTransSetTranslate(&this->btnBack.trans, -200 * param, 0.0, 0.0);
	e3dTransSetColor(&this->transStageNew, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transStageOld, 1.0, 1.0, 1.0, this->super.openAlpha);
	e3dTransSetColor(&this->transScrollBar, 1.0, 1.0, 1.0, this->super.openAlpha);

	// ステージストーリークリッピング
	double border = ((0.5 * global.screen.h) + (40 * this->sinTheta + 100 * this->cosTheta)) / this->sinTheta;
	for(int i = 0; i < this->stat->selectLength; i++){
		//struct pageMenuStatusSelect *select0 = &this->stat->selectList[i];
		struct popupCartridgeMenuSelectSelect *select1 = &this->selectList[i];
		double slidey = select1->y - this->slide;
		select1->emblem.trans.isVisible = (-border < slidey && slidey < border);
		select1->emblem.btnStart.super.inactive = !select1->emblem.trans.isVisible || select1->emblem.isLock;
		select1->emblem.btnReplay.super.inactive = !select1->emblem.trans.isVisible || select1->emblem.isLock;
	}

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct popupCartridgeMenuSelect *this){
}

// 破棄
static void dispose(struct popupCartridgeMenuSelect *this){
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->selectList);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
static struct popupCartridge *popupMenuSelect(struct pageMenuStatus *stat){
	struct popupCartridgeMenuSelect *this = (struct popupCartridgeMenuSelect*)engineUtilMemoryCalloc(1, sizeof(struct popupCartridgeMenuSelect));
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
void popupMenuSelectOpen(struct pageMenuStatus *stat){
	popupManagerPush(&stat->popup, popupMenuSelect(stat), NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

