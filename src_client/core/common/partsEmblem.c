#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プロトタイプ宣言 直接描画処理は今の所不可視にして、e3dTransListManagerを通しての描画以外は許可しない
void partsEmblemDraw(struct partsEmblem *this, struct engineMathMatrix44 *mat, double alpha);

static void partsEmblemDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct partsEmblem *this = (struct partsEmblem*)((char*)that - offsetof(struct partsEmblem, trans));
	partsEmblemDraw(this, mat, a);
}

// ステージタイプ おおきなくくり
static int getType1(struct partsEmblem *this){
	if(this->mark <= 0){}
	else if(this->mark <=  3 + 0){return 1;}
	else if(this->mark <=  3 + 6){return 2;}
	else if(this->mark <=  9 + 9){return 3;}
	else if(this->mark <= 18 + 9){return 4;}
	return 0;
}

// ステージタイプ ちいさなくくり
static int getType2(struct partsEmblem *this){
	if(this->mark <= 0){}
	else if(this->mark <=  3 + 0){return this->mark -  0;}
	else if(this->mark <=  3 + 6){return this->mark -  3;}
	else if(this->mark <=  9 + 9){return this->mark -  9;}
	else if(this->mark <= 18 + 9){return this->mark - 18;}
	return 0;
}

static void setTexpos(int xin, int yin, int win, int hin, int *xout, int *yout, int *wout, int *hout){
	*xout = xin;
	*yout = yin;
	*wout = win;
	*hout = hin;
}

// ステージエンブレム初期化 ステージの設定
void partsEmblemInitStage(struct partsEmblem *this, int icon, int mark, int score, bool isButton, bool isReplay){
	this->icon = icon;
	this->mark = mark;
	this->score = (int)utilMathCeil(score / 10.0);
	this->isStage = (mark > 0);
	this->isButton = isButton;
	this->isReplay = isReplay;
	if(this->isButton){
		if(this->isStage){
			partsButtonHexInit(&this->btnStart);
			if(this->isReplay){
				partsButtonHexInit(&this->btnReplay);
			}
		}else{
			partsButtonHexInit(&this->btnStart);
		}
	}
}

