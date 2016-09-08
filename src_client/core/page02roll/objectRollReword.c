#include "../library.h"
#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
void objectRollRewordInit(struct objectRollReword *this, struct pageRollStatus *stat){
	this->stat = stat;

	this->itemLength = 0;
	this->itemLength += this->stat->getCoin;
	this->itemLength += this->stat->getStar;
	this->itemLength += this->stat->getPlayerStar;
	this->itemLength += this->stat->getEnemyStar;
	this->itemLength += this->stat->getHealth;
	this->itemList = engineUtilMemoryCalloc(this->itemLength, sizeof(*this->itemList));
	int index = 0;

	// ゴールスター
	for(int i = 0; i < this->stat->getStar; i++){
		struct objectRollRewordItem *item = &this->itemList[index++];
		item->type = 0;
		item->rad = 13;
		item->mass = 4;
	}

	// コイン
	for(int i = 0; i < this->stat->getCoin; i++){
		struct objectRollRewordItem *item = &this->itemList[index++];
		item->type = 1;
		item->rad = 8;
		item->mass = 3;
	}

	// プレイヤーの星
	for(int i = 0; i < this->stat->getPlayerStar; i++){
		struct objectRollRewordItem *item = &this->itemList[index++];
		item->type = 2;
		item->rad = 6;
		item->mass = 2;
	}

	// 敵の星
	for(int i = 0; i < this->stat->getEnemyStar; i++){
		struct objectRollRewordItem *item = &this->itemList[index++];
		item->type = 3;
		item->rad = 6;
		item->mass = 2;
	}

	// 体力
	for(int i = 0; i < this->stat->getHealth; i++){
		struct objectRollRewordItem *item = &this->itemList[index++];
		item->type = 4;
		item->rad = 9;
		item->mass = 3;
	}

	// 位置順番を設定
	int num = 10;
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item = &this->itemList[i];
		item->step = -(90 + 3 * i);
		item->x = 70 * ((2 * (double)(i % num) / (num - 1)) - 1);
		item->y = 320;
	}
	// 位置順番をランダムに入れ替え
	for(int i = 0; i < this->itemLength * 3; i++){
		struct objectRollRewordItem *item1 = &this->itemList[utilRandomGet() % this->itemLength];
		struct objectRollRewordItem *item2 = &this->itemList[utilRandomGet() % this->itemLength];
		int step = item1->step;
		double x = item1->x;
		double y = item1->y;
		item1->step = item2->step;
		item1->x = item2->x;
		item1->y = item2->y;
		item2->step = step;
		item2->x = x;
		item2->y = y;
	}
}

