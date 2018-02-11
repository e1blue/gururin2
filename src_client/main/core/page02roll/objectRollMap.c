#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 内部変数を含むグルリンマップ構造体
struct objectRollMapImplement{
	struct objectRollMap super;

	int hexXsize;
	int hexYsize;
	int hexRadius;
	int wallRadius1;
	int wallRadius2;
	int trapRadius;
	struct objectRollMapHexInfo{
		int type;
		double x;
		double y;
	} *hexInfo;

	double e;
	double m;
};

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// マップチップの範囲外チェック
static bool isScope(struct objectRollMapImplement *this, int x, int y){
	if(x < 0 || this->hexXsize <= x){return false;}
	if(y < 0 || this->hexYsize <= y){return false;}
	return true;
}

// マップチップの壁確認
static bool isWall(struct objectRollMapHexInfo *this){
	return !!(this->type & 0x0001);
}

// マップチップの棘確認
static bool isTrapNeedle(struct objectRollMapHexInfo *this){
	return (this->type & ~(0x0001 | 0x0002)) ? false : !!(this->type & 0x0002);
}

// マップチップの加速器確認
static int isTrapAccelerator(struct objectRollMapHexInfo *this){
	if(this->type & ~0x001c){return -1;}
	int type = ((this->type & 0x001c) >> 2);
	return (type > 6 ? 6 : type) - 1;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
struct objectRollMap *objectRollMapInit(struct pageRollStatus *stat, int hexXsize, int hexYsize, int *hexChip){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)engineUtilMemoryCalloc(1, sizeof(struct objectRollMapImplement));
	this->super.stat = stat;
	this->hexXsize = hexXsize;
	this->hexYsize = hexYsize;
	this->hexRadius = 24;
	this->wallRadius1 = 5;
	this->wallRadius2 = 2;
	this->trapRadius = 16;
	this->e = 0.8;
	this->m = 0.1;

	int length = this->hexXsize * this->hexYsize;
	this->hexInfo = (struct objectRollMapHexInfo*)engineUtilMemoryCalloc(length, sizeof(struct objectRollMapHexInfo));
	for(int i = 0; i < length; i++){
		int hexx = i % this->hexXsize;
		int hexy = i / this->hexXsize;
		this->hexInfo[i].type = hexChip[i];
		this->hexInfo[i].x = objectRollMapCoordX(&this->super, hexx, hexy);
		this->hexInfo[i].y = objectRollMapCoordY(&this->super, hexx, hexy);
	}

	return (struct objectRollMap*)this;
}

// ----------------------------------------------------------------

// ヘックス座標からx座標獲得
double objectRollMapCoordX(struct objectRollMap *that, int hexx, int hexy){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	return (2 * hexx + hexy) * (this->hexRadius * CONST_SIN060);
}

// ヘックス座標からy座標獲得
double objectRollMapCoordY(struct objectRollMap *that, int hexx, int hexy){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	return 2 * hexy * (this->hexRadius * CONST_SIN060) * CONST_SIN060;
}

// ----------------------------------------------------------------

// 衝突データ
struct objectRollMapCollisionParameter{
	int num;
	double x;
	double y;
	double vx;
	double vy;
	bool isGround;
};

