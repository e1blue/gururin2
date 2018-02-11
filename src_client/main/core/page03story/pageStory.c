#include "../includeAll.h"
#include "pageStory.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジ構造体
struct pageCartridgeStory{
	struct pageCartridge super;
	struct pageStoryStatus *stat;

	struct popupManager popup;
	struct partsButtonHex btnBack;
	struct partsButtonHex btnNext;

	struct e3dTrans transRoot;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;
	struct e3dTransListManager transManagerEffect;

	struct e3dImage imgMask;
	struct e3dImage imgViewMask;
	struct e3dImage imgWallpaper;
	struct e3dImage imgItem[3][2];
	struct e3dImage imgBalloon[3];
	struct e3dImage imgEmotion[5];
	struct e3dImage imgShadow;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	bool isStart;
	int reqIdStory;
	int reqIdStage;
	int step;
	double rotvNext;
};

// ----------------------------------------------------------------

// 読み込み開始
static void load(struct pageCartridgeStory *this){
	char path[32];
	engineDataRequestFreeAll();

	snprintf(path, sizeof(path), "dat/story%04d.json", this->stat->id);
	this->reqIdStory = engineDataRequestLocal(path);

	if(this->stat->nextStageId > 0){
		snprintf(path, sizeof(path), "dat/stage%04d.json", this->stat->nextStageId);
		this->reqIdStage = engineDataRequestLocal(path);
	}
}

// 読み込み完了
static int loaded(struct pageCartridgeStory *this){
	// 読み込み確認
	size_t sizeStory = 0;
	size_t sizeStage = 0;
	byte *loadedStory = NULL;
	byte *loadedStage = NULL;
	bool isErrorStory = true;
	bool isErrorStage = true;
	isErrorStory = !engineDataRequestResponse(this->reqIdStory, (void**)&loadedStory, &sizeStory);
	isErrorStory = isErrorStory || (loadedStory == NULL);
	if(this->stat->nextStageId > 0){
		isErrorStage = !engineDataRequestResponse(this->reqIdStage, (void**)&loadedStage, &sizeStage);
		isErrorStage = isErrorStage || (loadedStage == NULL);
	}else{
		isErrorStage = false;
	}
	if(isErrorStory || isErrorStage){return PARTSPOPUPLOADING_LOADED_RETRY;}

	// 読み込み成功 ストーリーエンジン初期化
	objectStoryEngineInit(this->stat, loadedStory, sizeStory);
	engineDataRequestFree(this->reqIdStory);
	this->isStart = true;

	int bgmId = 0;
	if(this->stat->nextStageId > 0){
		// 次のステージが存在するならばBGM確認
		struct engineDataJsonValue jsonStage;
		engineDataJsonParse(&jsonStage, loadedStage, sizeStage);
		bgmId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonStage, "bgmId"), 1);
		//engineDataJsonTrace(&jsonStage);
		engineDataJsonFree(&jsonStage);
		engineDataRequestFree(this->reqIdStage);
	}

	// BGM変更
	if(bgmId > 0){soundBgmPlayRoll(bgmId);}
	else{soundBgmPlayMenu();}

	return PARTSPOPUPLOADING_LOADED_OK;
}

// ----------------------------------------------------------------

// 初期化
static void init(struct pageCartridgeStory *this){
	// ボタン作成
	partsButtonHexInit(&this->btnBack);
	partsButtonHexInit(&this->btnNext);
	// 画面回転初期化
	this->rotvNext = CONST_DEG2RAD * 0;
	this->stat->rotv = this->rotvNext + CONST_DEG2RAD * 30;
	this->stat->roth = CONST_DEG2RAD * 30;
	// エフェクト初期化
	objectStoryEffectManagerInit(&this->stat->effectManager, this->stat, &this->transManagerEffect);
	// 読み込み開始
	popupManagerPush(&this->popup, partsPopupLoading(this, (void(*)(void*))load, (int(*)(void*))loaded, NULL), NULL);
}

