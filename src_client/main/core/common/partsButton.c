#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ボタン初期化
void partsButtonInit(struct partsButton *this){
	this->x = 0;
	this->y = 0;
	this->w = 0;
	this->h = 0;
	this->active = false;
	this->inactive = false;
	this->trigger = false;
	this->child = NULL;
	this->zKey = false;
	this->xKey = false;
	this->cKey = false;
	this->vKey = false;
	this->inner = true;
}

// ボタン静的位置設定
void partsButtonPosition(struct partsButton *this, double x, double y, double w, double h){
	this->x = x + w * 0.5;
	this->y = y + h * 0.5;
	this->w = w;
	this->h = h;

	// 直方体のタッチ範囲
	this->hitVertexLength = 4;
	this->hitVertex0[0].x = w * -0.5;
	this->hitVertex0[0].y = h * -0.5;
	this->hitVertex0[0].z = 0;
	this->hitVertex0[1].x = w *  0.5;
	this->hitVertex0[1].y = h * -0.5;
	this->hitVertex0[1].z = 0;
	this->hitVertex0[2].x = w *  0.5;
	this->hitVertex0[2].y = h *  0.5;
	this->hitVertex0[2].z = 0;
	this->hitVertex0[3].x = w * -0.5;
	this->hitVertex0[3].y = h *  0.5;
	this->hitVertex0[3].z = 0;
}

// 領域内確認用に行列読み込み
void partsButtonSetMatrix(struct partsButton *this, struct engineMathMatrix44 *mat){
	this->isMatrix = true;
	for(int i = 0; i < this->hitVertexLength; i++){
		engineMathVec3MultiplyMat4(&this->hitVertex1[i], &this->hitVertex0[i], mat);
		this->hitVertex1[i].x = (this->hitVertex1[i].x + 1) * 0.5 * global.window.w;
		this->hitVertex1[i].y = (1 - this->hitVertex1[i].y) * 0.5 * global.window.h;
	}
}

// ボタン計算
void partsButtonCalc(struct partsButton *this){
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_BUTTON);
	partsButtonSubCalc(this, t, true);
	if(t != NULL){
		if(this->active && !t->active){
			engineCtrlTouchOwn();
		}else if(!t->dn){
			engineCtrlTouchFree();
		}
	}
}

// タッチ情報を使い回しての計算
void partsButtonSubCalc(struct partsButton *this, struct engineCtrlTouch *t, bool clickable){
	if(this->inactive || !clickable){
		// ボタン無効状態
		this->active = false;
	}else if((this->zKey && global.key.zb.isActive) || (this->xKey && global.key.xb.isActive) || (this->cKey && global.key.cb.isActive) || (this->vKey && global.key.vb.isActive)){
		// 対応キー押下中
		this->active = true;
	}else if(t == NULL){
		// タッチ無し
		this->active = false;
	}else if(t->dn){
		// ボタン押下中
		this->active = (partsButtonIsInner(this, t) == !!this->inner);
		// 子要素の範囲内では押下状態にならない
		if(this->child != NULL){this->active = this->active && !(partsButtonIsInner(this->child, t) == !!this->child->inner);}
	}else if(this->active){
		// ボタンを放した瞬間
		this->active = false;
		this->trigger = true;
	}
}

