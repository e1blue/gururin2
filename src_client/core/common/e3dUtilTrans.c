#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 変形構造体初期化
void e3dTransInit(struct e3dTrans *this){
	this->texture = NULL;
	this->vertVBO = NULL;
	this->clorVBO = NULL;
	this->texcVBO = NULL;
	this->faceIBO = NULL;
	this->color[0] = 1.0;
	this->color[1] = 1.0;
	this->color[2] = 1.0;
	this->color[3] = 1.0;
	this->parent = NULL;
	this->isScale = false;
	this->isRotate = false;
	this->isTranslate = false;
	this->isExist = true;
	this->isVisible = true;
	this->zIndex = 0;
	this->draw = NULL;
}

void e3dTransSetProperty(struct e3dTrans *this, struct e3dTrans *parent, int zIndex){
	this->parent = parent;
	this->zIndex = zIndex;
}

void e3dTransSetBuffer(struct e3dTrans *this, int *texture, int *vertVBO, int *clorVBO, int *texcVBO, int *faceIBO){
	this->texture = texture;
	this->vertVBO = vertVBO;
	this->clorVBO = clorVBO;
	this->texcVBO = texcVBO;
	this->faceIBO = faceIBO;
}

void e3dTransSetScale(struct e3dTrans *this, double x, double y, double z){
	this->isScale = !(x == 1.0 && y == 1.0 && z == 1.0);
	if(this->isScale){
		this->scale[0] = x;
		this->scale[1] = y;
		this->scale[2] = z;
	}
}

void e3dTransSetRotate(struct e3dTrans *this, double x, double y, double z){
	this->isRotate = !(x == 0.0 && y == 0.0 && z == 0.0);
	if(this->isRotate){
		engineMathMat4Identity(&this->rotate);
		if(y != 0.0){engineMathMat4RotateY(&this->rotate, y);}
		if(x != 0.0){engineMathMat4RotateX(&this->rotate, x);}
		if(z != 0.0){engineMathMat4RotateZ(&this->rotate, z);}
	}
}

void e3dTransSetTranslate(struct e3dTrans *this, double x, double y, double z){
	this->isTranslate = !(x == 0.0 && y == 0.0 && z == 0.0);
	if(this->isTranslate){
		this->translate[0] = x;
		this->translate[1] = y;
		this->translate[2] = z;
	}
}

void e3dTransSetColor(struct e3dTrans *this, double r, double g, double b, double a){
	this->color[0] = r;
	this->color[1] = g;
	this->color[2] = b;
	this->color[3] = a;
}

// ----------------------------------------------------------------

// 変形行列計算
void e3dTransMultMatrix(struct e3dTrans *this, struct engineMathMatrix44 *mat){
	if(this->parent != NULL){e3dTransMultMatrix(this->parent, mat);}

	// 行列合成
	if(this->isTranslate){engineMathMat4Translate(mat, this->translate[0], this->translate[1], this->translate[2]);}
	if(this->isRotate){engineMathMat4Multiply(mat, mat, &this->rotate);}
	if(this->isScale){engineMathMat4Scale(mat, this->scale[0], this->scale[1], this->scale[2]);}
}

// ----------------------------------------------------------------

// 変形構造体のバッファ登録
void e3dTransBindBuffer(struct e3dTrans *this){
	struct e3dTrans *trans;
	trans = this; while(trans != NULL){if(trans->texture != NULL){engineGraphicEngineBindTexture(*trans->texture); break;} trans = trans->parent;}
	trans = this; while(trans != NULL){if(trans->vertVBO != NULL){engineGraphicEngineBindVertVBO(*trans->vertVBO); break;} trans = trans->parent;}
	trans = this; while(trans != NULL){if(trans->clorVBO != NULL){engineGraphicEngineBindClorVBO(*trans->clorVBO); break;} trans = trans->parent;}
	trans = this; while(trans != NULL){if(trans->texcVBO != NULL){engineGraphicEngineBindTexcVBO(*trans->texcVBO); break;} trans = trans->parent;}
	trans = this; while(trans != NULL){if(trans->faceIBO != NULL){engineGraphicEngineBindFaceIBO(*trans->faceIBO); break;} trans = trans->parent;}
}