// ----------------------------------------------------------------

// タッチ処理の計算
static void calcTouch(struct pageCartridgeStory *this){
	static int prevsx;
	static int prevsy;
	// タッチ状態の確認
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_SCREEN);
	if(t != NULL){
		if(t->dn){
			if(!t->active){
				// タッチ開始時
				prevsx = t->screen.x;
				prevsy = t->screen.y;
				engineCtrlTouchOwn();
			}

			if(t->mv){
				// フィールド回転処理
				double cx = global.screen.w * 0.5;
				double cy = global.screen.h * 0.5;
				double x0 = prevsx - cx;
				double y0 = prevsy - cy;
				double r0 = utilMathSqrt(x0 * x0 + y0 * y0);
				double x1 = t->screen.x - cx;
				double y1 = t->screen.y - cy;
				double r1 = utilMathSqrt(x1 * x1 + y1 * y1);
				if(r0 > 20 && r1 > 20){
					double cos = (x0 * x1 + y0 * y1) / (r0 * r1);
					if(cos > 1){cos = 1;}else if(cos < -1){cos = -1;}
					this->rotvNext -= 0.8 * utilMathAcos(cos) * ((x0 * y1 - y0 * x1 > 0) ? 1 : -1);
				}
			}
			prevsx = t->screen.x;
			prevsy = t->screen.y;
		}else{
			if(!t->mv && this->step > 60){
				// タッチ処理 スキップ
				this->stat->isSkip = true;
			}
			// タッチ終了時
			engineCtrlTouchFree();
		}
	}
}

// 減衰計算
static void calcDamper(struct pageCartridgeStory *this){
	// 画面回転角度の減衰計算
	this->stat->rotv = utilCalcRotateDamper(this->stat->rotv, this->rotvNext, 0.2);
}

// ボタン計算
static bool calcButton(struct pageCartridgeStory *this){
	// 停止ボタン押下確認
	if(this->btnBack.super.trigger || global.key.bk.triggerInactive){
		this->btnBack.super.trigger = false;
		global.key.bk.triggerInactive = false;
		soundSePlayNG();
		// 終了フラグ
		this->stat->nextStageId = 0;
		this->stat->isExit = true;
		return true;
	}

	// 次へボタン押下確認
	if(this->btnNext.super.trigger){
		this->btnNext.super.trigger = false;
		soundSePlayOK();
		// 終了フラグ
		this->stat->isExit = true;
		return true;
	}

	return false;
}

// 計算
static void calc(struct pageCartridgeStory *this){
	// メモリートレースボタン計算
	partsButtonMemoryCalc();

	// ポップアップ処理
	if(popupManagerCalc(&this->popup, this)){return;}

	calcDamper(this);

	// タッチ処理
	partsButtonHexCalc(&this->btnBack);
	partsButtonHexCalc(&this->btnNext);
	calcTouch(this);
	
	// ストーリーエンジン計算
	objectStoryEngineCalc(this->stat);

	// エフェクト処理
	objectStoryEffectManagerCalc(&this->stat->effectManager);

	// ボタン処理
	if(calcButton(this)){return;}

	// 終了フラグ確認
	if(this->stat->isExit){
		if(this->stat->nextStageId > 0){
			// ページ遷移 次のステージへ
			pageRollInit(this->stat->nextStageId, false);
			this->super.exist = false;
		}else{
			// ページ遷移 メニューに戻る
			pageMenuInitSelect();
			this->super.exist = false;
		}
		return;
	}

	this->step++;
}

// ----------------------------------------------------------------