// 配列にステージエンブレムの描画情報を追加
void partsEmblemCreateArray(struct partsEmblem *this, int x, int y, double r){
	e3dTransInit(&this->trans);
	e3dTransSetTranslate(&this->trans, x, y, 0.0);
	this->trans.draw = partsEmblemDraw_trans;

	int vertIndex;
	int faceIndex;
	int faceNum;
	int tetraNum;
	double cos30[12];
	double sin30[12];
	cos30[ 0] = CONST_COS000; sin30[ 0] = CONST_SIN000;
	cos30[ 1] = CONST_COS030; sin30[ 1] = CONST_SIN030;
	cos30[ 2] = CONST_COS060; sin30[ 2] = CONST_SIN060;
	cos30[ 3] = CONST_COS090; sin30[ 3] = CONST_SIN090;
	cos30[ 4] = CONST_COS120; sin30[ 4] = CONST_SIN120;
	cos30[ 5] = CONST_COS150; sin30[ 5] = CONST_SIN150;
	cos30[ 6] = CONST_COS180; sin30[ 6] = CONST_SIN180;
	cos30[ 7] = CONST_COS210; sin30[ 7] = CONST_SIN210;
	cos30[ 8] = CONST_COS240; sin30[ 8] = CONST_SIN240;
	cos30[ 9] = CONST_COS270; sin30[ 9] = CONST_SIN270;
	cos30[10] = CONST_COS300; sin30[10] = CONST_SIN300;
	cos30[11] = CONST_COS330; sin30[11] = CONST_SIN330;

	double r0 = 36;
	double r1 = 33;
	double r2 =  4;
	double r3 = 15;
	double ex = this->isButton ? ((this->isStage ? -75 : -20)) : 0;
	double ey = this->isButton ? ((this->isStage ? -10 :   0)) : 0;

	if(this->icon > 0){
		// ヘックスエンブレム
		vertIndex = engineGraphicBufferVretIndexGet();
		double cr = utilMathCos(-r);
		double sr = utilMathSin(-r);
		int tx, ty, tw, th;
		if(this->isStage){
			switch(this->icon){
				case  1: setTexpos(TEXPOS_SYSTEM_ICONSTAGE01_XYWH, &tx, &ty, &tw, &th); break;
				case  2: setTexpos(TEXPOS_SYSTEM_ICONSTAGE02_XYWH, &tx, &ty, &tw, &th); break;
				case  3: setTexpos(TEXPOS_SYSTEM_ICONSTAGE03_XYWH, &tx, &ty, &tw, &th); break;
				case  4: setTexpos(TEXPOS_SYSTEM_ICONSTAGE04_XYWH, &tx, &ty, &tw, &th); break;
				case  5: setTexpos(TEXPOS_SYSTEM_ICONSTAGE05_XYWH, &tx, &ty, &tw, &th); break;
				case  6: setTexpos(TEXPOS_SYSTEM_ICONSTAGE06_XYWH, &tx, &ty, &tw, &th); break;
				case  7: setTexpos(TEXPOS_SYSTEM_ICONSTAGE07_XYWH, &tx, &ty, &tw, &th); break;
				case  8: setTexpos(TEXPOS_SYSTEM_ICONSTAGE08_XYWH, &tx, &ty, &tw, &th); break;
				case  9: setTexpos(TEXPOS_SYSTEM_ICONSTAGE09_XYWH, &tx, &ty, &tw, &th); break;
				case 10: setTexpos(TEXPOS_SYSTEM_ICONSTAGE10_XYWH, &tx, &ty, &tw, &th); break;
				case 11: setTexpos(TEXPOS_SYSTEM_ICONSTAGE11_XYWH, &tx, &ty, &tw, &th); break;
				case 12: setTexpos(TEXPOS_SYSTEM_ICONSTAGE12_XYWH, &tx, &ty, &tw, &th); break;
				case 13: setTexpos(TEXPOS_SYSTEM_ICONSTAGE13_XYWH, &tx, &ty, &tw, &th); break;
				case 14: setTexpos(TEXPOS_SYSTEM_ICONSTAGE14_XYWH, &tx, &ty, &tw, &th); break;
				case 15: setTexpos(TEXPOS_SYSTEM_ICONSTAGE15_XYWH, &tx, &ty, &tw, &th); break;
				case 16: setTexpos(TEXPOS_SYSTEM_ICONSTAGE16_XYWH, &tx, &ty, &tw, &th); break;
				case 17: setTexpos(TEXPOS_SYSTEM_ICONSTAGE17_XYWH, &tx, &ty, &tw, &th); break;
				case 18: setTexpos(TEXPOS_SYSTEM_ICONSTAGE18_XYWH, &tx, &ty, &tw, &th); break;
				case 19: setTexpos(TEXPOS_SYSTEM_ICONSTAGE19_XYWH, &tx, &ty, &tw, &th); break;
				case 20: setTexpos(TEXPOS_SYSTEM_ICONSTAGE20_XYWH, &tx, &ty, &tw, &th); break;
				case 21: setTexpos(TEXPOS_SYSTEM_ICONSTAGE21_XYWH, &tx, &ty, &tw, &th); break;
				case 22: setTexpos(TEXPOS_SYSTEM_ICONSTAGE22_XYWH, &tx, &ty, &tw, &th); break;
				case 23: setTexpos(TEXPOS_SYSTEM_ICONSTAGE23_XYWH, &tx, &ty, &tw, &th); break;
				case 24: setTexpos(TEXPOS_SYSTEM_ICONSTAGE24_XYWH, &tx, &ty, &tw, &th); break;
				case 25: setTexpos(TEXPOS_SYSTEM_ICONSTAGE25_XYWH, &tx, &ty, &tw, &th); break;
				case 26: setTexpos(TEXPOS_SYSTEM_ICONSTAGE26_XYWH, &tx, &ty, &tw, &th); break;
				case 27: setTexpos(TEXPOS_SYSTEM_ICONSTAGE27_XYWH, &tx, &ty, &tw, &th); break;
				default: setTexpos(TEXPOS_SYSTEM_TEST_XYWH, &tx, &ty, &tw, &th); break;
			}
		}else{
			switch(this->icon){
				case 1: setTexpos(TEXPOS_SYSTEM_ICONSTORY01_XYWH, &tx, &ty, &tw, &th); break;
				case 2: setTexpos(TEXPOS_SYSTEM_ICONSTORY02_XYWH, &tx, &ty, &tw, &th); break;
				case 3: setTexpos(TEXPOS_SYSTEM_ICONSTORY03_XYWH, &tx, &ty, &tw, &th); break;
				case 4: setTexpos(TEXPOS_SYSTEM_ICONSTORY04_XYWH, &tx, &ty, &tw, &th); break;
				case 5: setTexpos(TEXPOS_SYSTEM_ICONSTORY05_XYWH, &tx, &ty, &tw, &th); break;
				default: setTexpos(TEXPOS_SYSTEM_TEST_XYWH, &tx, &ty, &tw, &th); break;
			}
		}
		double tr = ((tw < th) ? tw : th) * 0.5;
		double tcx = tx + tw * 0.5;
		double tcy = ty + th * 0.5;
		for(int i = 0; i < 6; i++){
			double c0 = cos30[i * 2 + 1];
			double s0 = sin30[i * 2 + 1];
			// 三角関数の加法定理
			double c1 = c0 * cr - s0 * sr;
			double s1 = s0 * cr + c0 * sr;
			engineGraphicBufferPushVert(ex + r0 * c0, ey + r0 * s0, 0); engineGraphicBufferPushTexc((tcx + tr * c1) / TEXSIZ_SYSTEM_W, (tcy + tr * s1) / TEXSIZ_SYSTEM_H);
			engineGraphicBufferPushVert(ex + r0 * c0, ey + r0 * s0, 0); engineGraphicBufferPushTexcWhite();
			engineGraphicBufferPushVert(ex + r1 * c0, ey + r1 * s0, 0); engineGraphicBufferPushTexcWhite();
		}
		// ヘックスエンブレム中身
		faceIndex = engineGraphicBufferFaceIndexGet();
		faceNum = 0;
		engineGraphicBufferPushFace(vertIndex, 0 + 3 * 0, 0 + 3 * 1, 0 + 3 * 5); faceNum++;
		engineGraphicBufferPushFace(vertIndex, 0 + 3 * 1, 0 + 3 * 2, 0 + 3 * 5); faceNum++;
		engineGraphicBufferPushFace(vertIndex, 0 + 3 * 2, 0 + 3 * 4, 0 + 3 * 5); faceNum++;
		engineGraphicBufferPushFace(vertIndex, 0 + 3 * 2, 0 + 3 * 3, 0 + 3 * 4); faceNum++;
		this->imgEmblem.faceIndex = faceIndex;
		this->imgEmblem.faceNum = faceNum;
		// ヘックスエンブレム枠
		faceIndex = engineGraphicBufferFaceIndexGet();
		faceNum = 0;
		for(int i = 0; i < 6; i++){
			engineGraphicBufferPushFace(vertIndex, 1 + 3 * ((i + 0) % 6), 1 + 3 * ((i + 1) % 6), 2 + 3 * ((i + 1) % 6)); faceNum++;
			engineGraphicBufferPushFace(vertIndex, 1 + 3 * ((i + 0) % 6), 2 + 3 * ((i + 1) % 6), 2 + 3 * ((i + 0) % 6)); faceNum++;
		}
		this->imgBorder.faceIndex = faceIndex;
		this->imgBorder.faceNum = faceNum;
	}

	if(this->isStage){
		// ステージマーカー
		vertIndex = engineGraphicBufferVretIndexGet();
		faceIndex = engineGraphicBufferFaceIndexGet();
		faceNum = 0;
		int type1 = getType1(this);
		for(int i = 0; i < 6; i++){
			if(type1 == 1 && i != 1){continue;}
			if(type1 == 2 && i != 2 && i != 5){continue;}
			if(type1 == 3 && i != 1 && i != 2 && i != 4){continue;}
			if(type1 == 4 && i != 1 && i != 3 && i != 5){continue;}
			double x0 = ex + (r0 * CONST_SIN060 + r2 * 0.5) * cos30[i * 2 + 0];
			double y0 = ey + (r0 * CONST_SIN060 + r2 * 0.5) * sin30[i * 2 + 0];
			for(int j = 0; j < 3; j++){
				double x1 = x0 + r2 * cos30[((i + j) * 2 + 10) % 12];
				double y1 = y0 + r2 * sin30[((i + j) * 2 + 10) % 12];
				double x2 = x1 + r3 * cos30[((i + j) * 2 +  9) % 12];
				double y2 = y1 + r3 * sin30[((i + j) * 2 +  9) % 12];
				double x3 = x1 + r3 * cos30[((i + j) * 2 + 11) % 12];
				double y3 = y1 + r3 * sin30[((i + j) * 2 + 11) % 12];
				engineGraphicBufferPushVert(x1, y1, 0); engineGraphicBufferPushTexcWhite();
				engineGraphicBufferPushVert(x2, y2, 0); engineGraphicBufferPushTexcWhite();
				engineGraphicBufferPushVert(x3, y3, 0); engineGraphicBufferPushTexcWhite();
				engineGraphicBufferPushFace(vertIndex + faceNum * 3, 0, 1, 2); faceNum++;
			}
		}
		this->imgStage.faceIndex = faceIndex;
		this->imgStage.faceNum = faceNum;
	}

	if(this->score > 0){
		// 星作成
		int star1 = this->score / 100;
		int star2 = (this->score / 10) % 10;
		int star3 = this->score % 10;
		vertIndex = engineGraphicBufferVretIndexGet();
		faceIndex = engineGraphicBufferFaceIndexGet();
		tetraNum = 0;
		// でっかい星
		for(int i = 0; i < star1; i++){
			double x = ( 95 - 20) + ex - 40 * (star1 > 1 ? ((double)i / (star1 - 1)) : 0.5);
			double y = (-10 - 20) + ey;
			double s = (  2 * 20);
			e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x, y, s, s, TEXPOS_SYSTEM_ITEMSTAR1_XYWH);
			tetraNum++;
		}
		if(star1 > 0 || star2 > 0 || star3 > 0){
			// 中くらいの星
			for(int i = 0; i < 10; i++){
				double x = (125 - 10) + ex - 80 * ((double)i / (10 - 1));
				double y = ( 18 - 10) + ey;
				double s = (  2 * 10);
				if(10 - i <= star2){e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x, y, s, s, TEXPOS_SYSTEM_PARAMELEMSTAR1_XYWH);}
				else{e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x, y, s, s, TEXPOS_SYSTEM_PARAMELEMSTAR2_XYWH);}
				tetraNum++;
			}
			// ちっこい星
			for(int i = 0; i < 10; i++){
				double x = (125 - 5) + ex - 80 * ((double)i / (10 - 1));
				double y = ( 37 - 5) + ey;
				double s = (  2 * 5);
				if(10 - i <= star3){e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x, y, s, s, TEXPOS_SYSTEM_PARAMELEMCOIN1_XYWH);}
				else{e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x, y, s, s, TEXPOS_SYSTEM_PARAMELEMCOIN2_XYWH);}
				tetraNum++;
			}
		}
		for(int i = 0; i < tetraNum; i++){engineGraphicBufferPushTetraFace(vertIndex + i * 4);}
		this->imgStar.faceIndex = faceIndex;
		this->imgStar.faceNum = tetraNum * 2;
	}

	if(this->isButton){
		// 枠作成
		if(this->isStage){
			e3dImageWhiteCreateArray(&this->imgBox, -100, -40, 200, 80);
		}else{
			e3dImageWhiteCreateArray(&this->imgBox, -60, -25, 145, 50);
		}

		// ボタン作成
		if(this->isStage){
			partsButtonHexCreateArray(&this->btnStart,  80.0 + (32 * CONST_SIN060) * CONST_COS060,  0.0 + (32 * CONST_SIN060) * CONST_SIN060, 35, TEXPOS_SYSTEM_ICONPLAY_XYWH);
			e3dTransSetProperty(&this->btnStart.trans, &this->trans, 0);
			this->btnStart.rotInner = r;
			this->btnStart.rotOuter = CONST_PI * -0.5;
			if(this->isReplay){
				partsButtonHexCreateArray(&this->btnReplay, 80.0 - (32 * CONST_SIN060) * CONST_COS060,  0.0 - (32 * CONST_SIN060) * CONST_SIN060, 25, TEXPOS_SYSTEM_ICONREPLAY_XYWH);
				e3dTransSetProperty(&this->btnReplay.trans, &this->trans, 0);
				this->btnReplay.rotInner = r;
				this->btnReplay.rotOuter = CONST_PI * 0.5;
			}
		}else{
			partsButtonHexCreateArray(&this->btnStart, 50, 0, 35, TEXPOS_SYSTEM_ICONPLAY_XYWH);
			e3dTransSetProperty(&this->btnStart.trans, &this->trans, 0);
			this->btnStart.rotInner = r;
			this->btnStart.rotOuter = CONST_PI * 0.5;
		}

		// ロック作成
		e3dImageCreateArray(&this->imgLock, TEXSIZ_SYSTEM_WH, -30, -30, 60, 60, TEXPOS_SYSTEM_ICONLOCK_XYWH);
	}
}

