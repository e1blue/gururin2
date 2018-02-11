#ifndef __pageRoll_h_
#define __pageRoll_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 構造体の前方宣言
struct pageRollStatus;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// データ解釈
void objectRollParserParse(struct pageRollStatus *this, byte *json, size_t size);
void objectRollParserReset(struct pageRollStatus *this);
void objectRollParserDispose(struct pageRollStatus *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// グルリンマップ構造体
struct objectRollMap{
	struct pageRollStatus *stat;
	struct e3dImage imgBack;
	struct e3dImage imgWall;
	struct e3dImage imgTrap[4];
};

// グルリンキャラクタ構造体
struct objectRollMapChara{
	// 位置
	double x;
	double y;
	int hexx;
	int hexy;
	int hexRange;
	// 速度
	double vx;
	double vy;
	// 垂直方向の単位ベクトル
	double nx;
	double ny;
	// 半径
	double rad;
};

struct objectRollMap *objectRollMapInit(struct pageRollStatus *stat, int hexXsize, int hexYsize, int *hexChip);
double objectRollMapCoordX(struct objectRollMap *this, int hexx, int hexy);
double objectRollMapCoordY(struct objectRollMap *this, int hexx, int hexy);
void objectRollMapCalcHex(struct objectRollMap *this, struct objectRollMapChara *chara);
bool objectRollMapCalcTrapNeedle(struct objectRollMap *this, struct objectRollMapChara *chara);
bool objectRollMapCalcTrapAccelerator(struct objectRollMap *this, struct objectRollMapChara *chara);
bool objectRollMapCalcCollision(struct objectRollMap *this, struct objectRollMapChara *chara);
void objectRollMapCreateArray(struct objectRollMap *this);
void objectRollMapDispose(struct objectRollMap *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// エフェクト管理構造体
struct objectRollEffectManager{
	struct pageRollStatus *stat;

	struct e3dTransListManager *transManager;

	struct e3dImage imgDust[4];
	struct e3dImage imgStar[4][2];

	struct objectRollEffectDust{
		struct e3dTrans trans;
		struct objectRollEffectManager *manager;
		double x;
		double y;
		int step;
	} dustList[8];

	struct objectRollEffectStar{
		struct e3dTrans trans;
		struct objectRollEffectManager *manager;
		int type;
		double x;
		double y;
		double vx;
		double vy;
		int step;
	} starList[8];
};

void objectRollEffectManagerInit(struct objectRollEffectManager *this, struct pageRollStatus *stat, struct e3dTransListManager *transManager);
void objectRollEffectManagerCalc(struct objectRollEffectManager *this);
void objectRollEffectManagerCreateArray(struct objectRollEffectManager *this);
void objectRollEffectManagerReset(struct objectRollEffectManager *this);

void objectRollEffectManagerPushDust(struct objectRollEffectManager *this, double x, double y);
void objectRollEffectManagerPushStar(struct objectRollEffectManager *this, int type, double x, double y);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 報酬描画構造体
struct objectRollReword{
	struct pageRollStatus *stat;

	struct e3dImage imgGround;
	struct e3dImage imgStar[5];

	struct objectRollRewordItem{
		int type;
		int step;
		double x;
		double y;
		double vx;
		double vy;
		double ax;
		double ay;
		double rad;
		double mass;
	} *itemList;
	int itemLength;
};

void objectRollRewordInit(struct objectRollReword *this, struct pageRollStatus *stat);
void objectRollRewordCalc(struct objectRollReword *this);
void objectRollRewordCreateArray(struct objectRollReword *this);
void objectRollRewordDraw(struct objectRollReword *this, struct engineMathMatrix44 *mat, double alpha);
void objectRollRewordDispose(struct objectRollReword *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページ状態構造体
struct pageRollStatus{
	// ステージ情報
	int id;
	int icon;
	int mark;
	int wall1;
	int wall2;
	struct storageProgress0001 progress;
	struct storageStage0001 storage;
	// リプレイ情報
	bool isReplayWeb;
	bool isReplayStorage;
	bool isSetSeed;
	unsigned long beforeSeed[4];
	unsigned long replaySeed[4];

	// プレイヤーステータス
	double health;
	double gravityPower;
	int openStageIndex;
	int openStoryIndex;
	int openPuppetIndex;
	int nextStageId;
	int nextStoryId;
	enum hknwCharaKind playerKind;
	// ゲームスコア要素
	int getCoin;
	int getStar;
	int getPlayerStar;
	int getEnemyStar;
	int getHealth;
	// ゲームクリアステータス
	int score;
	int damage;
	int replayLength;
	long long replayId;
	bool isHighScore;

	// マップデータ
	struct objectRollMap **mapList;
	int mapLength;

	// キャラクターデータ
	struct pageRollStatusChara{
		int team;
		int mapIndex;
		int hp;
		struct objectRollMapChara data;
		enum hknwCharaKind kind;
		enum hknwCharaPosesId pose;
		int step;
		double scale;
		double rotv;
		double roth;
		bool isExist;
		bool isGround;
		int countGround;
		int countDamage;
	} *charaList;
	int charaLength;
	struct hknwCharaManager charaManager;

	// アイテムデータ
	struct pageRollStatusItem{
		int type;
		double x;
		double y;
		bool isExist;
	} *itemList;
	int itemLength;

	// エフェクトデータ
	struct objectRollEffectManager effectManager;

	// カメラデータ
	double camerax;
	double cameray;
	double cameraRot;
	double cameraScale;
	double cameraxNext;
	double camerayNext;
	double cameraRotNext;
	double cameraScaleNext;
	struct engineMathMatrix44 cameraMat;

	// 重力情報とリプレイ用データ
	double deviceRot;
	double gravityRot;
	double gravityCos;
	double gravitySin;
	short *gravityList;
	int gravityListIndex;
	int gravityListLength;
	double dispDeviceRot;
	double dispGravityRot;
};

// カメラ減衰計算
void pageRollDamperCamera(struct pageRollStatus *this);
// キャラクター描画
void pageRollDrawChara(struct pageRollStatus *this, struct pageRollStatusChara *chara, struct engineMathMatrix44 *mat, double alpha);

// ----------------------------------------------------------------
// ポップアップ関数

struct popupCartridge *popupRollStart(struct pageRollStatus *stat);

struct popupCartridge *popupRollGameClear(struct pageRollStatus *stat);
int popupRollGameClearResult(struct popupCartridge *this);

struct popupCartridge *popupRollGameOver(struct pageRollStatus *stat);
int popupRollGameOverResult(struct popupCartridge *this);

struct popupCartridge *popupRollPause(struct pageRollStatus *stat);
int popupRollPauseResult(struct popupCartridge *this);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