// フキダシ作成
static void createArrayBalloon(struct pageCartridgeStory *this){
	// 矢印
	this->imgBalloon[0].faceIndex = engineGraphicBufferFaceIndexGet();
	this->imgBalloon[0].faceNum = 1;
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 0, 1, 2);
	engineGraphicBufferPushVert( 0,   0, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-5, -10, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert( 5, -10, 0); engineGraphicBufferPushTexcWhite();
	// 四隅
	this->imgBalloon[1].faceIndex = engineGraphicBufferFaceIndexGet();
	this->imgBalloon[1].faceNum = 3;
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 0, 1, 2);
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 0, 2, 3);
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 0, 3, 4);
	engineGraphicBufferPushVert(0, 0, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-10 * CONST_COS000, -10 * CONST_SIN000, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-10 * CONST_COS030, -10 * CONST_SIN030, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-10 * CONST_COS060, -10 * CONST_SIN060, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(-10 * CONST_COS090, -10 * CONST_SIN090, 0); engineGraphicBufferPushTexcWhite();
	// 中央
	this->imgBalloon[2].faceIndex = engineGraphicBufferFaceIndexGet();
	this->imgBalloon[2].faceNum = 2;
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 0, 1, 2);
	engineGraphicBufferPushFace(engineGraphicBufferVretIndexGet(), 2, 3, 0);
	engineGraphicBufferPushVert(0, 0, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(0, 1, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(1, 1, 0); engineGraphicBufferPushTexcWhite();
	engineGraphicBufferPushVert(1, 0, 0); engineGraphicBufferPushTexcWhite();
}

// 影作成
static void createArrayShadow(struct pageCartridgeStory *this){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int faceNum = 32;
	engineGraphicBufferPushVert(0.0, 0.0, 0.0);
	engineGraphicBufferPushTexcWhite();
	for(int i = 0; i < faceNum; i++){
		double t = CONST_PI * 2 * i / faceNum;
		engineGraphicBufferPushVert(0.5 * utilMathCos(t), 0.0, 0.5 * utilMathSin(t));
		engineGraphicBufferPushTexcWhite();
		engineGraphicBufferPushFace(vertIndex, 0, 1 + i, 1 + (i + 1) % faceNum);
	}
	this->imgShadow.faceIndex = faceIndex;
	this->imgShadow.faceNum = faceNum;
}

// バッファ作成
static void createBuffer(struct pageCartridgeStory *this){
	int status = 1 | ((this->isStart ? 0x01 : 0x00) << 1);
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		// 変形描画構造体登録準備
		//int imgPoolIndex = 0;
		//struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		if(this->isStart){
			// ボタン作成
			partsButtonHexCreateArray(&this->btnBack, global.screen.w * 0.0 + 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONBACK_XYWH);
			partsButtonHexCreateArray(&this->btnNext, global.screen.w * 1.0 - 40, global.screen.h - 36, 35, TEXPOS_SYSTEM_ICONPLAY_XYWH);
			e3dTransListManagerPushProperty(&this->transManager, &this->btnBack.trans, &this->transRoot, 0);
			e3dTransListManagerPushProperty(&this->transManager, &this->btnNext.trans, &this->transRoot, 0);
			// ボタン表示条件設定
			this->btnNext.trans.isVisible = (this->stat->nextStageId > 0);
			// ボタン有効設定
			this->btnNext.super.inactive = !this->btnNext.trans.isVisible;

			// 表示マスク作成
			e3dImageWhiteCreateArray(&this->imgMask, 0, 0, global.screen.w, global.screen.h);
			e3dImageWhiteCreateArray(&this->imgViewMask, 0, -0.5, global.screen.w, 1);

			// 壁紙作成
			partsWallpaperCreateArray(&this->imgWallpaper, this->stat->wall, NULL, NULL);

			// アイテム作成
			e3dImageCreateArray(&this->imgItem[0][0], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_ITEMCOIN1_XYWH);
			e3dImageCreateArray(&this->imgItem[0][1], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_ITEMCOIN2_XYWH);
			e3dImageCreateArray(&this->imgItem[1][0], TEXSIZ_SYSTEM_WH, -18, -18, 36, 36, TEXPOS_SYSTEM_ITEMSTAR1_XYWH);
			e3dImageCreateArray(&this->imgItem[1][1], TEXSIZ_SYSTEM_WH, -18, -18, 36, 36, TEXPOS_SYSTEM_ITEMSTAR2_XYWH);;
			e3dImageCreateArray(&this->imgItem[2][0], TEXSIZ_SYSTEM_WH, -36, -36, 72, 72, TEXPOS_SYSTEM_ITEMSTAR1_XYWH);
			e3dImageCreateArray(&this->imgItem[2][1], TEXSIZ_SYSTEM_WH, -36, -36, 72, 72, TEXPOS_SYSTEM_ITEMSTAR2_XYWH);

			// フキダシ作成
			createArrayBalloon(this);

			// 感情アイコン作成
			e3dImageCreateArray(&this->imgEmotion[0], TEXSIZ_SYSTEM_WH, -20, -20, 40, 40, TEXPOS_SYSTEM_EMOTIONHAPPY_XYWH);
			e3dImageCreateArray(&this->imgEmotion[1], TEXSIZ_SYSTEM_WH, -20, -20, 40, 40, TEXPOS_SYSTEM_EMOTIONEXCLAMATION_XYWH);
			e3dImageCreateArray(&this->imgEmotion[2], TEXSIZ_SYSTEM_WH, -20, -20, 40, 40, TEXPOS_SYSTEM_EMOTIONQUESTION_XYWH);
			e3dImageCreateArray(&this->imgEmotion[3], TEXSIZ_SYSTEM_WH, -20, -20, 40, 40, TEXPOS_SYSTEM_EMOTIONSAD_XYWH);
			e3dImageCreateArray(&this->imgEmotion[4], TEXSIZ_SYSTEM_WH, -20, -20, 40, 40, TEXPOS_SYSTEM_EMOTIONANGRY_XYWH);

			// 影作成
			createArrayShadow(this);

			// エフェクト作成
			objectStoryEffectManagerCreateArray(&this->stat->effectManager);
		}

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// キャラクター描画
static void drawChara(struct pageCartridgeStory *this, int index, struct engineMathMatrix44 *mat){
	struct engineMathMatrix44 tempmat1;
	struct pageStoryStatusChara *chara = &this->stat->charaList[index];
	struct hknwChara *draw = hknwCharaManagerGet(&this->stat->charaManager, chara->kind);
	if(!chara->isExist){return;}
	// キャラクターポーズステップ
	int step = 0;
	switch(chara->pose){
		case HKNWCHARAPOSESID_WALK: step = (chara->step / 12) % hknwCharaGetPoseStepNum(draw, chara->pose); break;
		case HKNWCHARAPOSESID_HAPPY: step = (chara->step / 8) % hknwCharaGetPoseStepNum(draw, chara->pose); break;
		default: break;
	}
	// キャラクター描画
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, chara->x, chara->z, chara->y);
	engineMathMat4RotateZ(&tempmat1, chara->rotg);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	hknwCharaDrawChara3D(draw, &tempmat1, chara->rotv, chara->roth, chara->r, chara->scale, chara->pose, step);
}