// 衝突判定
static void calcWall(struct objectRollMapImplement *this, struct objectRollMapCollisionParameter *param, struct objectRollMapChara *chara, struct objectRollMapHexInfo *info0, struct objectRollMapHexInfo *info1){
	// キャラクターと壁との距離
	double radius = chara->rad + this->wallRadius1;
	// 衝突点からキャラクタ中心までの垂線の単位ベクトル
	double nx = 0;
	double ny = 0;
	// 衝突辺と平行な単位ベクトル 正負はどちらでも良い
	double px = 0;
	double py = 0;
	// めり込み量
	double penetration = -1;

	if(info1 != NULL){
		// 壁辺の衝突判定
		double ax = info1->x - info0->x;
		double ay = info1->y - info0->y;
		double ar = utilMathSqrt(ax * ax + ay * ay);
		double iar = (engineMathAbs(ar) > 0) ? (1 / ar) : 0;
		double bx = chara->x - info0->x;
		double by = chara->y - info0->y;
		// r1: キャラクタ中心と壁直線との距離、r2: 線分範囲内確認
		double r1 = (ax * by - ay * bx) * iar;
		double r2 = (ax * bx + ay * by) * iar * iar;
		double br = utilMathRound(utilMathAbs(r1) * 100) * 0.01;
		if(engineMathAbs(br) > 0 && br <= radius && 0 < r2 && r2 < 1){
			px = ax * iar;
			py = ay * iar;
			nx = ((r1 > 0) ? -1 : 1) * py;
			ny = ((r1 > 0) ? 1 : -1) * px;
			penetration = radius - br;
		}
	}else{
		// 壁頂点の衝突判定
		double bx = chara->x - info0->x;
		double by = chara->y - info0->y;
		double brr = bx * bx + by * by;
		if(brr < radius * radius){
			double br = utilMathSqrt(brr);
			double ibr = (engineMathAbs(br) > 0) ? (1 / br) : 0;
			nx = bx * ibr;
			ny = by * ibr;
			px = ny;
			py = -nx;
			penetration = radius - br;
		}
	}

	if(penetration >= 0){
		// 衝突計算
		param->num++;
		param->x += nx * penetration;
		param->y += ny * penetration;
		double nv = nx * chara->vx + ny * chara->vy;
		if(nv < 0){
			double e = (-nv > chara->rad * 0.1) ? this->e : 0;
			param->vx -= nv * nx * (1 + e);
			param->vy -= nv * ny * (1 + e);
		}
		// 摩擦計算
		double stand = nx * chara->nx + ny * chara->ny;
		if(stand > 0.70710678118){
			double pv = px * chara->vx + py * chara->vy;
			param->vx -= pv * px * (this->m * stand);
			param->vy -= pv * py * (this->m * stand);
			param->isGround = true;
		}
		// 砂けむりエフェクト
		if(nv < -2){
			double x = chara->x - nx * (chara->rad - 10);
			double y = chara->y - ny * (chara->rad - 10);
			objectRollEffectManagerPushDust(&this->super.stat->effectManager, x, y);
		}
	}
}

// キャラクターが存在するヘックス確認
void objectRollMapCalcHex(struct objectRollMap *that, struct objectRollMapChara *chara){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	double radius = chara->rad + this->wallRadius1;
	chara->hexy = (int)utilMathRound((chara->y / ((this->hexRadius * CONST_SIN060) * CONST_SIN060)) * 0.5);
	chara->hexx = (int)utilMathRound((chara->x / (this->hexRadius * CONST_SIN060) - chara->hexy) * 0.5);
	chara->hexRange = (int)utilMathCeil(radius / (2 * (this->hexRadius * CONST_SIN060)));
}

// キャラと棘の衝突判定
bool objectRollMapCalcTrapNeedle(struct objectRollMap *that, struct objectRollMapChara *chara){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	bool isTrap = false;

	for(int i = chara->hexy - chara->hexRange; i <= chara->hexy + chara->hexRange; i++){
		for(int j = chara->hexx - chara->hexRange; j <= chara->hexx + chara->hexRange; j++){
			struct objectRollMapHexInfo *info = isScope(this, j, i) ? &this->hexInfo[this->hexXsize * i + j] : NULL;
			if(info == NULL){continue;}
			if(!isTrapNeedle(info)){continue;}
			double x = chara->x - info->x;
			double y = chara->y - info->y;
			double rr = x * x + y * y;
			double r0 = this->trapRadius + chara->rad;
			if(rr >= r0 * r0){continue;}
			double ir = (engineMathAbs(rr) > 0) ? (1 / utilMathSqrt(rr)) : 0;
			chara->vx += x * ir * chara->rad * 0.2;
			chara->vy += y * ir * chara->rad * 0.2;
			isTrap = true;
		}
	}

	return isTrap;
}

// キャラと加速器の衝突判定
bool objectRollMapCalcTrapAccelerator(struct objectRollMap *that, struct objectRollMapChara *chara){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	bool isTrap = false;

	for(int i = chara->hexy - chara->hexRange; i <= chara->hexy + chara->hexRange; i++){
		for(int j = chara->hexx - chara->hexRange; j <= chara->hexx + chara->hexRange; j++){
			struct objectRollMapHexInfo *info = isScope(this, j, i) ? &this->hexInfo[this->hexXsize * i + j] : NULL;
			if(info == NULL){continue;}
			int accRot = isTrapAccelerator(info);
			if(accRot < 0){continue;}
			double x = chara->x - info->x;
			double y = chara->y - info->y;
			double r0 = this->trapRadius + chara->rad;
			if(x * x + y * y >= r0 * r0){continue;}
			double c = 1.0;
			double s = 0.0;
			switch(accRot){
				case 0: c = CONST_COS300; s = CONST_SIN300; break;
				case 1: c = CONST_COS240; s = CONST_SIN240; break;
				case 2: c = CONST_COS180; s = CONST_SIN180; break;
				case 3: c = CONST_COS120; s = CONST_SIN120; break;
				case 4: c = CONST_COS060; s = CONST_SIN060; break;
				case 5: c = CONST_COS000; s = CONST_SIN000; break;
			}
			chara->vx += c * chara->rad * 0.5;
			chara->vy += s * chara->rad * 0.5;
			isTrap = true;
		}
	}

	return isTrap;
}

