#include "../library.h"
#include "../includeAll.h"
#include "pageStory.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化すなぼこり
static void dustInit(struct objectStoryEffectDust *this, double x, double y, double z){
	this->x = x;
	this->y = y;
	this->z = z;
	this->step = 0;
}

// 計算すなぼこり
static void dustCalc(struct objectStoryEffectDust *this){
	if(!this->trans.isExist){return;}
	this->trans.isExist = (++this->step < 24);
}

// 描画すなぼこり
static void dustDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct objectStoryEffectDust *this = (struct objectStoryEffectDust*)((char*)that - offsetof(struct objectStoryEffectDust, trans));
	// バッファ登録
	e3dTransBindBuffer(&this->trans);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(&this->trans, &tempmat1);
	engineMathMat4Translate(&tempmat1, this->x, this->z, this->y);
	engineMathMat4RotateZ(&tempmat1, -this->manager->stat->rotg);
	engineMathMat4RotateY(&tempmat1, -this->manager->stat->rotv);
	engineMathMat4RotateX(&tempmat1, -this->manager->stat->roth);
	engineMathMat4Scale(&tempmat1, 0.03, -0.03, 1.0);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(&this->trans, r, g, b, a);

	e3dImageDraw(&this->manager->imgDust[(int)(this->step / 6)]);
}

// 演出登録すなぼこり
void objectStoryEffectManagerPushDust(struct objectStoryEffectManager *this, double x, double y, double z){
	struct objectStoryEffectDust *effect = NULL;
	// 空いている構造体を取得
	for(int i = 0; i < (int)(sizeof(this->dustList) / sizeof(*this->dustList)); i++){
		if(!this->dustList[i].trans.isExist){effect = &this->dustList[i]; break;}
	}
	if(effect == NULL){return;}
	// 構造体初期化
	e3dTransInit(&effect->trans);
	effect->trans.draw = dustDraw_trans;
	effect->manager = this;
	// 構造体設定
	dustInit(effect, x, y, z);

	e3dTransListManagerPush(this->transManager, &effect->trans);
}

// ----------------------------------------------------------------

// 初期化星
static void starInit(struct objectStoryEffectStar *this, int type, double x, double y, double z){
	this->type = type;
	this->x = x;
	this->y = y;
	this->z = z;
	this->step = 0;
	double c = utilMathCos(this->manager->stat->rotg);
	double s = utilMathSin(this->manager->stat->rotg);
	// 水平方向の速度
	double radius = 0.002 * (utilRandomGet() % 30);
	double theta = CONST_DEG2RAD * (utilRandomGet() % 360);
	double vx = radius * utilMathCos(theta);
	double vy = radius * utilMathSin(theta);
	this->vx = vx *  c;
	this->vy = vy;
	this->vz = vx * -s;
	// 垂直方向の速度
	this->vx += 0.18 * s;
	this->vz += 0.18 * c;
}

// 計算星
static void starCalc(struct objectStoryEffectStar *this){
	if(!this->trans.isExist){return;}
	this->vx += -0.01 * utilMathSin(this->manager->stat->rotg);
	this->vz += -0.01 * utilMathCos(this->manager->stat->rotg);
	this->x += this->vx;
	this->y += this->vy;
	this->z += this->vz;
	this->trans.isExist = (++this->step < 36);
}

// 描画星
static void starDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct objectStoryEffectStar *this = (struct objectStoryEffectStar*)((char*)that - offsetof(struct objectStoryEffectStar, trans));
	// バッファ登録
	e3dTransBindBuffer(&this->trans);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(&this->trans, &tempmat1);
	engineMathMat4Translate(&tempmat1, this->x, this->z, this->y);
	engineMathMat4RotateZ(&tempmat1, -this->manager->stat->rotg);
	engineMathMat4RotateY(&tempmat1, -this->manager->stat->rotv);
	engineMathMat4RotateX(&tempmat1, -this->manager->stat->roth);
	engineMathMat4Scale(&tempmat1, 0.03, -0.03, 1.0);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(&this->trans, r, g, b, a);

	e3dImageDraw(&this->manager->imgStar[this->type][(int)(this->step / 8) % 2]);
}

// 演出登録星
void objectStoryEffectManagerPushStar(struct objectStoryEffectManager *this, int type, double x, double y, double z){
	struct objectStoryEffectStar *effect = NULL;
	// 空いている構造体を取得
	for(int i = 0; i < (int)(sizeof(this->starList) / sizeof(*this->starList)); i++){
		if(!this->starList[i].trans.isExist){effect = &this->starList[i]; break;}
	}
	if(effect == NULL){return;}
	// 構造体初期化
	e3dTransInit(&effect->trans);
	effect->trans.draw = starDraw_trans;
	effect->manager = this;
	// 構造体設定
	starInit(effect, type, x, y, z);

	e3dTransListManagerPush(this->transManager, &effect->trans);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
void objectStoryEffectManagerInit(struct objectStoryEffectManager *this, struct pageStoryStatus *stat, struct e3dTransListManager *transManager){
	this->stat = stat;
	this->transManager = transManager;
}

// 計算
void objectStoryEffectManagerCalc(struct objectStoryEffectManager *this){
	for(int i = 0; i < (int)(sizeof(this->dustList) / sizeof(*this->dustList)); i++){dustCalc(&this->dustList[i]);}
	for(int i = 0; i < (int)(sizeof(this->starList) / sizeof(*this->starList)); i++){starCalc(&this->starList[i]);}
}

// バッファ作成
void objectStoryEffectManagerCreateArray(struct objectStoryEffectManager *this){
	e3dImageCreateArray(&this->imgDust[0], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSMORK1_XYWH);
	e3dImageCreateArray(&this->imgDust[1], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSMORK2_XYWH);
	e3dImageCreateArray(&this->imgDust[2], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSMORK3_XYWH);
	e3dImageCreateArray(&this->imgDust[3], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSMORK4_XYWH);
	e3dImageCreateArray(&this->imgStar[0][0], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR11_XYWH);
	e3dImageCreateArray(&this->imgStar[0][1], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR12_XYWH);
	e3dImageCreateArray(&this->imgStar[1][0], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR21_XYWH);
	e3dImageCreateArray(&this->imgStar[1][1], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR22_XYWH);
	e3dImageCreateArray(&this->imgStar[2][0], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR31_XYWH);
	e3dImageCreateArray(&this->imgStar[2][1], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR32_XYWH);
	e3dImageCreateArray(&this->imgStar[3][0], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR41_XYWH);
	e3dImageCreateArray(&this->imgStar[3][1], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR42_XYWH);
}

// エフェクトリセット
void objectStoryEffectManagerReset(struct objectStoryEffectManager *this){
	for(int i = 0; i < (int)(sizeof(this->dustList) / sizeof(*this->dustList)); i++){this->dustList[i].trans.isExist = false;}
	for(int i = 0; i < (int)(sizeof(this->starList) / sizeof(*this->starList)); i++){this->starList[i].trans.isExist = false;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