// アイテム描画
static void drawItem(struct pageCartridgeStory *this, int index, struct engineMathMatrix44 *mat){
	struct engineMathMatrix44 tempmat1;
	struct pageStoryStatusItem *item = &this->stat->itemList[index];
	if(!item->isExist){return;}
	// アイテム描画
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, item->x, item->z, item->y);
	engineMathMat4RotateZ(&tempmat1, -this->stat->rotg);
	engineMathMat4RotateY(&tempmat1, -this->stat->rotv);
	engineMathMat4RotateX(&tempmat1, -this->stat->roth);
	engineMathMat4Scale(&tempmat1, 0.03, -0.03, 1.0);
	engineGraphicEngineSetMatrix(&tempmat1);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	e3dImageDraw(&this->imgItem[item->type][(this->step / 10) % 2]);
}

// フキダシ描画
static void drawBalloon(struct pageCartridgeStory *this, struct engineMathMatrix44 *mat, int width, int height){
	struct engineMathMatrix44 tempmat1;
	if(width < 30){width = 30;}
	if(height < 30){height = 30;}
	double originx = -width * 0.5;
	if(originx > -15){originx = -15;}
	if(originx < -(width - 15)){originx = -(width - 15);}
	double originy = -(10 + height);

	// 三角
	engineMathMat4Copy(&tempmat1, mat);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[0]);
	// 左上
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + 10, originy + 10, 0.0);
	engineMathMat4RotateZ(&tempmat1, CONST_DEG2RAD * 0);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[1]);
	// 右上
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + width - 10, originy + 10, 0.0);
	engineMathMat4RotateZ(&tempmat1, CONST_DEG2RAD * 90);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[1]);
	// 右下
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + width - 10, originy + height - 10, 0.0);
	engineMathMat4RotateZ(&tempmat1, CONST_DEG2RAD * 180);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[1]);
	// 左下
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + 10, originy + height - 10, 0.0);
	engineMathMat4RotateZ(&tempmat1, CONST_DEG2RAD * 270);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[1]);
	// 中央
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + 10, originy, 0.0);
	engineMathMat4Scale(&tempmat1, (width - 20), height, 10);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[2]);
	// 左
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx, originy + 10, 0.0);
	engineMathMat4Scale(&tempmat1, 10, (height - 20), 10);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[2]);
	// 右
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, originx + height - 10, originy + 10, 0.0);
	engineMathMat4Scale(&tempmat1, 10, (height - 20), 10);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dImageDraw(&this->imgBalloon[2]);
}