// キャラと壁の衝突判定
bool objectRollMapCalcCollision(struct objectRollMap *that, struct objectRollMapChara *chara){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;

	struct objectRollMapCollisionParameter param;
	param.num = 0;
	param.x = param.y = 0;
	param.vx = param.vy = 0;
	param.isGround = false;

	for(int i = chara->hexy - chara->hexRange; i <= chara->hexy + chara->hexRange; i++){
		for(int j = chara->hexx - chara->hexRange; j <= chara->hexx + chara->hexRange; j++){
			struct objectRollMapHexInfo *info0 = isScope(this, j + 0, i + 0) ? &this->hexInfo[this->hexXsize * (i + 0) + (j + 0)] : NULL;
			if(info0 == NULL || !isWall(info0)){continue;}
			struct objectRollMapHexInfo *info1 = isScope(this, j + 1, i + 0) ? &this->hexInfo[this->hexXsize * (i + 0) + (j + 1)] : NULL;
			struct objectRollMapHexInfo *info2 = isScope(this, j + 1, i - 1) ? &this->hexInfo[this->hexXsize * (i - 1) + (j + 1)] : NULL;
			//struct objectRollMapHexInfo *info3 = isScope(this, j + 0, i - 1) ? &this->hexInfo[this->hexXsize * (i - 1) + (j + 0)] : NULL;
			struct objectRollMapHexInfo *info4 = isScope(this, j - 1, i + 0) ? &this->hexInfo[this->hexXsize * (i + 0) + (j - 1)] : NULL;
			struct objectRollMapHexInfo *info5 = isScope(this, j - 1, i + 1) ? &this->hexInfo[this->hexXsize * (i + 1) + (j - 1)] : NULL;
			struct objectRollMapHexInfo *info6 = isScope(this, j + 0, i + 1) ? &this->hexInfo[this->hexXsize * (i + 1) + (j + 0)] : NULL;
			bool isBarrier1 = (info1 == NULL || isWall(info1));
			bool isBarrier2 = (info2 == NULL || isWall(info2));
			bool isBarrier4 = (info4 == NULL || isWall(info4));
			bool isBarrier5 = (info5 == NULL || isWall(info5));
			bool isBarrier6 = (info6 == NULL || isWall(info6));
			if((info1 != NULL && isWall(info1)) && !(isBarrier6 && isBarrier2)){calcWall(this, &param, chara, info0, info1);}
			if((info5 != NULL && isWall(info5)) && !(isBarrier4 && isBarrier6)){calcWall(this, &param, chara, info0, info5);}
			if((info6 != NULL && isWall(info6)) && !(isBarrier5 && isBarrier1)){calcWall(this, &param, chara, info0, info6);}
			// 壁頂点の衝突判定
			calcWall(this, &param, chara, info0, NULL);
		}
	}

	if(param.num > 0){
		// キャラクタの位置補正 壁辺にぶつからない位置
		chara->x += param.x / param.num;
		chara->y += param.y / param.num;
		// 衝突反射
		chara->vx += param.vx / param.num;
		chara->vy += param.vy / param.num;
	}

	return param.isGround;
}

// ----------------------------------------------------------------

static int createHex(struct objectRollMapImplement *this, int phase, int hexIndex, int vertIndex){
	int vertNum = 6;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;

		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS030, y0 + radius0 * CONST_SIN030, 0);
		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS090, y0 + radius0 * CONST_SIN090, 0);
		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS150, y0 + radius0 * CONST_SIN150, 0);
		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS210, y0 + radius0 * CONST_SIN210, 0);
		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS270, y0 + radius0 * CONST_SIN270, 0);
		engineGraphicBufferPushVert(x0 + radius0 * CONST_COS330, y0 + radius0 * CONST_SIN330, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 0, 1, 5);
			engineGraphicBufferPushFace(vertIndex, 1, 2, 5);
			engineGraphicBufferPushFace(vertIndex, 2, 4, 5);
			engineGraphicBufferPushFace(vertIndex, 2, 3, 4);
			break;
		case 1:
			// 壁面作成
			break;
		case 2:
			// 外面作成
			break;
	}
	return vertNum;
}