// タッチ座標のボタン領域内確認
bool partsButtonIsInner(struct partsButton *this, struct engineCtrlTouch *t){
	if(this->isMatrix){
		// すべての辺に対し対象点との外積が全て同一符号なら内部
		double crossz[sizeof(this->hitVertex1) / sizeof(*this->hitVertex1)];
		for(int i = 0; i < this->hitVertexLength; i++){
			struct engineMathVector3 *v0 = &this->hitVertex1[i];
			struct engineMathVector3 *v1 = &this->hitVertex1[(i + 1) % this->hitVertexLength];
			crossz[i] = (t->window.x - v0->x) * (v1->y - v0->y) - (v1->x - v0->x) * (t->window.y - v0->y);
		}
		// 符号がすべて等しいことを確認
		for(int i = 0; i < this->hitVertexLength - 1; i++){if(crossz[i] * crossz[i + 1] <= 0){return false;}}
		return (this->hitVertexLength > 0);
	}else{
		double x0 = this->x - this->w * 0.5;
		double y0 = this->y - this->h * 0.5;
		double x1 = x0 + this->w;
		double y1 = y0 + this->h;
		return (x0 < t->screen.x && t->screen.x < x1 && y0 < t->screen.y && t->screen.y < y1);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static void partsButtonBoxDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct partsButtonBox *this = (struct partsButtonBox*)((char*)that - offsetof(struct partsButtonBox, trans));
	partsButtonBoxDraw(this, mat, a);
}

// ボタン初期化
void partsButtonBoxInit(struct partsButtonBox *this){
	partsButtonInit((struct partsButton*)this);
	this->select = false;
}

// 配列にボタンの描画情報を追加
void partsButtonBoxCreateArray(struct partsButtonBox *this, int x, int y, int w, int h, char *text, double scale){
	e3dTransInit(&this->trans);
	this->trans.draw = partsButtonBoxDraw_trans;

	partsButtonPosition((struct partsButton*)(this), x, y, w, h);
	int x0 = -(int)(this->super.w * 0.5);
	int y0 = -(int)(this->super.h * 0.5);

	this->faceIndex = engineGraphicBufferFaceIndexGet();

	e3dImageBoxCreateArray(NULL, x0, y0, this->super.w, this->super.h, TEXPOS_SYSTEM_BUTTONBASICNORMAL_XYWH);
	e3dImageBoxCreateArray(NULL, x0, y0, this->super.w, this->super.h, TEXPOS_SYSTEM_BUTTONBASICACTIVE_XYWH);
	e3dImageBoxCreateArray(NULL, x0, y0, this->super.w, this->super.h, TEXPOS_SYSTEM_BUTTONBASICSELECT_XYWH);
	e3dImageBoxCreateArray(NULL, x0, y0, this->super.w, this->super.h, TEXPOS_SYSTEM_BUTTONBASICINACTIVE_XYWH);

	e3dImageTextCreateArray(&this->imgText, x0 + this->super.w * 0.5, y0 + this->super.h * 0.5, text, scale, 0, 0);
}

// ボタン描画
void partsButtonBoxDraw(struct partsButtonBox *this, struct engineMathMatrix44 *mat, double alpha){
	e3dTransBindBuffer(&this->trans);

	struct engineMathMatrix44 tempmat;
	engineMathMat4Copy(&tempmat, mat);
	e3dTransMultMatrix(&this->trans, &tempmat);
	engineMathMat4Translate(&tempmat, this->super.x, this->super.y, 0);
	partsButtonSetMatrix(&this->super, &tempmat);

	engineGraphicEngineSetMatrix(&tempmat);
	// ボタン枠描画
	bool isInactive = (this->super.inactive && !this->super.active && !this->select);
	int index = this->faceIndex + 18 * (isInactive ? 3 : this->super.active ? 1 : this->select ? 2 : 0);
	e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);
	engineGraphicEngineDrawIndex(index * 3, 18 * 3);

	engineMathMat4Translate(&tempmat, 0, this->super.active ? 1 : -1, 0);
	engineGraphicEngineSetMatrix(&tempmat);
	// 文字列描画
	double bright = isInactive ? 0.5 : 0.0;
	e3dTransBindColor(&this->trans, bright, bright, bright, alpha);
	e3dImageDraw(&this->imgText);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static void partsButtonPickerDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct partsButtonPicker *this = (struct partsButtonPicker*)((char*)that - offsetof(struct partsButtonPicker, trans));
	partsButtonPickerDraw(this, mat, a);
}

// ボタン初期化
void partsButtonPickerInit(struct partsButtonPicker *this, int picked){
	partsButtonInit((struct partsButton*)this);
	this->select = false;
	this->picked = picked;
}

// 配列にボタンの描画情報を追加
void partsButtonPickerCreateArray(struct partsButtonPicker *this, int x, int y, int w, char **textList, int textListLength, double scale){
	e3dTransInit(&this->trans);
	this->trans.draw = partsButtonPickerDraw_trans;

	partsButtonPosition((struct partsButton*)(this), x, y, w, 30);
	int x0 = -(int)(this->super.w * 0.5);
	int y0 = -(int)(this->super.h * 0.5);

	this->faceIndex = engineGraphicBufferFaceIndexGet();

	e3dImageBoxCreateArray(NULL, x0 + 28 - 10, y0 + 3, this->super.w - (28 - 10), this->super.h - 6, TEXPOS_SYSTEM_BUTTONBASICNORMAL_XYWH);
	e3dImageBoxCreateArray(NULL, x0 + 28 - 10, y0 + 3, this->super.w - (28 - 10), this->super.h - 6, TEXPOS_SYSTEM_BUTTONBASICACTIVE_XYWH);
	e3dImageBoxCreateArray(NULL, x0 + 28 - 10, y0 + 3, this->super.w - (28 - 10), this->super.h - 6, TEXPOS_SYSTEM_BUTTONBASICSELECT_XYWH);
	e3dImageBoxCreateArray(NULL, x0 + 28 - 10, y0 + 3, this->super.w - (28 - 10), this->super.h - 6, TEXPOS_SYSTEM_BUTTONBASICINACTIVE_XYWH);
	e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x0, y0, 28, 30, TEXPOS_SYSTEM_BUTTONPICKERNORMAL_XYWH);
	e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x0, y0, 28, 30, TEXPOS_SYSTEM_BUTTONPICKERACTIVE_XYWH);
	e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x0, y0, 28, 30, TEXPOS_SYSTEM_BUTTONPICKERSELECT_XYWH);
	e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, x0, y0, 28, 30, TEXPOS_SYSTEM_BUTTONPICKERINACTIVE_XYWH);

	for(int i = 0; i < textListLength; i++){e3dImageTextCreateArray(&this->imgText[i], x0 + 28 + 5, y0 + this->super.h * 0.5, textList[i], scale, 1, 0);}
}