// 描画
static void draw(struct pageCartridgeStory *this){
	engineGraphicEngineClearAll();
	engineGraphicStencilClear();
	engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_NONE);

	// 描画準備
	createBuffer(this);
	hknwCharaManagerCreateBuffer(&this->stat->charaManager);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
	if(this->isStart){
		struct engineMathMatrix44 worldMat;
		struct engineMathMatrix44 tempmat1;
		struct engineMathMatrix44 tempmat2;
		// 射影行列作成
		double xFrustum = 0.050 * global.screen.aspect.x * ((global.screen.h < global.screen.w) ? ((double)global.screen.w / global.screen.h) : 1);
		double yFrustum = 0.050 * global.screen.aspect.y * ((global.screen.h > global.screen.w) ? ((double)global.screen.h / global.screen.w) : 1);
		engineMathMat4Frustum(&e3dMatFrustum, -xFrustum, xFrustum, -yFrustum, yFrustum, 0.1, 100);
		engineMathMat4Ortho(&e3dMatOrtho, -global.screen.offset.x, global.screen.w + global.screen.offset.x, global.screen.h + global.screen.offset.y, -global.screen.offset.y, -1, 1);
		// ワールド回転行列作成
		engineMathMat4Identity(&tempmat1);
		engineMathMat4RotateX(&tempmat1, this->stat->roth);
		engineMathMat4RotateY(&tempmat1, this->stat->rotv);
		engineMathMat4RotateZ(&tempmat1, this->stat->rotg);
		// ワールド行列作成
		engineMathMat4Copy(&worldMat, &e3dMatFrustum);
		engineMathMat4Translate(&worldMat, 0, 0, -this->stat->cameraDist);
		engineMathMat4Multiply(&worldMat, &worldMat, &tempmat1);
		engineMathMat4Translate(&worldMat, -this->stat->camerax, -this->stat->cameraz, -this->stat->cameray);
		// キャラクターの回転打ち消し変数の計算
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageStoryStatusChara *chara = &this->stat->charaList[i];
			if(!chara->isExist){continue;}
			engineMathMat4Copy(&tempmat2, &tempmat1);
			engineMathMat4RotateZ(&tempmat2, chara->rotg);
			chara->roth = utilMathAsin(tempmat2.m12);
			chara->rotv = -utilMathAtan2(tempmat2.m02, tempmat2.m22);
		}
		// レターボックス描画準備
		double openParam = (double)(this->step - 10) / (60 - 10);
		if(openParam > 1){openParam = 1;}else if(openParam < 0){openParam = 0;}
		double letterBoxColor = 0.2 * openParam;
		double letterBoxSize = 32;
		letterBoxSize -= (1 - openParam * openParam * openParam * openParam * openParam) * (letterBoxSize - (global.screen.h * 0.5));

		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
		// 表示エリアマスク描画
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, 0.0, global.screen.h * 0.5, 0.0);
		engineMathMat4Scale(&tempmat1, 1.0, global.screen.h - letterBoxSize * 2, 1.0);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_MASK_1);
		e3dImageDraw(&this->imgViewMask);
		// レターボックス描画
		engineGraphicEngineSetMatrix(&e3dMatOrtho);
		engineGraphicEngineSetColorRgba(letterBoxColor, letterBoxColor, letterBoxColor, 1.0);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ0);
		e3dImageDraw(&this->imgMask);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1);
		// 背景描画
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, -((this->step / 2) % 32), -((this->step / 8) % 32), 0.0);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
		e3dImageDraw(&this->imgWallpaper);

		// 深度バッファのリセット androidではここでクリアしておかないとうまく描画ができない
		engineGraphicEngineClearDepth();

		// ハコニワ描画
		engineGraphicEngineBindTexture(e3dIdSystemImageNearest);
		for(int i = 0; i < this->stat->mapLength; i++){
			struct pageStoryStatusMap *mapData = &this->stat->mapList[i];
			if(mapData->map == NULL){continue;}
			hknwMapDraw(mapData->map, &worldMat);
		}
		engineGraphicEngineBindTexture(e3dIdSystemImageLinear);

		// 3D描画準備
		engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_NORMAL);
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		// 影描画
		engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 1.0);
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageStoryStatusChara *chara = &this->stat->charaList[i];
			if(!chara->isExist){continue;}
			if(!chara->isShadow){continue;}
			double x = chara->x - chara->rad * utilMathSin(chara->rotg);
			double y = chara->y;
			double height = chara->z - 1; // 地面位置固定
			engineMathMat4Copy(&tempmat1, &worldMat);
			engineMathMat4Translate(&tempmat1, x, chara->z - height + 0.01, y);
			engineMathMat4Scale(&tempmat1, 1.2 * chara->rad, 1.0, 1.2 * chara->rad);
			engineGraphicEngineSetMatrix(&tempmat1);
			e3dImageDraw(&this->imgShadow);
