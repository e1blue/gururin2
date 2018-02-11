#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// パーツの設定
static void setParts(struct hknwCharaParts **ptrParts, int u, int v, bool isActive){
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.00, u + 0, v +  0, 32, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_CART_BEAK,  0.00,  0.00,  0.00, u + 0, v + 32, 32, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_CART_TIRE,  0.00,  0.00,  0.00, u + 0, v + 64, 16, 0x02, isActive);
}

// 直立ポーズの設定
static void setPosesStand(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// 歩行ポーズの設定
static void setPosesWalk(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.23, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.18, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// しゃがみポーズの設定
static void setPosesSquat(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.15, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// ジャンプポーズの設定
static void setPosesJump(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// 落下ポーズの設定
static void setPosesFall(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// うれしいときのポーズ設定
static void setPosesHappy(struct hknwCharaPoses **ptrPoses, bool isActive){
	// ジャンプ
	for(int i = 0; i < 10; i++){
		double jump = 0.0;
		switch(i){
			case 0: case 4: jump = 0.10; break;
			case 1: case 5: jump = 0.15; break;
			case 2: case 6: jump = 0.10; break;
		}
	
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25 + jump, CONST_PI * 0.0, 0x00, isActive);
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20 + jump, CONST_PI * 0.0, 0x00, isActive);
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
		hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
	}
}

// やられ1ポーズの設定
static void setPosesBreak1(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// やられ2ポーズの設定
static void setPosesBreak2(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_BODY,  0.00,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_BEAK,  0.15,  0.00,  0.15, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_TIRE,  0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12, -0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_CART_TIRE, -0.12,  0.12,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 上記関数を呼び出す
static void setPartsPoses(struct hknwCharaParts **ptrParts, struct hknwCharaPoses **ptrPoses, int u, int v, bool isActive){
	setParts(ptrParts, u, v, isActive);
	setPosesStand(ptrPoses, isActive);
	setPosesWalk(ptrPoses, isActive);
	setPosesSquat(ptrPoses, isActive);
	setPosesJump(ptrPoses, isActive);
	setPosesFall(ptrPoses, isActive);
	setPosesHappy(ptrPoses, isActive);
	setPosesBreak1(ptrPoses, isActive);
	setPosesBreak2(ptrPoses, isActive);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// カート系キャラの作成
void hknwCharaInitDataBuggy(struct hknwChara *this, int imgw, int imgh, int u, int v){
	this->faceNum = 0;
	this->imgw = imgw;
	this->imgh = imgh;
	
	this->radius2d = 7.00;
	this->radius3d = 0.20;

	// 要素数を数える
	struct hknwCharaParts *ptrParts = NULL;
	struct hknwCharaPoses *ptrPoses = NULL;
	setPartsPoses(&ptrParts, &ptrPoses, u, v, false);
	this->partsNum = (int)(ptrParts - (struct hknwCharaParts*)NULL);
	this->posesNum = (int)(ptrPoses - (struct hknwCharaPoses*)NULL);
	// 数えた要素数でメモリ確保
	ptrParts = this->parts = (struct hknwCharaParts*)engineUtilMemoryCalloc(this->partsNum, sizeof(struct hknwCharaParts));
	ptrPoses = this->poses = (struct hknwCharaPoses*)engineUtilMemoryCalloc(this->posesNum, sizeof(struct hknwCharaPoses));

	// 要素作成
	setPartsPoses(&ptrParts, &ptrPoses, u, v, true);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