static int createTriangle1(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 3;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c1, y0 + radius0 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c2, y0 + radius0 * s2, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			break;
		case 1:
			// 壁面作成
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 1, 2);
			break;
	}
	return vertNum;
}

static int createTriangle2(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 7;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}
		double x1 = x0 + radius0 * c1;
		double y1 = y0 + radius0 * s1;
		double x2 = x0 + radius0 * c2;
		double y2 = y0 + radius0 * s2;

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x1, y1, 0);
		engineGraphicBufferPushVert(x2, y2, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c1, y0 + radius1 * 2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c1, y0 + radius2 * 2 * s1, 0);
		engineGraphicBufferPushVert(x1 + ((x2 - x1) / radius0) * (0.5 * radius0 - radius1), y1 + ((y2 - y1) / radius0) * (0.5 * radius0 - radius1), 0);
		engineGraphicBufferPushVert(x1 + ((x2 - x1) / radius0) * (0.5 * radius0 - radius2), y1 + ((y2 - y1) / radius0) * (0.5 * radius0 - radius2), 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 1, 3, 5);
			break;
		case 1:
			// 壁面作成
			engineGraphicBufferPushFace(vertIndex, 3, 4, 6);
			engineGraphicBufferPushFace(vertIndex, 3, 5, 6);
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 2, 6);
			engineGraphicBufferPushFace(vertIndex, 0, 4, 6);
			break;
	}
	return vertNum;
}

static int createTriangle3(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 7;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}
		double x1 = x0 + radius0 * c1;
		double y1 = y0 + radius0 * s1;
		double x2 = x0 + radius0 * c2;
		double y2 = y0 + radius0 * s2;

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x1, y1, 0);
		engineGraphicBufferPushVert(x2, y2, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c2, y0 + radius1 * 2 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c2, y0 + radius2 * 2 * s2, 0);
		engineGraphicBufferPushVert(x2 + ((x1 - x2) / radius0) * (0.5 * radius0 - radius1), y2 + ((y1 - y2) / radius0) * (0.5 * radius0 - radius1), 0);
		engineGraphicBufferPushVert(x2 + ((x1 - x2) / radius0) * (0.5 * radius0 - radius2), y2 + ((y1 - y2) / radius0) * (0.5 * radius0 - radius2), 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 2, 3, 5);
			break;
		case 1:
			// 壁面作成
			engineGraphicBufferPushFace(vertIndex, 3, 4, 6);
			engineGraphicBufferPushFace(vertIndex, 3, 5, 6);
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 1, 6);
			engineGraphicBufferPushFace(vertIndex, 0, 4, 6);
			break;
	}
	return vertNum;
}

static int createTriangle4(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 11;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}
		double x1 = x0 + radius0 * c1;
		double y1 = y0 + radius0 * s1;
		double x2 = x0 + radius0 * c2;
		double y2 = y0 + radius0 * s2;

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x1, y1, 0);
		engineGraphicBufferPushVert(x2, y2, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c1, y0 + radius1 * 2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c1, y0 + radius2 * 2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c2, y0 + radius1 * 2 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c2, y0 + radius2 * 2 * s2, 0);
		engineGraphicBufferPushVert(x1 + ((x2 - x1) / radius0) * (0.5 * radius0 - radius1), y1 + ((y2 - y1) / radius0) * (0.5 * radius0 - radius1), 0);
		engineGraphicBufferPushVert(x1 + ((x2 - x1) / radius0) * (0.5 * radius0 - radius2), y1 + ((y2 - y1) / radius0) * (0.5 * radius0 - radius2), 0);
		engineGraphicBufferPushVert(x2 + ((x1 - x2) / radius0) * (0.5 * radius0 - radius1), y2 + ((y1 - y2) / radius0) * (0.5 * radius0 - radius1), 0);
		engineGraphicBufferPushVert(x2 + ((x1 - x2) / radius0) * (0.5 * radius0 - radius2), y2 + ((y1 - y2) / radius0) * (0.5 * radius0 - radius2), 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 1, 3, 7);
			engineGraphicBufferPushFace(vertIndex, 2, 5, 9);
			break;
		case 1:
			// 壁面作成
			engineGraphicBufferPushFace(vertIndex, 3, 4, 8);
			engineGraphicBufferPushFace(vertIndex, 3, 7, 8);
			engineGraphicBufferPushFace(vertIndex, 5, 6, 10);
			engineGraphicBufferPushFace(vertIndex, 5, 9, 10);
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 4, 8);
			engineGraphicBufferPushFace(vertIndex, 0, 6, 10);
			engineGraphicBufferPushFace(vertIndex, 0, 8, 10);
			break;
	}
	return vertNum;
}