// 計算
void objectRollRewordCalc(struct objectRollReword *this){
	double e = 0.5;
	//double u = 0.5;
	double k = 80.0;
	double dt = 0.1;

	// 要素単体計算 リセット
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item = &this->itemList[i];
		item->step++;
		item->ax = 0;
		item->ay = 0;
	}

	// 要素相互作用計算 力の計算
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item0 = &this->itemList[i];
		if(item0->step < 0){continue;}
		for(int j = i + 1; j < this->itemLength; j++){
			struct objectRollRewordItem *item1 = &this->itemList[j];
			if(item1->step < 0){continue;}
			double x = item1->x - item0->x;
			double y = item1->y - item0->y;
			double rr = x * x + y * y;
			double r0 = item1->rad + item0->rad;
			if(rr <= 0 || r0 * r0 <= rr){continue;}
			double r = utilMathSqrt(rr);
			double ir = 1 / r;
			double nx = x * ir;
			double ny = y * ir;
			//double px = ny;
			//double py = -nx;
			// 衝突の力
			double nv = (item1->vx - item0->vx) * nx + (item1->vy - item0->vy) * ny;
			double fi = ((1 + e) * item0->mass * item1->mass / (item0->mass + item1->mass)) * nv;
			double fix = fi * nx;
			double fiy = fi * ny;
			// めり込みの力
			double fk = k * (r - r0);
			double fkx = fk * nx;
			double fky = fk * ny;
			// 力を加速度に
			item0->ax += (fix + fkx) / item0->mass;
			item0->ay += (fiy + fky) / item0->mass;
			item1->ax -= (fix + fkx) / item1->mass;
			item1->ay -= (fiy + fky) / item1->mass;
		}
	}

	// 要素単体計算 力の計算
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item = &this->itemList[i];
		if(item->step < 0){continue;}

		// 壁衝突
		double wmax = 90;
		double wx = 0;
		if(item->x < -(wmax - item->rad)){wx = -(wmax - item->rad);}
		if(item->x >  (wmax - item->rad)){wx =  (wmax - item->rad);}
		if(engineMathAbs(wx) >= DBL_EPSILON){
			double fix = (1 + e) * item->mass * (0 - item->vx);
			double fkx = k * (wx - item->x);
			item->ax += (fix + fkx) / item->mass;
		}
		// 床衝突
		if(item->y < item->rad){
			double fiy = (1 + e) * item->mass * (0 - item->vy);
			double fky = k * (item->rad - item->y);
			item->ay += (fiy + fky) / item->mass;
		}

		// 重力加速度
		double g = 9.8;
		item->ay += -g;
	}

	// 要素単体計算 数値微分オイラー法
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item = &this->itemList[i];
		if(item->step < 0){continue;}

		item->vx += item->ax * dt;
		item->vy += item->ay * dt;

		// 最大速度をオーバーしない
		double vmax = 100;
		double vrr = item->vx * item->vx + item->vy * item->vy;
		if(vrr > vmax * vmax){
			double ir = vmax / utilMathSqrt(vrr);
			item->vx *= ir;
			item->vy *= ir;
		}

		item->x += item->vx * dt;
		item->y += item->vy * dt;
	}
}

// バッファ作成
void objectRollRewordCreateArray(struct objectRollReword *this){
	// 地面作成
	int faceIndex = engineGraphicBufferFaceIndexGet();
	int faceNum = 0;
	e3dImageWhiteCreateArray(NULL, -90,        -1, 180,   2); faceNum += 2;
	//e3dImageWhiteCreateArray(NULL,  -1,       -15,   2,  30); faceNum += 2;
	//e3dImageWhiteCreateArray(NULL,  -1 - 90, -200,   2, 200); faceNum += 2;
	//e3dImageWhiteCreateArray(NULL,  -1 + 90, -200,   2, 200); faceNum += 2;
	this->imgGround.faceIndex = faceIndex;
	this->imgGround.faceNum = faceNum;
	// スター作成
	e3dImageCreateArray(&this->imgStar[0], TEXSIZ_SYSTEM_WH, -18, -18, 36, 36, TEXPOS_SYSTEM_ITEMSTAR1_XYWH);
	e3dImageCreateArray(&this->imgStar[1], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_ITEMCOIN1_XYWH);
	e3dImageCreateArray(&this->imgStar[2], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR21_XYWH);
	e3dImageCreateArray(&this->imgStar[3], TEXSIZ_SYSTEM_WH, -16, -16, 32, 32, TEXPOS_SYSTEM_EFFECTSTAR31_XYWH);
	e3dImageCreateArray(&this->imgStar[4], TEXSIZ_SYSTEM_WH, -10, -10, 20, 20, TEXPOS_SYSTEM_PARAMHEART_XYWH);
}

// 描画
void objectRollRewordDraw(struct objectRollReword *this, struct engineMathMatrix44 *mat, double alpha){
	struct engineMathMatrix44 tempmat1;

	// スター描画
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	for(int i = 0; i < this->itemLength; i++){
		struct objectRollRewordItem *item = &this->itemList[i];
		if(item->step < 0){continue;}
		engineMathMat4Copy(&tempmat1, mat);
		engineMathMat4Translate(&tempmat1, item->x, -item->y, 0.0);
		engineGraphicEngineSetMatrix(&tempmat1);
		e3dImageDraw(&this->imgStar[item->type]);
	}

	// 地面描画
	engineMathMat4Copy(&tempmat1, mat);
	engineGraphicEngineSetMatrix(&tempmat1);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, alpha);
	e3dImageDraw(&this->imgGround);
}

// 破棄
void objectRollRewordDispose(struct objectRollReword *this){
	engineUtilMemoryFree(this->itemList);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