//			if(false){
//				// ついでにあたり判定の目安描画
//				engineMathMat4Copy(&tempmat1, &worldMat);
//				engineMathMat4Translate(&tempmat1, chara->x, chara->z + chara->rad, chara->y);
//				engineMathMat4RotateY(&tempmat1, CONST_DEG2RAD * this->step * 5);
//				engineMathMat4Scale(&tempmat1, 2.0 * chara->rad, 2.0 * chara->rad, 1.0);
//				engineMathMat4RotateX(&tempmat1, CONST_DEG2RAD * 90);
//				engineGraphicEngineSetMatrix(&tempmat1);
//				engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
//				e3dImageDraw(&this->imgShadow);
//				engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 1.0);
//			}
		}

		// アイテム描画
		for(int i = 0; i < this->stat->itemLength; i++){drawItem(this, i, &worldMat);}

		// エフェクトリスト描画
		e3dTransListManagerDraw(&this->transManagerEffect, &worldMat, 1.0, 1.0 ,1.0, 1.0);

		// キャラクター描画
		engineGraphicEngineBindVertVBO(this->stat->charaManager.e3dIdCharaVert);
		engineGraphicEngineBindTexcVBO(this->stat->charaManager.e3dIdCharaTexc);
		engineGraphicEngineBindFaceIBO(this->stat->charaManager.e3dIdCharaFace);
		// ハコニワに隠れない部分の描画
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_GE1_WRITE_INCR);
		for(int i = 0; i < this->stat->charaLength; i++){drawChara(this, i, &worldMat);}
		// ハコニワに隠れる部分のステンシル描画
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1_MASK_0);
		engineGraphicEngineIgnoreDepthMask(true);
		engineGraphicEngineIgnoreDepthTest(true);
		for(int i = 0; i < this->stat->charaLength; i++){drawChara(this, i, &worldMat);}

		// 2D描画準備
		engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		// 表示エリア内でハコニワに隠れる部分のマスク描画
		engineMathMat4Copy(&tempmat1, &e3dMatOrtho);
		engineMathMat4Translate(&tempmat1, 0.0, global.screen.h * 0.5, 0.0);
		engineMathMat4Scale(&tempmat1, 1.0, global.screen.h - letterBoxSize * 2, 1.0);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 0.3);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ0);
		e3dImageDraw(&this->imgViewMask);
		// 表示エリアマスク再描画
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_MASK_1);
		e3dImageDraw(&this->imgViewMask);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1);

		// 3D描画準備
		engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_NORMAL);
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		// 深度バッファのリセット
		engineGraphicEngineClearDepth();
		// フキダシ描画
		engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
		for(int i = 0; i < this->stat->charaLength; i++){
			struct pageStoryStatusChara *chara = &this->stat->charaList[i];
			if(!chara->isExist || chara->emotion == PAGESTORYEMOTION_NONE){continue;}
			double rx2 = 1.5 * chara->rad * utilMathSin(this->stat->rotg);
			double ry2 = 1.5 * chara->rad * utilMathCos(this->stat->rotg);
			double rx1 = 1.5 * chara->rad * utilMathSin(-chara->rotg);
			double ry1 = 1.5 * chara->rad * utilMathCos(-chara->rotg);
			engineMathMat4Copy(&tempmat1, &worldMat);
			engineMathMat4Translate(&tempmat1, chara->x + rx1 + rx2, chara->z + ry1 + ry2, chara->y);
			engineMathMat4RotateZ(&tempmat1, -this->stat->rotg);
			engineMathMat4RotateY(&tempmat1, -this->stat->rotv);
			engineMathMat4RotateX(&tempmat1, -this->stat->roth);
			engineMathMat4Scale(&tempmat1, 0.03 * chara->emotionScale, -0.03 * chara->emotionScale, 1.0);
			double param = utilMathCos(CONST_DEG2RAD * chara->stepBalloon * 10);
			double scale = (chara->stepBalloon < 48) ? (1 - (double)chara->stepBalloon / 50) : 0.04;
			engineMathMat4Scale(&tempmat1, 1 - scale * param, 1 + scale * param, 1);
			drawBalloon(this, &tempmat1, 50, 50);
			// 感情
			engineMathMat4Copy(&tempmat2, &tempmat1);
			engineMathMat4Translate(&tempmat2, 0.0, -(10 + 50 * 0.5), 0.1);
			switch(chara->emotion){
				case PAGESTORYEMOTION_HAPPY:
					engineMathMat4RotateZ(&tempmat2, 0.25 * utilMathCos(CONST_DEG2RAD * this->step * 10));
					engineGraphicEngineSetMatrix(&tempmat2);
					e3dImageDraw(&this->imgEmotion[0]);
					break;
				case PAGESTORYEMOTION_EXCLAMATION:
					param = utilMathAbs(utilMathCos(CONST_DEG2RAD * this->step * 5));
					engineMathMat4Translate(&tempmat2, 0.0, 20.0 - 10 * param, 0.0);
					engineMathMat4Scale(&tempmat2, 1.0, 1.0 + 0.25 * param, 1.0);
					engineMathMat4Translate(&tempmat2, 0.0, -15.0, 0.0);
					engineGraphicEngineSetMatrix(&tempmat2);
					e3dImageDraw(&this->imgEmotion[1]);
					break;
				case PAGESTORYEMOTION_QUESTION:
					engineMathMat4Translate(&tempmat2, 0.0,  15.0, 0.0);
					engineMathMat4Scale(&tempmat2, 1.0, 1.0 + 0.25 * utilMathCos(CONST_DEG2RAD * this->step * 5), 1.0);
					engineMathMat4Translate(&tempmat2, 0.0, -15.0, 0.0);
					engineGraphicEngineSetMatrix(&tempmat2);
					e3dImageDraw(&this->imgEmotion[2]);
					break;
				case PAGESTORYEMOTION_SAD:
					param = utilMathAbs(utilMathCos(CONST_DEG2RAD * this->step * 2));
					engineMathMat4Translate(&tempmat2, 0.0,  15.0, 0.0);
					engineMathMat4Scale(&tempmat2, 1.0 - 0.25 * param, 1.0 + 0.25 * param, 1.0);
					engineMathMat4Translate(&tempmat2, 0.0, -15.0, 0.0);
					engineGraphicEngineSetMatrix(&tempmat2);
					e3dImageDraw(&this->imgEmotion[3]);
					break;
				case PAGESTORYEMOTION_ANGRY:
					engineMathMat4RotateZ(&tempmat2, 0.25 * utilMathCos(CONST_DEG2RAD * this->step * 20));
					engineGraphicEngineSetMatrix(&tempmat2);
					e3dImageDraw(&this->imgEmotion[4]);
					break;
				default: break;
			}
		}

		// 2D描画準備
		engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
		engineGraphicEngineBindVertVBO(this->e3dIdVert);
		engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
		engineGraphicEngineBindFaceIBO(this->e3dIdFace);
		// 全体マスク描画
		engineGraphicEngineSetMatrix(&e3dMatOrtho);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_MASK_1);
		e3dImageDraw(&this->imgMask);
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_READ_EQ1);

		// リスト設定
		double paramButton = (double)(this->step - 60) / (90 - 60);
		if(paramButton > 1){paramButton = 1;}else if(paramButton < 0){paramButton = 0;}
		e3dTransSetTranslate(&this->btnBack.trans, -100 * (1 - paramButton) * (1 - paramButton), 0.0, 0.0);
		e3dTransSetTranslate(&this->btnNext.trans,  100 * (1 - paramButton) * (1 - paramButton), 0.0, 0.0);

		// リスト描画
		e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

		// ステンシルマスク解除
		engineGraphicStencilSetMode(ENGINEGRAPHICSTENCILMODE_NONE);
	}

	// ポップアップ描画
	popupManagerDraw(&this->popup);

	// メモリートレースボタン描画
	partsButtonMemoryDraw();

	engineGraphicEngineFlush();
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct pageCartridgeStory *this){
	popupManagerPause(&this->popup);
}

// 破棄
static void dispose(struct pageCartridgeStory *this){
	popupManagerDispose(&this->popup);
	objectStoryEngineDispose(this->stat);
	e3dTransListManagerDispose(&this->transManager);
	e3dTransListManagerDispose(&this->transManagerEffect);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->stat);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ページカートリッジ作成
struct pageCartridge *cartridgeStory(struct pageStoryStatus *stat){
	struct pageCartridgeStory *this = (struct pageCartridgeStory*)engineUtilMemoryCalloc(1, sizeof(struct pageCartridgeStory));
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
void pageStoryInit(int id, int nextStageId){
	struct pageStoryStatus *stat = (struct pageStoryStatus*)engineUtilMemoryCalloc(1, sizeof(struct pageStoryStatus));
	stat->id = id;
	stat->nextStageId = nextStageId;
	pageCartridgePush(cartridgeStory(stat));
}
void pageStoryInitTest(){pageStoryInit(1, 0);}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

