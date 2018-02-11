#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 配列に画像の描画情報を追加
void e3dImageCreateArray(struct e3dImage *this, int imgw, int imgh, double x, double y, double w, double h, int tu, int tv, int tw, int th){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int tetraNum = 1;
	// 頂点座標データを生成
	engineGraphicBufferPushTetraVert(x, y, w, h);
	// テクスチャ座標データを生成
	engineGraphicBufferPushTetraTexc(imgw, imgh, tu, tv, tw, th);
	// インデックスデータを作成
	for(int i = 0; i < tetraNum; i++){engineGraphicBufferPushTetraFace(vertIndex + i * 4);}

	if(this != NULL){
		this->faceIndex = faceIndex;
		this->faceNum = tetraNum * 2;
	}
}

// ----------------------------------------------------------------

// 配列に箱の描画情報を追加
void e3dImageBoxCreateArray(struct e3dImage *this, double x, double y, double w, double h, int tu, int tv, int tw, int th){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int tetraNum = 9;
	int w10 = utilMathFloor(tw * 0.5);
	int h10 = utilMathFloor(th * 0.5);
	int w05 = utilMathFloor((w < w10 ? w : w10) * 0.5);
	int h05 = utilMathFloor((h < h10 ? h : h10) * 0.5);

	// 頂点テクスチャ座標データを生成
	double x1 = x          ; double u1 = (double)(tu                ) / TEXSIZ_SYSTEM_W;
	double x2 = x +     w05; double u2 = (double)(tu + w10          ) / TEXSIZ_SYSTEM_W;
	double x3 = x + w - w05; double u3 = (double)(tu + w10 + 1      ) / TEXSIZ_SYSTEM_W;
	double x4 = x + w      ; double u4 = (double)(tu + w10 + 1 + w10) / TEXSIZ_SYSTEM_W;
	double y1 = y          ; double v1 = (double)(tv                ) / TEXSIZ_SYSTEM_H;
	double y2 = y +     h05; double v2 = (double)(tv + h10          ) / TEXSIZ_SYSTEM_H;
	double y3 = y + h - h05; double v3 = (double)(tv + h10 + 1      ) / TEXSIZ_SYSTEM_H;
	double y4 = y + h      ; double v4 = (double)(tv + h10 + 1 + h10) / TEXSIZ_SYSTEM_H;
	engineGraphicBufferPushVert(x1, y1, 0.0); engineGraphicBufferPushVert(x2, y1, 0.0); engineGraphicBufferPushVert(x3, y1, 0.0); engineGraphicBufferPushVert(x4, y1, 0.0);
	engineGraphicBufferPushVert(x1, y2, 0.0); engineGraphicBufferPushVert(x2, y2, 0.0); engineGraphicBufferPushVert(x3, y2, 0.0); engineGraphicBufferPushVert(x4, y2, 0.0);
	engineGraphicBufferPushVert(x1, y3, 0.0); engineGraphicBufferPushVert(x2, y3, 0.0); engineGraphicBufferPushVert(x3, y3, 0.0); engineGraphicBufferPushVert(x4, y3, 0.0);
	engineGraphicBufferPushVert(x1, y4, 0.0); engineGraphicBufferPushVert(x2, y4, 0.0); engineGraphicBufferPushVert(x3, y4, 0.0); engineGraphicBufferPushVert(x4, y4, 0.0);
	engineGraphicBufferPushTexc(u1, v1); engineGraphicBufferPushTexc(u2, v1); engineGraphicBufferPushTexc(u3, v1); engineGraphicBufferPushTexc(u4, v1);
	engineGraphicBufferPushTexc(u1, v2); engineGraphicBufferPushTexc(u2, v2); engineGraphicBufferPushTexc(u3, v2); engineGraphicBufferPushTexc(u4, v2);
	engineGraphicBufferPushTexc(u1, v3); engineGraphicBufferPushTexc(u2, v3); engineGraphicBufferPushTexc(u3, v3); engineGraphicBufferPushTexc(u4, v3);
	engineGraphicBufferPushTexc(u1, v4); engineGraphicBufferPushTexc(u2, v4); engineGraphicBufferPushTexc(u3, v4); engineGraphicBufferPushTexc(u4, v4);
	// インデックスデータを作成
	engineGraphicBufferPushFace(vertIndex,  0,  1,  5); engineGraphicBufferPushFace(vertIndex,  5,  4,  0);
	engineGraphicBufferPushFace(vertIndex,  1,  2,  6); engineGraphicBufferPushFace(vertIndex,  6,  5,  1);
	engineGraphicBufferPushFace(vertIndex,  2,  3,  7); engineGraphicBufferPushFace(vertIndex,  7,  6,  2);
	engineGraphicBufferPushFace(vertIndex,  4,  5,  9); engineGraphicBufferPushFace(vertIndex,  9,  8,  4);
	engineGraphicBufferPushFace(vertIndex,  5,  6, 10); engineGraphicBufferPushFace(vertIndex, 10,  9,  5);
	engineGraphicBufferPushFace(vertIndex,  6,  7, 11); engineGraphicBufferPushFace(vertIndex, 11, 10,  6);
	engineGraphicBufferPushFace(vertIndex,  8,  9, 13); engineGraphicBufferPushFace(vertIndex, 13, 12,  8);
	engineGraphicBufferPushFace(vertIndex,  9, 10, 14); engineGraphicBufferPushFace(vertIndex, 14, 13,  9);
	engineGraphicBufferPushFace(vertIndex, 10, 11, 15); engineGraphicBufferPushFace(vertIndex, 15, 14, 10);

	if(this != NULL){
		this->faceIndex = faceIndex;
		this->faceNum = tetraNum * 2;
	}
}