static int createTriangle5(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int divide = 2 + 4;
	int vertNum = 3 + divide * 2;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}
		double ratio1 = 0.2;
		double ratio2 = 0.5;
		double radius1c = radius1 * 2 * ratio1;
		double radius2c = radius2 * 2 * ratio2;

		double r1x1 = x0 + radius1 * 2 * c1;
		double r1y1 = y0 + radius1 * 2 * s1;
		double r1x2 = x0 + radius1 * 2 * c2;
		double r1y2 = y0 + radius1 * 2 * s2;
		double r1x0 = x0 + ((r1x1 - x0) + (r1x2 - x0)) / 3;
		double r1y0 = y0 + ((r1y1 - y0) + (r1y2 - y0)) / 3;
		double r1rx1 = r1x0 + (r1x1 - r1x0) * ratio1;
		double r1ry1 = r1y0 + (r1y1 - r1y0) * ratio1;
		double r1rx2 = r1x0 + (r1x2 - r1x0) * ratio1;
		double r1ry2 = r1y0 + (r1y2 - r1y0) * ratio1;
		double r1rx0 = r1x0 + ((r1rx1 - r1x0) + (r1rx2 - r1x0)) * 2;
		double r1ry0 = r1y0 + ((r1ry1 - r1y0) + (r1ry2 - r1y0)) * 2;

		double r2x1 = x0 + radius2 * 2 * c1;
		double r2y1 = y0 + radius2 * 2 * s1;
		double r2x2 = x0 + radius2 * 2 * c2;
		double r2y2 = y0 + radius2 * 2 * s2;
		double r2x0 = x0 + ((r2x1 - x0) + (r2x2 - x0)) / 3;
		double r2y0 = y0 + ((r2y1 - y0) + (r2y2 - y0)) / 3;
		double r2rx1 = r2x0 + (r2x1 - r2x0) * ratio2;
		double r2ry1 = r2y0 + (r2y1 - r2y0) * ratio2;
		double r2rx2 = r2x0 + (r2x2 - r2x0) * ratio2;
		double r2ry2 = r2y0 + (r2y2 - r2y0) * ratio2;
		double r2rx0 = r2x0 + ((r2rx1 - r2x0) + (r2rx2 - r2x0)) * 2;
		double r2ry0 = r2y0 + ((r2ry1 - r2y0) + (r2ry2 - r2y0)) * 2;

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c1, y0 + radius0 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c2, y0 + radius0 * s2, 0);
		// 直線部分
		engineGraphicBufferPushVert(r1x1, r1y1, 0);
		engineGraphicBufferPushVert(r2x1, r2y1, 0);
		// 曲線部分
		int rdiv = divide - 2;
		engineGraphicBufferPushVert(r1rx1, r1ry1, 0);
		engineGraphicBufferPushVert(r2rx1, r2ry1, 0);
		for(int i = 1; i < rdiv - 1; i++){
			double degree1 = 60 - 30 * (1 - 2 * (double)i / (rdiv - 1));
			double degree2 = 30 - 60 * triangleIndex;
			double c = utilMathCos(CONST_DEG2RAD * (90 + degree1 + degree2));
			double s = utilMathSin(CONST_DEG2RAD * (90 + degree1 + degree2));
			engineGraphicBufferPushVert(r1rx0 + radius1c * c, r1ry0 + radius1c * s, 0);
			engineGraphicBufferPushVert(r2rx0 + radius2c * c, r2ry0 + radius2c * s, 0);
		}
		engineGraphicBufferPushVert(r1rx2, r1ry2, 0);
		engineGraphicBufferPushVert(r2rx2, r2ry2, 0);
		// 直線部分
		engineGraphicBufferPushVert(r1x2, r1y2, 0);
		engineGraphicBufferPushVert(r2x2, r2y2, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			for(int i = 0; i < divide - 1; i++){engineGraphicBufferPushFace(vertIndex, 1, 3 + i * 2, 3 + (i + 1) * 2);}
			engineGraphicBufferPushFace(vertIndex, 1, 3 + (divide - 1) * 2, 2);
			break;
		case 1:
			// 壁面作成
			for(int i = 0; i < divide - 1; i++){
				engineGraphicBufferPushFace(vertIndex, 3 + (i + 0) * 2, 4 + (i + 0) * 2, 4 + (i + 1) * 2);
				engineGraphicBufferPushFace(vertIndex, 3 + (i + 0) * 2, 3 + (i + 1) * 2, 4 + (i + 1) * 2);
			}
			break;
		case 2:
			// 外面作成
			for(int i = 0; i < divide - 1; i++){engineGraphicBufferPushFace(vertIndex, 0, 4 + i * 2, 4 + (i + 1) * 2);}
			break;
	}
	return vertNum;
}

