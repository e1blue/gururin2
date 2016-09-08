#ifndef __cartridge_h_
#define __cartridge_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジ構造体
struct pageCartridge{
	bool exist;
	struct pageCartridge *next;

	void (*init)(struct pageCartridge *this);
	void (*calc)(struct pageCartridge *this);
	void (*draw)(struct pageCartridge *this);
	void (*pause)(struct pageCartridge *this);
	void (*dispose)(struct pageCartridge *this);
};

void pageCartridgeGameInit();
void pageCartridgeGameCalc();
void pageCartridgeGamePause();
void pageCartridgeGameFinal();

void pageCartridgePush(struct pageCartridge *cartridge);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 各ページカートリッジ装填のコンストラクタ
void pageTestInit();
void pageMenuInitTitle();
void pageMenuInitSelect();
void pageRollInitTest();
void pageRollInit(int id, bool isReplay);
void pageStoryInitTest();
void pageStoryInit(int id, int nextStageId);

// 最初のページカートリッジ装填
#define FIRSTCARTRIDGE pageMenuInitTitle

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct popupCartridge{
	bool exist;
	struct popupCartridge *next;

	int openStep;
	int openStepMax;
	double openParam;
	double openSize;
	double openAlpha;
	double backParam;
	double backAlpha;
	bool isOpening;
	bool isClosing;
	bool isContinuousOpen;
	bool isContinuousClose;

	void (*init)(struct popupCartridge *this);
	void (*calc)(struct popupCartridge *this);
	void (*draw)(struct popupCartridge *this);
	void (*pause)(struct popupCartridge *this);
	void (*dispose)(struct popupCartridge *this);

	void (*onClose)(void *parent, struct popupCartridge *cartridge);
};

// ----------------------------------------------------------------

// ポップアップ管理構造体
struct popupManager{
	struct popupCartridge *popupCartridgeList;
};

bool popupManagerCalc(struct popupManager *this, void *parent);
void popupManagerDraw(struct popupManager *this);
void popupManagerPause(struct popupManager *this);
void popupManagerDispose(struct popupManager *this);

void popupManagerPush(struct popupManager *this, struct popupCartridge *cartridge, void(*onClose)(void*, struct popupCartridge*));

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