// ステージエンブレム描画
void partsEmblemDraw(struct partsEmblem *this, struct engineMathMatrix44 *mat, double alpha){
	// バッファ登録
	e3dTransBindBuffer(&this->trans);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(&this->trans, &tempmat1);
	engineGraphicEngineSetMatrix(&tempmat1);

	// 枠描画
	if(this->isButton){
		e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha * 0.5);
		e3dImageDraw(&this->imgBox);
	}

	if(this->icon > 0){
		// エンブレム描画
		e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);
		e3dImageDraw(&this->imgEmblem);
		if(this->isButton){e3dTransBindColor(&this->trans, 0.0, 0.0, 0.0, alpha);}
		if(!this->isButton){e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);}
		e3dImageDraw(&this->imgBorder);
	}

	if(this->score > 0){
		// 星描画
		e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);
		e3dImageDraw(&this->imgStar);
	}

	if(this->isStage){
		// ステージマーカー描画
		int markerNum = 18;
		switch(getType1(this)){
			case 1: markerNum = 3; e3dTransBindColor(&this->trans, 1.0, 1.0, 0.5, alpha * 0.8); break;
			case 2: markerNum = 6; e3dTransBindColor(&this->trans, 0.5, 1.0, 1.0, alpha * 0.8); break;
			case 3: markerNum = 9; e3dTransBindColor(&this->trans, 0.5, 1.0, 0.5, alpha * 0.8); break;
			case 4: markerNum = 9; e3dTransBindColor(&this->trans, 1.0, 0.5, 0.5, alpha * 0.8); break;
			default: e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha * 0.8); break;
		}
		int type2 = getType2(this);
		engineGraphicEngineDrawIndex(this->imgStage.faceIndex * 3,  type2 * 3);
		if(markerNum > type2){
			e3dTransBindColor(&this->trans, 0.5, 0.5, 0.5, alpha * 0.8);
			engineGraphicEngineDrawIndex((this->imgStage.faceIndex + type2) * 3,  (markerNum - type2) * 3);
		}
	}

	// ボタン描画
	if(this->isButton){
		if(this->isStage){
			partsButtonHexDraw(&this->btnStart, mat, alpha);
			if(this->isReplay){
				partsButtonHexDraw(&this->btnReplay, mat, alpha);
			}
		}else{
			partsButtonHexDraw(&this->btnStart, mat, alpha);
		}
	}

	// ロック描画
	if(this->isButton && this->isLock){
		engineMathMat4Copy(&tempmat1, mat);
		e3dTransMultMatrix(&this->trans, &tempmat1);
		engineMathMat4Translate(&tempmat1, 10.0, 0.0, 0.0);
		engineMathMat4RotateZ(&tempmat1, this->btnStart.rotInner);
		engineGraphicEngineSetMatrix(&tempmat1);
		e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);
		e3dImageDraw(&this->imgLock);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

