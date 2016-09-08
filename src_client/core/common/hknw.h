#ifndef __hknw_h_
#define __hknw_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ハコニワキャラクタの種類名
enum hknwCharaKind{
	HKNWCHARAKIND_PLAYERGURU,
	HKNWCHARAKIND_PLAYERKUMA,
	HKNWCHARAKIND_PLAYERRIBB,
	HKNWCHARAKIND_PLAYERPEBU,
	HKNWCHARAKIND_PLAYERMAMI,
	HKNWCHARAKIND_PLAYERHAND,
	HKNWCHARAKIND_PLAYERBEAR,
	HKNWCHARAKIND_PLAYERGUDA,
	HKNWCHARAKIND_PLAYERROBO,
	HKNWCHARAKIND_PLAYERPIKA,
	HKNWCHARAKIND_ENEMYKORO,
	HKNWCHARAKIND_ENEMYBONE,
	HKNWCHARAKIND_ENEMYARMO,
};

// ハコニワキャラクタのパーツ名
enum hknwCharaPartsId{
	HKNWCHARAPARTSID_HUMAN_HEAD,
	HKNWCHARAPARTSID_HUMAN_BODY,
	HKNWCHARAPARTSID_HUMAN_FTR1,
	HKNWCHARAPARTSID_HUMAN_FTL1,
	HKNWCHARAPARTSID_HUMAN_FTR2,
	HKNWCHARAPARTSID_HUMAN_FTL2,
	HKNWCHARAPARTSID_HUMAN_HNDR,
	HKNWCHARAPARTSID_HUMAN_HNDL,
	HKNWCHARAPARTSID_CART_BODY,
	HKNWCHARAPARTSID_CART_BEAK,
	HKNWCHARAPARTSID_CART_TIRE,
	HKNWCHARAPARTSID_BALL_BOD1,
	HKNWCHARAPARTSID_BALL_BOD2,
};

// ハコニワキャラクタのポーズ名
enum hknwCharaPosesId{
	HKNWCHARAPOSESID_STAND,
	HKNWCHARAPOSESID_WALK,
	HKNWCHARAPOSESID_SQUAT,
	HKNWCHARAPOSESID_JUMP,
	HKNWCHARAPOSESID_FALL,
	HKNWCHARAPOSESID_HAPPY,
	HKNWCHARAPOSESID_BREAK1,
	HKNWCHARAPOSESID_BREAK2,
};

// ハコニワキャラクタ構造体
struct hknwChara{
	int faceIndex;
	int faceNum;

	int imgw;
	int imgh;

	double radius2d;
	double radius3d;

	int partsNum;
	struct hknwCharaParts{
		enum hknwCharaPartsId partsId;
		double x;
		double y;
		double z;
		int texx;
		int texy;
		int texs;
		int type;
	} *parts;

	int posesNum;
	struct hknwCharaPoses{
		enum hknwCharaPosesId posesId;
		enum hknwCharaPartsId partsId;
		int stepIndex;
		double x;
		double y;
		double z;
		double r;
		int type;
	} *poses;
};

struct hknwChara *hknwCharaInit(enum hknwCharaKind charaKind);
enum hknwCharaKind hknwCharaConvertKind(char *kind);
enum hknwCharaPosesId hknwCharaConvertPoseId(char *pose);

void hknwCharaPartsSet(struct hknwCharaParts *this, enum hknwCharaPartsId partsId, double x, double y, double z, int texx, int texy, int texs, int type, bool isActive);
void hknwCharaPosesSet(struct hknwCharaPoses *this, enum hknwCharaPosesId posesId, int stepIndex, enum hknwCharaPartsId partsId, double x, double y, double z, double r, int type, bool isActive);
void hknwCharaInitDataHuman(struct hknwChara *this, int type, int imgw, int imgh, int u, int v);
void hknwCharaInitDataKuma(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataBuggy(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataHand(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataBear(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataGuda(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataRobo(struct hknwChara *this, int imgw, int imgh, int u, int v);
void hknwCharaInitDataBall(struct hknwChara *this, int imgw, int imgh, int u, int v);

void hknwCharaCreateArray(struct hknwChara *this);
int hknwCharaGetPoseStepNum(struct hknwChara *this, enum hknwCharaPosesId poseId);
void hknwCharaDrawChara2D(struct hknwChara *this, struct engineMathMatrix44 *mat, double r, double s, enum hknwCharaPosesId poseId, int step);
void hknwCharaDrawChara3D(struct hknwChara *this, struct engineMathMatrix44 *mat, double rotv, double roth, double r, double s, enum hknwCharaPosesId poseId, int step);
void hknwCharaDispose(struct hknwChara *this);

// ----------------------------------------------------------------

// ハコニワキャラクタ管理構造体
struct hknwCharaManager{
	struct hknwCharaManagerDraw{
		enum hknwCharaKind kind;
		struct hknwChara *draw;
	} *drawList;
	int drawLength;
	int bufferCharaStatus;
	int e3dIdCharaVert;
	int e3dIdCharaTexc;
	int e3dIdCharaFace;
};

struct hknwChara *hknwCharaManagerGet(struct hknwCharaManager *this, enum hknwCharaKind charaKind);
void hknwCharaManagerCreateBuffer(struct hknwCharaManager *this);
void hknwCharaManagerDispose(struct hknwCharaManager *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ハコニワマップ構造体
struct hknwMap{
	int xsize;
	int ysize;
	int zsize;
	int *mapchip;

	int e3dIdVert;
	int e3dIdClor;
	int e3dIdTexc;
	int e3dIdFace;
	int faceNum;
};

struct hknwMap *hknwMapInit(int xsize, int ysize, int zsize, int *mapchip);
void hknwMapDraw(struct hknwMap *this, struct engineMathMatrix44 *mat);
void hknwMapDispose(struct hknwMap *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