// ----------------------------------------------------------------

// 文字コード(utf8)を確認
static unsigned int getUtf8Code(unsigned char *word){
	unsigned int code = 0;
	if(*word <= 0x7f){code = *(word + 0) & 0x7f;}
	else if(*word <= 0xbf){}
	else if(*word <= 0xdf){code = ((*(word + 0) & 0x1f) << 6) + (*(word + 1) & 0x3f);}
	else if(*word <= 0xef){code = ((*(word + 0) & 0x0f) << 12) + ((*(word + 1) & 0x3f) << 6) + (*(word + 2) & 0x3f);}
	else if(*word <= 0xf7){code = ((*(word + 0) & 0x07) << 18) + ((*(word + 1) & 0x3f) << 12) + ((*(word + 2) & 0x3f) << 6) + (*(word + 3) & 0x3f);}
	return code;
}

// 行数の計算
static int getRowNum(char *text){
	int rownum = 1;
	while(*text){
		if(*text == '\n'){rownum++;}
		text++;
	}
	return rownum;
}

// 行に含まれる文字数の計算
static int getColNum(char *text){
	int colnum = 0;
	while(*text && *text != '\n'){
		unsigned int code = getUtf8Code((unsigned char *)text);
		if(0x1f <= code  && code !=0x7f){colnum++;}
		text++;
	}
	return colnum;
}

