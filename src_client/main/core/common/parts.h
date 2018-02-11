#ifndef __parts_h_
#define __parts_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ボタン構造体
struct partsButton{
	double x;
	double y;
	double w;
	double h;
	bool active;
	bool inactive;
	bool trigger;
	struct partsButton *child;
	bool zKey;
	bool xKey;
	bool cKey;
	bool vKey;
	bool inner;

	bool isMatrix;
	struct engineMathVector3 hitVertex0[6];
	struct engineMathVector3 hitVertex1[6];
	int hitVertexLength;
};

void partsButtonInit(struct partsButton *this);
void partsButtonPosition(struct partsButton *this, double x, double y, double w, double h);
void partsButtonSetMatrix(struct partsButton *this, struct engineMathMatrix44 *mat);
void partsButtonCalc(struct partsButton *this);
void partsButtonSubCalc(struct partsButton *this, struct engineCtrlTouch *t, bool clickable);
bool partsButtonIsInner(struct partsButton *this, struct engineCtrlTouch *t);

// ----------------------------------------------------------------

// 表示箱付きボタン構造体
struct partsButtonBox{
	struct partsButton super;
	struct e3dTrans trans;
	struct e3dImage imgText;
	int faceIndex;
	bool select;
};

void partsButtonBoxInit(struct partsButtonBox *this);
void partsButtonBoxCreateArray(struct partsButtonBox *this, int x, int y, int w, int h, char *text, double scale);
#define partsButtonBoxCalc(this) partsButtonCalc((struct partsButton*)(this))
#define partsButtonBoxSubCalc(this, t, clickable) partsButtonSubCalc((struct partsButton*)(this), t, clickable)
void partsButtonBoxDraw(struct partsButtonBox *this, struct engineMathMatrix44 *mat, double alpha);

// ----------------------------------------------------------------

// ピッカーボタン構造体
struct partsButtonPicker{
	struct partsButton super;
	struct e3dTrans trans;
	struct e3dImage imgText[5];
	int picked;
	int faceIndex;
	bool select;
};

void partsButtonPickerInit(struct partsButtonPicker *this, int picked);
void partsButtonPickerCreateArray(struct partsButtonPicker *this, int x, int y, int w, char **textList, int textListLength, double scale);
#define partsButtonPickerCalc(this) partsButtonCalc((struct partsButton*)(this))
#define partsButtonPickerSubCalc(this, t, clickable) partsButtonSubCalc((struct partsButton*)(this), t, clickable)
void partsButtonPickerDraw(struct partsButtonPicker *this, struct engineMathMatrix44 *mat, double alpha);

// ----------------------------------------------------------------

// ヘックスボタン構造体
struct partsButtonHex{
	struct partsButton super;
	struct e3dTrans trans;
	double rotInner;
	double rotOuter;
	int faceIndex;
	bool isIcon;
	bool select;
};

void partsButtonHexInit(struct partsButtonHex *this);
void partsButtonHexCreateArray(struct partsButtonHex *this, int x, int y, int r, int tu, int tv, int tw, int th);
#define partsButtonHexCalc(this) partsButtonCalc((struct partsButton*)(this))
#define partsButtonHexSubCalc(this, t, clickable) partsButtonSubCalc((struct partsButton*)(this), t, clickable)
void partsButtonHexDraw(struct partsButtonHex *this, struct engineMathMatrix44 *mat, double alpha);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// メモリートレースボタン

#ifdef FUHAHA_MEMORY_DEBUG
void partsButtonMemoryCalcImplement();
void partsButtonMemoryDrawImplement();
#define partsButtonMemoryCalc() partsButtonMemoryCalcImplement()
#define partsButtonMemoryDraw() partsButtonMemoryDrawImplement()
#else
#define partsButtonMemoryCalc()
#define partsButtonMemoryDraw()
#endif

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ステージエンブレム構造体
struct partsEmblem{
	int icon;
	int mark;
	int score;
	bool isLock;
	bool isStage;
	bool isButton;
	bool isReplay;
	struct e3dTrans trans;
	struct e3dImage imgEmblem;
	struct e3dImage imgBorder;
	struct e3dImage imgStage;
	struct e3dImage imgStar;
	struct e3dImage imgLock;
	struct e3dImage imgBox;
	struct partsButtonHex btnStart;
	struct partsButtonHex btnReplay;
};

void partsEmblemInitStage(struct partsEmblem *this, int icon, int mark, int score, bool isButton, bool isReplay);
void partsEmblemCreateArray(struct partsEmblem *this, int x, int y, double r);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 移動する壁紙作成
void partsWallpaperCreateArray(struct e3dImage *image, int type, int *width, int *height);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 何もしないですぐ消えるポップアップ ポップアップ間の処理挿入用
struct popupCartridge *partsPopupNothing();

// ローディングポップアップ
#define PARTSPOPUPLOADING_LOADED_OK      1
#define PARTSPOPUPLOADING_LOADED_RETRY   2
#define PARTSPOPUPLOADING_LOADED_PENDING 3
struct popupCartridge *partsPopupLoading(void *stat, void(*load)(void*), int(*loaded)(void*), void(*cancel)(void*));

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