static int createTriangle6(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 7;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c1, y0 + radius0 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c2, y0 + radius0 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius1 * c1, y0 + radius1 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius2 * c1, y0 + radius2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c2, y0 + radius1 * 2 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c2, y0 + radius2 * 2 * s2, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 1, 2, 3);
			engineGraphicBufferPushFace(vertIndex, 2, 3, 5);
			break;
		case 1:
			// 壁面作成
			engineGraphicBufferPushFace(vertIndex, 3, 4, 6);
			engineGraphicBufferPushFace(vertIndex, 3, 5, 6);
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 4, 6);
			break;
	}
	return vertNum;
}

static int createTriangle7(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int vertNum = 7;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c1, y0 + radius0 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c2, y0 + radius0 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius1 * 2 * c1, y0 + radius1 * 2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius2 * 2 * c1, y0 + radius2 * 2 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius1 * c2, y0 + radius1 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius2 * c2, y0 + radius2 * s2, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			engineGraphicBufferPushFace(vertIndex, 1, 2, 3);
			engineGraphicBufferPushFace(vertIndex, 2, 3, 5);
			break;
		case 1:
			// 壁面作成
			engineGraphicBufferPushFace(vertIndex, 3, 4, 6);
			engineGraphicBufferPushFace(vertIndex, 3, 5, 6);
			break;
		case 2:
			// 外面作成
			engineGraphicBufferPushFace(vertIndex, 0, 4, 6);
			break;
	}
	return vertNum;
}

static int createTriangle8(struct objectRollMapImplement *this, int phase, int hexIndex, int triangleIndex, int vertIndex){
	int divide = 4;
	int vertNum = 3 + divide * 2;
	if(phase == 0){
		// 頂点作成
		double radius0 = this->hexRadius;
		double radius1 = this->wallRadius1;
		double radius2 = this->wallRadius2;
		double x0 = this->hexInfo[hexIndex].x;
		double y0 = this->hexInfo[hexIndex].y;
		double c1 = 1.0; double s1 = 0.0;
		double c2 = 1.0; double s2 = 0.0;
		switch(triangleIndex){
			case 0: c1 = CONST_COS030; s1 = CONST_SIN030; c2 = CONST_COS330; s2 = CONST_SIN330; break;
			case 1: c1 = CONST_COS330; s1 = CONST_SIN330; c2 = CONST_COS270; s2 = CONST_SIN270; break;
			case 2: c1 = CONST_COS270; s1 = CONST_SIN270; c2 = CONST_COS210; s2 = CONST_SIN210; break;
			case 3: c1 = CONST_COS210; s1 = CONST_SIN210; c2 = CONST_COS150; s2 = CONST_SIN150; break;
			case 4: c1 = CONST_COS150; s1 = CONST_SIN150; c2 = CONST_COS090; s2 = CONST_SIN090; break;
			case 5: c1 = CONST_COS090; s1 = CONST_SIN090; c2 = CONST_COS030; s2 = CONST_SIN030; break;
		}

		engineGraphicBufferPushVert(x0, y0, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c1, y0 + radius0 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius0 * c2, y0 + radius0 * s2, 0);
		// 曲線部分
		engineGraphicBufferPushVert(x0 + radius1 * c1, y0 + radius1 * s1, 0);
		engineGraphicBufferPushVert(x0 + radius2 * c1, y0 + radius2 * s1, 0);
		for(int i = 1; i < divide - 1; i++){
			double degree = 30 - 60 * (triangleIndex + (double)i / (divide - 1));
			double c = utilMathCos(CONST_DEG2RAD * degree);
			double s = utilMathSin(CONST_DEG2RAD * degree);
			engineGraphicBufferPushVert(x0 + radius1 * c, y0 + radius1 * s, 0);
			engineGraphicBufferPushVert(x0 + radius2 * c, y0 + radius2 * s, 0);
		}
		engineGraphicBufferPushVert(x0 + radius1 * c2, y0 + radius1 * s2, 0);
		engineGraphicBufferPushVert(x0 + radius2 * c2, y0 + radius2 * s2, 0);

		for(int i = 0; i < vertNum; i++){engineGraphicBufferPushTexcWhite();}
	}

	switch(phase){
		case 0:
			// 内面作成
			for(int i = 0; i < divide - 1; i++){engineGraphicBufferPushFace(vertIndex, 1, 3 + i * 2, 3 + (i + 1) * 2);}
			engineGraphicBufferPushFace(vertIndex, 1, 3 + (divide - 1) * 2, 2);
			break;
		case 1:
			// 壁面作成
			for(int i = 0; i < divide - 1; i++){
				engineGraphicBufferPushFace(vertIndex, 3 + (i + 0) * 2, 4 + (i + 0) * 2, 4 + (i + 1) * 2);
				engineGraphicBufferPushFace(vertIndex, 3 + (i + 0) * 2, 3 + (i + 1) * 2, 4 + (i + 1) * 2);
			}
			break;
		case 2:
			// 外面作成
			for(int i = 0; i < divide - 1; i++){engineGraphicBufferPushFace(vertIndex, 0, 4 + i * 2, 4 + (i + 1) * 2);}
			break;
	}
	return vertNum;
}