// ボタン描画
void partsButtonPickerDraw(struct partsButtonPicker *this, struct engineMathMatrix44 *mat, double alpha){
	e3dTransBindBuffer(&this->trans);

	struct engineMathMatrix44 tempmat;
	engineMathMat4Copy(&tempmat, mat);
	e3dTransMultMatrix(&this->trans, &tempmat);
	engineMathMat4Translate(&tempmat, this->super.x, this->super.y, 0);
	partsButtonSetMatrix(&this->super, &tempmat);

	engineGraphicEngineSetMatrix(&tempmat);
	// ボタン枠描画
	bool isInactive = (this->super.inactive && !this->super.active && !this->select);
	int type = (isInactive ? 3 : this->super.active ? 1 : this->select ? 2 : 0);
	e3dTransBindColor(&this->trans, 1.0, 1.0, 1.0, alpha);
	engineGraphicEngineDrawIndex((this->faceIndex + 18 * type +  2) * 3, 4 * 3);
	engineGraphicEngineDrawIndex((this->faceIndex + 18 * type +  8) * 3, 4 * 3);
	engineGraphicEngineDrawIndex((this->faceIndex + 18 * type + 14) * 3, 4 * 3);
	engineGraphicEngineDrawIndex((this->faceIndex + 18 * 4 + 2 * type) * 3, 2 * 3);

	engineMathMat4Translate(&tempmat, 0, this->super.active ? 1 : -1, 0);
	engineGraphicEngineSetMatrix(&tempmat);
	// 文字列描画
	double bright = isInactive ? 0.5 : 0.0;
	e3dTransBindColor(&this->trans, bright, bright, bright, alpha);
	e3dImageDraw(&this->imgText[this->picked]);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static void partsButtonHexDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct partsButtonHex *this = (struct partsButtonHex*)((char*)that - offsetof(struct partsButtonHex, trans));
	partsButtonHexDraw(this, mat, a);
}

// ボタン初期化
void partsButtonHexInit(struct partsButtonHex *this){
	partsButtonInit((struct partsButton*)this);
	this->select = false;
}