// 変形構造体の色情報登録
void e3dTransBindColor(struct e3dTrans *this, double r, double g, double b, double a){
	double color[4] = {r, g, b, a};
	struct e3dTrans *trans = this;
	while(trans != NULL){
		color[0] *= trans->color[0];
		color[1] *= trans->color[1];
		color[2] *= trans->color[2];
		color[3] *= trans->color[3];
		trans = trans->parent;
	}
	engineGraphicEngineSetColorRgba(color[0], color[1], color[2], color[3]);
}

// 変形構造体登録
void e3dTransBind(struct e3dTrans *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	// バッファ登録
	e3dTransBindBuffer(this);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(this, &tempmat1);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(this, r, g, b, a);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 変形管理構造体のリセット
void e3dTransListManagerReset(struct e3dTransListManager *this){
	for(int i = 0; i < this->transLength; i++){
		struct e3dTrans *trans = this->transList[i];
		if(trans == NULL){continue;}
		trans->isExist = false;
	}
}

// 変形管理構造体に要素追加
void e3dTransListManagerPush(struct e3dTransListManager *this, struct e3dTrans *trans){
	// 同じ要素がすでにあるか確認
	for(int i = 0; i < this->transLength; i++){
		if(this->transList[i] == trans){return;}
	}
	// データの挿入先を探す
	int index = -1;
	// リストの空きを探す
	for(int i = 0; i < this->transLength; i++){
		struct e3dTrans *temp = this->transList[i];
		if(temp == NULL || !temp->isExist){index = i; break;}
	}
	if(index < 0){
		// リストに空きがなかったらリスト拡張
		index = this->transLength;
		int addNum = 10;
		int length = index + addNum;
		void *list = engineUtilMemoryCalloc(length, sizeof(*this->transList));
		if(this->transLength > 0){
			memcpy(list, this->transList, this->transLength * sizeof(*this->transList));
			engineUtilMemoryFree(this->transList);
		}
		this->transLength = length;
		this->transList = list;
	}
	// データ挿入
	this->transList[index] = trans;
}

// 並べ替え関数
static int e3dTransList_sort(const void *a, const void *b){
	struct e3dTrans *trans1 = *(struct e3dTrans**)a;
	struct e3dTrans *trans2 = *(struct e3dTrans**)b;
	double zIndex1 = (trans1 != NULL) ? trans1->zIndex : 0;
	double zIndex2 = (trans2 != NULL) ? trans2->zIndex : 0;
	return (zIndex1 - zIndex2);
}

// 変形管理構造体の描画
void e3dTransListManagerDraw(struct e3dTransListManager *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	// 並べ替え
	qsort((void*)this->transList, this->transLength, sizeof(struct e3dTrans*), e3dTransList_sort);
	// 描画
	for(int i = 0; i < this->transLength; i++){
		struct e3dTrans *trans = this->transList[i];
		if(trans == NULL || trans->draw == NULL){continue;}
		while(trans != NULL && trans->isExist && trans->isVisible){trans = trans->parent;}
		if(trans == NULL){
			trans = this->transList[i];
			trans->draw(trans, mat, r, g, b, a);
		}
	}
}

// 変形管理構造体の破棄
void e3dTransListManagerDispose(struct e3dTransListManager *this){
	engineUtilMemoryFree(this->transList);
}

// 変形管理構造体に要素追加して設定
void e3dTransListManagerPushProperty(struct e3dTransListManager *this, struct e3dTrans *trans, struct e3dTrans *parent, int zIndex){
	e3dTransListManagerPush(this, trans);
	e3dTransSetProperty(trans, parent, zIndex);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 変形描画構造体初期化
void e3dTransImageInit(struct e3dTransImage *this){
	e3dTransInit(&this->super);
	this->super.draw = (void(*)(struct e3dTrans*, struct engineMathMatrix44*, double, double, double, double))e3dTransImageDraw;
}

// 変形描画構造体描画
void e3dTransImageDraw(struct e3dTransImage *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	e3dTransBind(&this->super, mat, r, g, b, a);
	e3dImageDraw(&this->image);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