// バッファ作成
void objectRollMapCreateArray(struct objectRollMap *that){
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	struct e3dImage *image[3];
	int length = this->hexXsize * this->hexYsize;

	image[0] = &this->super.imgBack;
	image[1] = &this->super.imgWall;
	image[2] = NULL;
	int vertIndex0 = engineGraphicBufferVretIndexGet();
	for(int i = 0; i < 3; i++){
		if(image[i] == NULL){continue;}
		image[i]->faceIndex = engineGraphicBufferFaceIndexGet();
		image[i]->faceNum = 0;

		int vertIndex = vertIndex0;
		for(int j = 0; j < length; j++){
			if(isWall(&this->hexInfo[j])){
				int hexx = j % this->hexXsize;
				int hexy = j / this->hexXsize;
				bool isWallAround[6];
				isWallAround[0] = isScope(this, hexx + 1, hexy + 0) ? isWall(&this->hexInfo[this->hexXsize * (hexy + 0) + (hexx + 1)]) : true;
				isWallAround[1] = isScope(this, hexx + 1, hexy - 1) ? isWall(&this->hexInfo[this->hexXsize * (hexy - 1) + (hexx + 1)]) : true;
				isWallAround[2] = isScope(this, hexx + 0, hexy - 1) ? isWall(&this->hexInfo[this->hexXsize * (hexy - 1) + (hexx + 0)]) : true;
				isWallAround[3] = isScope(this, hexx - 1, hexy + 0) ? isWall(&this->hexInfo[this->hexXsize * (hexy + 0) + (hexx - 1)]) : true;
				isWallAround[4] = isScope(this, hexx - 1, hexy + 1) ? isWall(&this->hexInfo[this->hexXsize * (hexy + 1) + (hexx - 1)]) : true;
				isWallAround[5] = isScope(this, hexx + 0, hexy + 1) ? isWall(&this->hexInfo[this->hexXsize * (hexy + 1) + (hexx + 0)]) : true;
				for(int k = 0; k < 6; k++){
					// 周囲の点の位置によって分岐
					bool isWallNext1 = isWallAround[(k + 1) % 6];
					bool isWallNext2 = isWallAround[(k + 5) % 6];
					if(isWallAround[k]){
						if(isWallNext1 && isWallNext2){
							vertIndex += createTriangle1(this, i, j, k, vertIndex);
						}else if(isWallNext1){
							vertIndex += createTriangle2(this, i, j, k, vertIndex);
						}else if(isWallNext2){
							vertIndex += createTriangle3(this, i, j, k, vertIndex);
						}else{
							vertIndex += createTriangle4(this, i, j, k, vertIndex);
						}
					}else{
						if(isWallNext1 && isWallNext2){
							vertIndex += createTriangle5(this, i, j, k, vertIndex);
						}else if(isWallNext1){
							vertIndex += createTriangle6(this, i, j, k, vertIndex);
						}else if(isWallNext2){
							vertIndex += createTriangle7(this, i, j, k, vertIndex);
						}else{
							vertIndex += createTriangle8(this, i, j, k, vertIndex);
						}
					}
				}
			}else{
				vertIndex += createHex(this, i, j, vertIndex);
			}
		}

		image[i]->faceNum = engineGraphicBufferFaceIndexGet() - image[i]->faceIndex;
	}

	// トラップバッファ作成
	for(int i = 0; i < 4; i++){
		int vertIndex = engineGraphicBufferVretIndexGet();
		int faceIndex = engineGraphicBufferFaceIndexGet();
		int tetraNum = 0;
		int length = this->hexXsize * this->hexYsize;
		for(int j = 0; j < length; j++){
			struct objectRollMapHexInfo *info = &this->hexInfo[j];
			int type = -1;
			int rot = 0;
			if(isTrapNeedle(info)){type = 0;}
			int accRot = isTrapAccelerator(info);
			if(accRot >= 0){type = 1; rot = accRot;}
			if(type < 0){continue;}
			int tu = 0;
			int tv = 0;
			int tw = 0;
			int th = 0;
			switch(type * 4 + i){
				case 0:  tu = TEXPOS_SYSTEM_TRAPNEEDLE1_X; tv = TEXPOS_SYSTEM_TRAPNEEDLE1_Y; tw = TEXPOS_SYSTEM_TRAPNEEDLE1_W; th = TEXPOS_SYSTEM_TRAPNEEDLE1_H; break;
				case 1:  tu = TEXPOS_SYSTEM_TRAPNEEDLE2_X; tv = TEXPOS_SYSTEM_TRAPNEEDLE2_Y; tw = TEXPOS_SYSTEM_TRAPNEEDLE2_W; th = TEXPOS_SYSTEM_TRAPNEEDLE2_H; break;
				case 2:  tu = TEXPOS_SYSTEM_TRAPNEEDLE3_X; tv = TEXPOS_SYSTEM_TRAPNEEDLE3_Y; tw = TEXPOS_SYSTEM_TRAPNEEDLE3_W; th = TEXPOS_SYSTEM_TRAPNEEDLE3_H; break;
				case 3:  tu = TEXPOS_SYSTEM_TRAPNEEDLE4_X; tv = TEXPOS_SYSTEM_TRAPNEEDLE4_Y; tw = TEXPOS_SYSTEM_TRAPNEEDLE4_W; th = TEXPOS_SYSTEM_TRAPNEEDLE4_H; break;
				case 4:  tu = TEXPOS_SYSTEM_TRAPDASH1_X; tv = TEXPOS_SYSTEM_TRAPDASH1_Y; tw = TEXPOS_SYSTEM_TRAPDASH1_W; th = TEXPOS_SYSTEM_TRAPDASH1_H; break;
				case 5:  tu = TEXPOS_SYSTEM_TRAPDASH2_X; tv = TEXPOS_SYSTEM_TRAPDASH2_Y; tw = TEXPOS_SYSTEM_TRAPDASH2_W; th = TEXPOS_SYSTEM_TRAPDASH2_H; break;
				case 6:  tu = TEXPOS_SYSTEM_TRAPDASH3_X; tv = TEXPOS_SYSTEM_TRAPDASH3_Y; tw = TEXPOS_SYSTEM_TRAPDASH3_W; th = TEXPOS_SYSTEM_TRAPDASH3_H; break;
				case 7:  tu = TEXPOS_SYSTEM_TRAPDASH4_X; tv = TEXPOS_SYSTEM_TRAPDASH4_Y; tw = TEXPOS_SYSTEM_TRAPDASH4_W; th = TEXPOS_SYSTEM_TRAPDASH4_H; break;
			}
			double x0 = tw * 0.2;
			double y0 = th * 0.2;
			double c = 1.0;
			double s = 0.0;
			switch(rot){
				case 0: c = CONST_COS030; s = CONST_SIN030; break;
				case 1: c = CONST_COS330; s = CONST_SIN330; break;
				case 2: c = CONST_COS270; s = CONST_SIN270; break;
				case 3: c = CONST_COS210; s = CONST_SIN210; break;
				case 4: c = CONST_COS150; s = CONST_SIN150; break;
				case 5: c = CONST_COS090; s = CONST_SIN090; break;
			}
			double x1 = c * x0 - s * y0;
			double y1 = s * x0 + c * y0;
			engineGraphicBufferPushVert(info->x - x1, info->y - y1, 0.0);
			engineGraphicBufferPushVert(info->x - y1, info->y + x1, 0.0);
			engineGraphicBufferPushVert(info->x + x1, info->y + y1, 0.0);
			engineGraphicBufferPushVert(info->x + y1, info->y - x1, 0.0);
			engineGraphicBufferPushTetraTexc(TEXSIZ_SYSTEM_WH, tu, tv, tw, th);
			tetraNum++;
		}
		for(int k = 0; k < tetraNum; k++){engineGraphicBufferPushTetraFace(vertIndex + k * 4);}
		this->super.imgTrap[i].faceIndex = faceIndex;
		this->super.imgTrap[i].faceNum = tetraNum * 2;
	}
}

// ----------------------------------------------------------------

// 破棄
void objectRollMapDispose(struct objectRollMap *that){
	if(that == NULL){return;}
	struct objectRollMapImplement *this = (struct objectRollMapImplement*)that;
	engineUtilMemoryFree(this->hexInfo);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

