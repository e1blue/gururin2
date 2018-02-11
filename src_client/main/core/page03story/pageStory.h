#ifndef __pageStory_h_
#define __pageStory_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 構造体の前方宣言
struct pageStoryStatus;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// キャラクターの感情フキダシ
enum pageStoryEmotion{
	PAGESTORYEMOTION_NONE,
	PAGESTORYEMOTION_HAPPY,
	PAGESTORYEMOTION_EXCLAMATION,
	PAGESTORYEMOTION_QUESTION,
	PAGESTORYEMOTION_SAD,
	PAGESTORYEMOTION_ANGRY,
};

// ストーリーステップ
struct pageStoryEngineStep{
	enum pageStoryEngineStepType{
		PAGESTORYENGINESTEPTYPE_NONE = 0,
		PAGESTORYENGINESTEPTYPE_CREATEMAP,
		PAGESTORYENGINESTEPTYPE_SETCAMERA,
		PAGESTORYENGINESTEPTYPE_SETGRAVITY,
		PAGESTORYENGINESTEPTYPE_CREATECHARA,
		PAGESTORYENGINESTEPTYPE_SETCHARAPOSE,
		PAGESTORYENGINESTEPTYPE_SETCHARAEMOTION,
		PAGESTORYENGINESTEPTYPE_SETCHARASTATUS,
		PAGESTORYENGINESTEPTYPE_ACTCHARAWALK,
		PAGESTORYENGINESTEPTYPE_ACTCHARAJUMP,
		PAGESTORYENGINESTEPTYPE_REMOVECHARA,
		PAGESTORYENGINESTEPTYPE_CREATEITEM,
		PAGESTORYENGINESTEPTYPE_REMOVEITEM,
		PAGESTORYENGINESTEPTYPE_WAIT,
		PAGESTORYENGINESTEPTYPE_SKIPBREAK,
	} type;
	union{
		struct{int mapId; int xsize; int ysize; int zsize; int *mapchip;} createMap;
		struct{int charaId; int active; double x; double y; double z; double scale;} setCamera;
		struct{double time; double rotg;} setGravity;
		struct{int charaId; enum hknwCharaKind kind; double scale; bool isShadow;} createChara;
		struct{int charaId; enum hknwCharaPosesId pose;} setCharaPose;
		struct{int charaId; enum pageStoryEmotion emotion; double scale;} setCharaEmotion;
		struct{int charaId; double time; int active; double x; double y; double z; double r; double rotg;} setCharaStatus;
		struct{int charaId; double time; int active; double x; double y; double z; double speed;} actCharaWalk;
		struct{int charaId; double time; int active; double x; double y; double z; double height;} actCharaJump;
		struct{int charaId;} removeChara;
		struct{int itemId; int type; double x; double y; double z;} createItem;
		struct{int itemId;} removeItem;
		struct{double time;} wait;
		struct{} skipBreak;
	};
	// 一時データ
	int step;
};

// ----------------------------------------------------------------
// ストーリーエンジン関数

void objectStoryEngineInit(struct pageStoryStatus *this, byte *json, size_t size);
void objectStoryEngineCalc(struct pageStoryStatus *this);
void objectStoryEngineDispose(struct pageStoryStatus *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// エフェクト管理構造体
struct objectStoryEffectManager{
	struct pageStoryStatus *stat;

	struct e3dTransListManager *transManager;

	struct e3dImage imgDust[4];
	struct e3dImage imgStar[4][2];

	struct objectStoryEffectDust{
		struct e3dTrans trans;
		struct objectStoryEffectManager *manager;
		double x;
		double y;
		double z;
		int step;
	} dustList[8];

	struct objectStoryEffectStar{
		struct e3dTrans trans;
		struct objectStoryEffectManager *manager;
		int type;
		double x;
		double y;
		double z;
		double vx;
		double vy;
		double vz;
		int step;
	} starList[8];
};

void objectStoryEffectManagerInit(struct objectStoryEffectManager *this, struct pageStoryStatus *stat, struct e3dTransListManager *transManager);
void objectStoryEffectManagerCalc(struct objectStoryEffectManager *this);
void objectStoryEffectManagerCreateArray(struct objectStoryEffectManager *this);
void objectStoryEffectManagerReset(struct objectStoryEffectManager *this);

void objectStoryEffectManagerPushDust(struct objectStoryEffectManager *this, double x, double y, double z);
void objectStoryEffectManagerPushStar(struct objectStoryEffectManager *this, int type, double x, double y, double z);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページ状態構造体
struct pageStoryStatus{
	// ストーリー情報
	int id;
	int icon;
	int wall;
	int nextStageId;

	// ストーリーステップ
	struct pageStoryEngineStep *stepList;
	int stepLength;

	// マップデータ
	struct pageStoryStatusMap{
		int mapId;
		struct hknwMap *map;
	} *mapList;
	int mapLength;

	// キャラクターデータ
	struct pageStoryStatusChara{
		int charaId;
		bool isExist;
		bool isShadow;
		double scale;
		double x;
		double y;
		double z;
		double r;
		double rotg;
		double rad;
		enum hknwCharaKind kind;
		enum hknwCharaPosesId pose;
		int step;
		// フキダシデータ
		enum pageStoryEmotion emotion;
		double emotionScale;
		int stepBalloon;
		// 一時データ
		bool stepActed;
		double stepx;
		double stepy;
		double stepz;
		double stepr;
		double stepRotg;
		double rotv;
		double roth;
	} *charaList;
	int charaLength;
	struct hknwCharaManager charaManager;

	// アイテムデータ
	struct pageStoryStatusItem{
		int itemId;
		int type;
		bool isExist;
		double x;
		double y;
		double z;
	} *itemList;
	int itemLength;

	// エフェクトデータ
	struct objectStoryEffectManager effectManager;

	bool isSkip;
	// カメラデータ
	bool isInitCamera;
	int cameraCharaId;
	double camerax;
	double cameray;
	double cameraz;
	double cameraDist;
	double cameraxNext;
	double camerayNext;
	double camerazNext;
	double cameraDistNext;
	// 画面回転と重力方向
	double rotv;
	double roth;
	double rotg;
	// 一時データ
	bool isWait;
	bool isFinish;
	bool isExit;
	bool stepActed;
	double stepRotg;
};

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

