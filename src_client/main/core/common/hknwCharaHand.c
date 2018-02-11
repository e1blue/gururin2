#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// パーツの設定
static void setParts(struct hknwCharaParts **ptrParts, int u, int v, bool isActive){
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.00, u +  0, v +  0, 32, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.00, u +  0, v + 32, 32, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_FTR1,  0.00,  0.00,  0.00, u +  0, v + 64, 16, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_FTL1,  0.00,  0.00,  0.00, u +  0, v + 64, 16, 0x10, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_FTR2,  0.00,  0.00,  0.00, u + 64, v + 64, 16, 0x00, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_FTL2,  0.00,  0.00,  0.00, u + 64, v + 64, 16, 0x10, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HNDR,  0.00,  0.00,  0.00, u +  0, v + 80, 16, 0x01, isActive);
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HNDL,  0.00,  0.00,  0.00, u +  0, v + 80, 16, 0x11, isActive);
}

// 直立ポーズの設定
static void setPosesStand(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// 歩行ポーズの設定
static void setPosesWalk(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.58, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HNDR,  0.10,  0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HNDL, -0.10, -0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.33, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.49, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HNDR, -0.05,  0.13,  0.12, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HNDL,  0.05, -0.13,  0.12, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.58, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HNDR, -0.10,  0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HNDL,  0.10, -0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.33, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.49, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.62, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HNDR,  0.05,  0.13,  0.12, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HNDL, -0.05, -0.13,  0.12, CONST_PI * 0.0, 0x00, isActive);
}

// しゃがみポーズの設定
static void setPosesSquat(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.27, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.41, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.52, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.52, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.13,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// ジャンプポーズの設定
static void setPosesJump(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.11,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.11,  0.10, CONST_PI * 0.0, 0x00, isActive);
}

// 落下ポーズの設定
static void setPosesFall(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47, CONST_PI * 0.0, 0x01, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60, CONST_PI * 0.0, 0x11, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.15,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.15,  0.10, CONST_PI * 0.0, 0x00, isActive);
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

		switch(i){
			case 0: case 2: case 4: case 6:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47 + jump, CONST_PI * 0.0, 0x01, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.16,  0.13 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.16,  0.13 + jump, CONST_PI * 0.0, 0x00, isActive); break;
			case 1: case 5:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47 + jump, CONST_PI * 0.0, 0x01, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.18,  0.15 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.18,  0.15 + jump, CONST_PI * 0.0, 0x00, isActive); break;
			default:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.04,  0.00,  0.31 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.10,  0.00,  0.47 + jump, CONST_PI * 0.0, 0x01, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR2,  0.02,  0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL2, -0.02, -0.10,  0.60 + jump, CONST_PI * 0.0, 0x11, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.13,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.13,  0.10 + jump, CONST_PI * 0.0, 0x00, isActive); break;
		}
	}
}

// やられ1ポーズの設定
static void setPosesBreak1(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.05,  0.00,  0.48, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.24, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_FTR1,  0.00,  0.04,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_FTL2,  0.10, -0.08,  0.15, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.20,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK1, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.20,  0.25, CONST_PI * 0.0, 0x00, isActive);
}

// やられ2ポーズの設定
static void setPosesBreak2(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.40, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.00,  0.15, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.12,  0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_FTL2,  0.12, -0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.12,  0.15,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_BREAK2, 0, HKNWCHARAPARTSID_HUMAN_HNDL, -0.12, -0.15,  0.10, CONST_PI * 0.0, 0x00, isActive);
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
// 人間系逆立ちキャラの作成
void hknwCharaInitDataHand(struct hknwChara *this, int imgw, int imgh, int u, int v){
	this->faceNum = 0;
	this->imgw = imgw;
	this->imgh = imgh;
	
	this->radius2d = 10.00;
	this->radius3d =  0.28;

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

