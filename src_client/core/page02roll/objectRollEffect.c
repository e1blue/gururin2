#include "../library.h"
#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化すなぼこり
static void dustInit(struct objectRollEffectDust *this, double x, double y){
	this->x = x;
	this->y = y;
	this->step = 0;
}

// 計算すなぼこり
static void dustCalc(struct objectRollEffectDust *this){
	if(!this->trans.isExist){return;}
	this->trans.isExist = (++this->step < 24);
}

// 描画すなぼこり
static void dustDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct objectRollEffectDust *this = (struct objectRollEffectDust*)((char*)that - offsetof(struct objectRollEffectDust, trans));
	// バッファ登録
	e3dTransBindBuffer(&this->trans);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	e3dTransMultMatrix(&this->trans, &tempmat1);
	engineMathMat4Translate(&tempmat1, this->x, this->y, 0.0);
	engineMathMat4RotateZ(&tempmat1, -this->manager->stat->gravityRot);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(&this->trans, r, g, b, a);

	e3dImageDraw(&this->manager->imgDust[(int)(this->step / 6)]);
}

// 演出登録すなぼこり
void objectRollEffectManagerPushDust(struct objectRollEffectManager *this, double x, double y){
	struct objectRollEffectDust *effect = NULL;
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
	dustInit(effect, x, y);

	e3dTransListManagerPush(this->transManager, &effect->trans);
}

// ----------------------------------------------------------------

// 初期化星
static void starInit(struct objectRollEffectStar *this, int type, double x, double y){
	this->type = type;
	this->x = x;
	this->y = y;
	this->step = 0;
	// 水平方向の速度
	double radius = 1.0 * ((int)(utilRandomGet() % 201) - 100) * 0.01;
	this->vx = radius * this->manager->stat->gravityCos;
	this->vy = radius * this->manager->stat->gravitySin;
	// 垂直方向の速度
	this->vx += -3.0 * this->manager->stat->gravitySin;
	this->vy += -3.0 * this->manager->stat->gravityCos;
}

// 計算星
static void starCalc(struct objectRollEffectStar *this){
	if(!this->trans.isExist){return;}
	// 重力による加速計算
	this->vx += this->manager->stat->gravityPower * this->manager->stat->gravitySin;
	this->vy += this->manager->stat->gravityPower * this->manager->stat->gravityCos;
	// 位置計算
	this->x += this->vx;
	this->y += this->vy;
	// 消滅判定
	double x = (this->x - this->manager->stat->camerax) * this->manager->stat->cameraScale;
	double y = (this->y - this->manager->stat->cameray) * this->manager->stat->cameraScale;
	double s = (global.screen.w + global.screen.h) * 0.5 + 16;
	this->trans.isExist = (-s < x && x < s && -s < y && y < s);
}

// 描画星
static void starDraw_trans(struct e3dTrans *that, struct engineMathMatrix44 *mat, double r, double g, double b, double a){
	struct objectRollEffectStar *this = (struct objectRollEffectStar*)((char*)that - offsetof(struct objectRollEffectStar, trans));
	// バッファ登録
	e3dTransBindBuffer(&this->trans);
	// 行列登録
	struct engineMathMatrix44 tempmat1;
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4Translate(&tempmat1, this->x, this->y, 0.0);
	engineMathMat4RotateZ(&tempmat1, -this->manager->stat->gravityRot);
	engineGraphicEngineSetMatrix(&tempmat1);
	// 色登録
	e3dTransBindColor(&this->trans, r, g, b, a);

	e3dImageDraw(&this->manager->imgStar[this->type][(int)(this->step / 8) % 2]);
}

// 演出登録星
void objectRollEffectManagerPushStar(struct objectRollEffectManager *this, int type, double x, double y){
	for(int i = 0; i < 2; i++){
		struct objectRollEffectStar *effect = NULL;
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
		starInit(effect, type, x, y);

		e3dTransListManagerPush(this->transManager, &effect->trans);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
void objectRollEffectManagerInit(struct objectRollEffectManager *this, struct pageRollStatus *stat, struct e3dTransListManager *transManager){
	this->stat = stat;
	this->transManager = transManager;
}

// 計算
void objectRollEffectManagerCalc(struct objectRollEffectManager *this){
	for(int i = 0; i < (int)(sizeof(this->dustList) / sizeof(*this->dustList)); i++){dustCalc(&this->dustList[i]);}
	for(int i = 0; i < (int)(sizeof(this->starList) / sizeof(*this->starList)); i++){starCalc(&this->starList[i]);}
}

// バッファ作成
void objectRollEffectManagerCreateArray(struct objectRollEffectManager *this){
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
void objectRollEffectManagerReset(struct objectRollEffectManager *this){
	for(int i = 0; i < (int)(sizeof(this->dustList) / sizeof(*this->dustList)); i++){this->dustList[i].trans.isExist = false;}
	for(int i = 0; i < (int)(sizeof(this->starList) / sizeof(*this->starList)); i++){this->starList[i].trans.isExist = false;}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

