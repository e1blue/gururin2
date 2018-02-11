#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// パーツの設定
static void setParts(struct hknwCharaParts **ptrParts, int type, int u, int v, bool isActive){
	hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.00, u +  0, v +  0, 32, 0x00, isActive);
	switch(type){
		case 1:
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD, -0.16,  0.00,  0.08, u +  0, v + 96, 32, 0x00, isActive);
			break;
		case 2:
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD, -0.16,  0.00,  0.08, u +  0, v +  96, 32, 0x00, isActive);
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD, -0.19,  0.00, -0.09, u +  0, v + 128, 32, 0x00, isActive);
			break;
		case 3:
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD, -0.15,  0.15, -0.15, u +  0, v +  96, 32, 0x00, isActive);
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD, -0.15, -0.15, -0.15, u +  0, v +  96, 32, 0x10, isActive);
			hknwCharaPartsSet((*ptrParts)++, HKNWCHARAPARTSID_HUMAN_HEAD,  0.10,  0.15,  0.10, u +  0, v + 128, 32, 0x00, isActive);
	}
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
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.52, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.00,  0.27, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_FTR1,  0.02,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_FTL1, -0.02, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.02,  0.20,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_STAND, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.20,  0.25, CONST_PI * 0.0, 0x00, isActive);
}

// 歩行ポーズの設定
static void setPosesWalk(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.12,  0.00,  0.45, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.23, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_FTR1,  0.10,  0.07,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.20, -0.07,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HNDR, -0.10,  0.15,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.10, -0.15,  0.25, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HEAD,  0.12,  0.00,  0.47, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.26, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_FTR1,  0.00,  0.07,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_FTL1,  0.00, -0.07,  0.15, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HNDR, -0.05,  0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 1, HKNWCHARAPARTSID_HUMAN_HNDL,  0.05, -0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HEAD,  0.12,  0.00,  0.45, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.23, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_FTR2, -0.20,  0.07,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_FTL1,  0.10, -0.07,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HNDR,  0.10,  0.15,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 2, HKNWCHARAPARTSID_HUMAN_HNDL, -0.10, -0.15,  0.25, CONST_PI * 0.0, 0x00, isActive);

	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HEAD,  0.12,  0.00,  0.47, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_BODY,  0.00,  0.00,  0.26, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_FTR1,  0.00,  0.07,  0.15, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_FTL1,  0.00, -0.07,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HNDR,  0.05,  0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_WALK, 3, HKNWCHARAPARTSID_HUMAN_HNDL, -0.05, -0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);
}

// しゃがみポーズの設定
static void setPosesSquat(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.12,  0.00,  0.43, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.00,  0.22, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_FTR1, -0.02,  0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_FTL1, -0.02, -0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HNDR,  0.05,  0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_SQUAT, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.05, -0.18,  0.25, CONST_PI * 0.0, 0x00, isActive);
}

// ジャンプポーズの設定
static void setPosesJump(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.45, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_FTR2, -0.12,  0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_FTL2, -0.12, -0.10,  0.10, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02,  0.20,  0.28, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_JUMP, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.20,  0.28, CONST_PI * 0.0, 0x00, isActive);
}

// 落下ポーズの設定
static void setPosesFall(struct hknwCharaPoses **ptrPoses, bool isActive){
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00,  0.45, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.00,  0.20, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_FTR2,  0.12,  0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_FTL2,  0.12, -0.10,  0.10, CONST_PI * 1.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02,  0.20,  0.28, CONST_PI * 0.0, 0x00, isActive);
	hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_FALL, 0, HKNWCHARAPARTSID_HUMAN_HNDL,  0.02, -0.20,  0.28, CONST_PI * 0.0, 0x00, isActive);
}

// うれしいときのポーズ設定
static void setPosesHappy(struct hknwCharaPoses **ptrPoses, bool isActive){
	// 猫ダンス
	for(int i = 0; i < 20; i++){
		double jump = 0.0;
		switch(i){
			case 0: case 4: case 10: case 14: jump = 0.10; break;
			case 1: case 5: case 11: case 15: jump = 0.15; break;
			case 2: case 6: case 12: case 16: jump = 0.10; break;
		}

		switch(i){
			case 0: case 2: case 4: case 6:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00, 0.52 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.02, -0.02, 0.27 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR1, -0.02, -0.03, 0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL2, -0.12,  0.12, 0.20 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02, -0.18, 0.45 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL, -0.02,  0.18, 0.45 + jump, CONST_PI * 0.0, 0x00, isActive); break;
			case 1: case 3: case 5: case 7: case 8: case 9:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00, 0.52 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.02, -0.02, 0.27 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR1, -0.02, -0.03, 0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL2, -0.12,  0.12, 0.20 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02, -0.20, 0.35 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL, -0.02,  0.20, 0.35 + jump, CONST_PI * 0.0, 0x00, isActive); break;
			case 10: case 12: case 14: case 16:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00, 0.52 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.02, 0.27 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR2, -0.12, -0.12, 0.20 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL1, -0.02,  0.03, 0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02, -0.18, 0.45 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL, -0.02,  0.18, 0.45 + jump, CONST_PI * 0.0, 0x00, isActive); break;
			case 11: case 13: case 15: case 17: case 18: case 19:
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HEAD,  0.00,  0.00, 0.52 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_BODY, -0.02,  0.02, 0.27 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTR2, -0.12, -0.12, 0.20 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_FTL1, -0.02,  0.03, 0.10 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDR,  0.02, -0.20, 0.35 + jump, CONST_PI * 0.0, 0x00, isActive);
				hknwCharaPosesSet((*ptrPoses)++, HKNWCHARAPOSESID_HAPPY, i, HKNWCHARAPARTSID_HUMAN_HNDL, -0.02,  0.20, 0.35 + jump, CONST_PI * 0.0, 0x00, isActive); break;
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
static void setPartsPoses(struct hknwCharaParts **ptrParts, struct hknwCharaPoses **ptrPoses, int type, int u, int v, bool isActive){
	setParts(ptrParts, type, u, v, isActive);
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

// 人間系キャラの作成
void hknwCharaInitDataHuman(struct hknwChara *this, int type, int imgw, int imgh, int u, int v){
	this->faceNum = 0;
	this->imgw = imgw;
	this->imgh = imgh;
	
	this->radius2d = 10.00;
	this->radius3d =  0.28;

	// 要素数を数える
	struct hknwCharaParts *ptrParts = NULL;
	struct hknwCharaPoses *ptrPoses = NULL;
	setPartsPoses(&ptrParts, &ptrPoses, type, u, v, false);
	this->partsNum = (int)(ptrParts - (struct hknwCharaParts*)NULL);
	this->posesNum = (int)(ptrPoses - (struct hknwCharaPoses*)NULL);
	// 数えた要素数でメモリ確保
	ptrParts = this->parts = (struct hknwCharaParts*)engineUtilMemoryCalloc(this->partsNum, sizeof(struct hknwCharaParts));
	ptrPoses = this->poses = (struct hknwCharaPoses*)engineUtilMemoryCalloc(this->posesNum, sizeof(struct hknwCharaPoses));

	// 要素作成
	setPartsPoses(&ptrParts, &ptrPoses, type, u, v, true);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