// 配列に文字列(utf8)の描画情報を追加
void e3dImageTextCreateArray(struct e3dImage *this, double x, double y, char *text, double scale, int xalign, int yalign){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int tetraNum = 0;
	int row = 0;
	int col = 0;
	int rowmax = getRowNum(text);
	int colmax = getColNum(text);
	while(*text){
		if(*text == '\n'){
			row++;
			col = 0;
			colmax = getColNum(text + 1);
		}else{
			unsigned int code = getUtf8Code((unsigned char *)text);
			if(0x1f <= code && code !=0x7f){
				int texposu = TEXPOS_SYSTEM_FONTABCD_X;
				int texposv = TEXPOS_SYSTEM_FONTABCD_Y;
				if(0x20 <= code && code <= 0x7f){code = code - 0x20;}
				else if(0x3040 <= code && code <= 0x309f){texposu = TEXPOS_SYSTEM_FONTHIRA_X; texposv = TEXPOS_SYSTEM_FONTHIRA_Y; code = code - 0x3040;}
				else if(0x30a0 <= code && code <= 0x30ff){texposu = TEXPOS_SYSTEM_FONTKATA_X; texposv = TEXPOS_SYSTEM_FONTKATA_Y; code = code - 0x30a0;}
				//else if(code == 0x97F3){texposu = TEXPOS_SYSTEM_FONTKANJI_X; texposv = TEXPOS_SYSTEM_FONTKANJI_Y; code = 0;}
				//else if(code == 0x697D){texposu = TEXPOS_SYSTEM_FONTKANJI_X; texposv = TEXPOS_SYSTEM_FONTKANJI_Y; code = 1;}
				//else if(code == 0x7D20){texposu = TEXPOS_SYSTEM_FONTKANJI_X; texposv = TEXPOS_SYSTEM_FONTKANJI_Y; code = 2;}
				//else if(code == 0x6750){texposu = TEXPOS_SYSTEM_FONTKANJI_X; texposv = TEXPOS_SYSTEM_FONTKANJI_Y; code = 3;}
				else{code = 0;}
				double x1 = x + (col - colmax * ((xalign > 0) ? 0.0 : (xalign == 0) ? 0.5 : 1.0)) * 10 * scale;
				double y1 = y + (row - rowmax * ((yalign > 0) ? 0.0 : (yalign == 0) ? 0.5 : 1.0)) * 15 * scale;
				int u1 = texposu + 20 * (code % 16);
				int v1 = texposv + 30 * utilMathFloor(code / 16);
				engineGraphicBufferPushTetraVert(x1, y1, 10 * scale, 15 * scale);
				engineGraphicBufferPushTetraTexc(TEXSIZ_SYSTEM_WH, u1, v1, 20, 30);
				tetraNum++;
				col++;
			}
		}
		text++;
	}

	// インデックスデータを作成
	for(int i = 0; i < tetraNum; i++){engineGraphicBufferPushTetraFace(vertIndex + i * 4);}

	if(this != NULL){
		this->faceIndex = faceIndex;
		this->faceNum = tetraNum * 2;
	}
}

// ----------------------------------------------------------------

// 描画
void e3dImageDraw(struct e3dImage *this){
	engineGraphicEngineDrawIndex(this->faceIndex * 3, this->faceNum * 3);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 配列に数列の描画情報を追加
void e3dNumberCreateArray(struct e3dNumber *this, double x, double y, int tu, int tv, int tw, int th, double scale, int xalign, int yalign){
	int vertIndex = engineGraphicBufferVretIndexGet();
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int tetraNum = 10;

	y -= ((yalign > 0) ? 0.0 : (yalign == 0) ? 0.5 : 1.0) * 15 * scale;
	tw /= 10;

	for(int i = 0; i < tetraNum; i++){
		engineGraphicBufferPushTetraVert(x, y, 10 * scale, 15 * scale);
		engineGraphicBufferPushTetraTexc(TEXSIZ_SYSTEM_WH, tu + tw * i, tv, tw, th);
	}

	// インデックスデータを作成
	for(int i = 0; i < tetraNum; i++){engineGraphicBufferPushTetraFace(vertIndex + i * 4);}

	this->faceIndex = faceIndex;
	this->width = 10 * scale;
	this->xalign = xalign;
}

// 描画
void e3dNumberDraw(struct e3dNumber *this, struct engineMathMatrix44 *mat, int number){
	// 桁数確認
	int length = 0;
	int temp = abs(number);
	do{temp /= 10; length++;}while(temp >= 1);

	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, ((this->xalign > 0) ? 1.0 : (this->xalign == 0) ? 0.5 : 0.0) * length * this->width, 0, 0);
	temp = abs(number);
	do{
		engineMathMat4Translate(&tempmat1, -this->width, 0, 0);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicEngineDrawIndex((this->faceIndex + (temp % 10) * 2) * 3, 2 * 3);
		temp /= 10;
	}while(temp >= 1);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