// 配列にボタンの描画情報を追加
void partsButtonHexCreateArray(struct partsButtonHex *this, int x, int y, int r, int tu, int tv, int tw, int th){
	e3dTransInit(&this->trans);
	this->trans.draw = partsButtonHexDraw_trans;

	this->super.x = x;
	this->super.y = y;

	int vertIndex = engineGraphicBufferVretIndexGet();
	this->faceIndex = engineGraphicBufferFaceIndexGet();

	engineGraphicBufferPushVert((r - 3) * CONST_COS000, (r - 3) * CONST_SIN000, 0);
	engineGraphicBufferPushVert((r - 3) * CONST_COS060, (r - 3) * CONST_SIN060, 0);
	engineGraphicBufferPushVert((r - 3) * CONST_COS120, (r - 3) * CONST_SIN120, 0);
	engineGraphicBufferPushVert((r - 3) * CONST_COS180, (r - 3) * CONST_SIN180, 0);
	engineGraphicBufferPushVert((r - 3) * CONST_COS240, (r - 3) * CONST_SIN240, 0);
	engineGraphicBufferPushVert((r - 3) * CONST_COS300, (r - 3) * CONST_SIN300, 0);
	engineGraphicBufferPushVert(r * CONST_COS000, r * CONST_SIN000, 0);
	engineGraphicBufferPushVert(r * CONST_COS060, r * CONST_SIN060, 0);
	engineGraphicBufferPushVert(r * CONST_COS120, r * CONST_SIN120, 0);
	engineGraphicBufferPushVert(r * CONST_COS180, r * CONST_SIN180, 0);
	engineGraphicBufferPushVert(r * CONST_COS240, r * CONST_SIN240, 0);
	engineGraphicBufferPushVert(r * CONST_COS300, r * CONST_SIN300, 0);
	for(int i = 0; i < 12; i++){engineGraphicBufferPushTexcWhite();}
	engineGraphicBufferPushFace(vertIndex, 0, 1, 5);
	engineGraphicBufferPushFace(vertIndex, 1, 2, 5);
	engineGraphicBufferPushFace(vertIndex, 2, 4, 5);
	engineGraphicBufferPushFace(vertIndex, 2, 3, 4);
	for(int i = 0; i < 6; i++){
		engineGraphicBufferPushFace(vertIndex, 0 + (i + 0) % 6, 0 + (i + 1) % 6, 6 + (i + 1) % 6);
		engineGraphicBufferPushFace(vertIndex, 0 + (i + 0) % 6, 6 + (i + 1) % 6, 6 + (i + 0) % 6);
	}

	if(tw > 0 && th > 0){
		// アイコン
		this->isIcon = true;
		e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, tw * -0.5, th * -0.5, tw, th, tu, tv, tw, th);
	}

	// 六角形のタッチ範囲
	this->super.hitVertexLength = 6;
	this->super.hitVertex0[0].x = r * CONST_COS000; this->super.hitVertex0[0].y = r * CONST_SIN000; this->super.hitVertex0[0].z = 0;
	this->super.hitVertex0[1].x = r * CONST_COS060; this->super.hitVertex0[1].y = r * CONST_SIN060; this->super.hitVertex0[1].z = 0;
	this->super.hitVertex0[2].x = r * CONST_COS120; this->super.hitVertex0[2].y = r * CONST_SIN120; this->super.hitVertex0[2].z = 0;
	this->super.hitVertex0[3].x = r * CONST_COS180; this->super.hitVertex0[3].y = r * CONST_SIN180; this->super.hitVertex0[3].z = 0;
	this->super.hitVertex0[4].x = r * CONST_COS240; this->super.hitVertex0[4].y = r * CONST_SIN240; this->super.hitVertex0[4].z = 0;
	this->super.hitVertex0[5].x = r * CONST_COS300; this->super.hitVertex0[5].y = r * CONST_SIN300; this->super.hitVertex0[5].z = 0;
}

// ボタン描画
void partsButtonHexDraw(struct partsButtonHex *this, struct engineMathMatrix44 *mat, double alpha){
	e3dTransBindBuffer(&this->trans);

	struct engineMathMatrix44 tempmat1;
	struct engineMathMatrix44 tempmat2;
	engineMathMat4Copy(&tempmat2, mat);
	e3dTransMultMatrix(&this->trans, &tempmat2);
	engineMathMat4Translate(&tempmat2, this->super.x, this->super.y, 0);
	engineMathMat4RotateZ(&tempmat2, this->rotInner);
	engineMathMat4Copy(&tempmat1, &tempmat2);
	engineMathMat4RotateZ(&tempmat1, this->rotOuter - this->rotInner);
	partsButtonSetMatrix(&this->super, &tempmat1);
	if(this->super.active){
		engineMathMat4Scale(&tempmat1, 0.95, 0.95, 1.0);
		engineMathMat4Scale(&tempmat2, 0.95, 0.95, 1.0);
	}

	// ボタン枠描画
	bool isInactive = (this->super.inactive && !this->super.active && !this->select);
	double gray1 = (isInactive ? 0.4 : this->super.active ? 0.6 : this->select ? 1.0 : 0.8);
	double gray2 = (isInactive ? 0.4 : this->super.active ? 0.6 : this->select ? 1.0 : 1.0);
	engineGraphicEngineSetMatrix(&tempmat1);
	e3dTransBindColor(&this->trans, 0.0, 0.0, 0.0, alpha * 0.8);
	engineGraphicEngineDrawIndex((this->faceIndex +  4) * 3, 12 * 3);
	e3dTransBindColor(&this->trans, gray1, gray1, gray1, alpha * 0.8);
	engineGraphicEngineDrawIndex((this->faceIndex +  0) * 3,  4 * 3);
	if(this->isIcon){
		// アイコン描画
		engineGraphicEngineSetMatrix(&tempmat2);
		e3dTransBindColor(&this->trans, gray2, gray2, gray2, alpha);
		engineGraphicEngineDrawIndex((this->faceIndex + 16) * 3,  2 * 3);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

