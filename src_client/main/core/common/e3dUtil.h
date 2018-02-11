#ifndef __e3dUtil_h_
#define __e3dUtil_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// システム画像
extern engineGraphicTextureId e3dIdSystemImageLinear;
extern engineGraphicTextureId e3dIdSystemImageNearest;

// 行列データ保管場所
extern struct engineMathMatrix44 e3dMatOrtho;
extern struct engineMathMatrix44 e3dMatFrustum;

// ----------------------------------------------------------------

void e3dUtilGameInit();
void e3dUtilGameFinal();

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#define engineGraphicBufferPushTexcWhite() engineGraphicBufferPushTexc((double)16 / TEXSIZ_SYSTEM_W, (double)16 / TEXSIZ_SYSTEM_H)

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 描画構造体
struct e3dImage{
	int faceIndex;
	int faceNum;
};

// 配列に画像の描画情報を追加
void e3dImageCreateArray(struct e3dImage *this, int imgw, int imgh, double x, double y, double w, double h, int tu, int tv, int tw, int th);
#define e3dImageWhiteCreateArray(this, x, y, w, h) e3dImageCreateArray(this, TEXSIZ_SYSTEM_WH, x, y, w, h, 2, 2, 28, 28)

// 配列に箱の描画情報を追加
void e3dImageBoxCreateArray(struct e3dImage *this, double x, double y, double w, double h, int tu, int tv, int tw, int th);
#define e3dImageBoxDefaultCreateArray(this, x, y, w, h) e3dImageBoxCreateArray(this, x, y, w, h, TEXPOS_SYSTEM_BOXBASIC_XYWH)

// 配列に文字列(utf8)の描画情報を追加
void e3dImageTextCreateArray(struct e3dImage *this, double x, double y, char *text, double scale, int xalign, int yalign);

// 描画
void e3dImageDraw(struct e3dImage *this);

// ----------------------------------------------------------------

// 数列構造体
struct e3dNumber{
	int faceIndex;
	double width;
	int xalign;
};

// 配列に数列の描画情報を追加
void e3dNumberCreateArray(struct e3dNumber *this, double x, double y, int tu, int tv, int tw, int th, double scale, int xalign, int yalign);
#define e3dNumberDefaultCreateArray(this, x, y, scale, xalign, yalign) e3dNumberCreateArray(this, x, y, TEXPOS_SYSTEM_FONTABCD_X, TEXPOS_SYSTEM_FONTABCD_Y + 30, 20 * 10, 30, scale, xalign, yalign)
#define e3dNumberOutlineCreateArray(this, x, y, scale, xalign, yalign) e3dNumberCreateArray(this, x, y, TEXPOS_SYSTEM_FONTNUMBER_X, TEXPOS_SYSTEM_FONTNUMBER_Y, 20 * 10, 30, scale, xalign, yalign)

// 描画
void e3dNumberDraw(struct e3dNumber *this, struct engineMathMatrix44 *mat, int number);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 変形構造体
struct e3dTrans{
	int *texture;
	int *vertVBO;
	int *clorVBO;
	int *texcVBO;
	int *faceIBO;
	double scale[3];
	struct engineMathMatrix44 rotate;
	double translate[3];
	double color[4];
	struct e3dTrans *parent;
	bool isScale;
	bool isRotate;
	bool isTranslate;
	// リスト描画情報
	bool isExist;
	bool isVisible;
	int zIndex;
	void (*draw)(struct e3dTrans *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a);
};

void e3dTransInit(struct e3dTrans *this);
void e3dTransSetProperty(struct e3dTrans *this, struct e3dTrans *parent, int zIndex);
void e3dTransSetBuffer(struct e3dTrans *this, int *texture, int *vertVBO, int *clorVBO, int *texcVBO, int *faceIBO);
void e3dTransSetScale(struct e3dTrans *this, double x, double y, double z);
void e3dTransSetRotate(struct e3dTrans *this, double x, double y, double z);
void e3dTransSetTranslate(struct e3dTrans *this, double x, double y, double z);
void e3dTransSetColor(struct e3dTrans *this, double r, double g, double b, double a);

void e3dTransMultMatrix(struct e3dTrans *this, struct engineMathMatrix44 *mat);

void e3dTransBindBuffer(struct e3dTrans *this);
void e3dTransBindColor(struct e3dTrans *this, double r, double g, double b, double a);
void e3dTransBind(struct e3dTrans *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a);

// ----------------------------------------------------------------

// 変形管理構造体
struct e3dTransListManager{
	struct e3dTrans **transList;
	int transLength;
};

void e3dTransListManagerReset(struct e3dTransListManager *this);
void e3dTransListManagerPush(struct e3dTransListManager *this, struct e3dTrans *trans);
void e3dTransListManagerDraw(struct e3dTransListManager *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a);
void e3dTransListManagerDispose(struct e3dTransListManager *this);

void e3dTransListManagerPushProperty(struct e3dTransListManager *this, struct e3dTrans *trans, struct e3dTrans *parent, int zIndex);

// ----------------------------------------------------------------

// 変形描画構造体
struct e3dTransImage{
	struct e3dTrans super;
	struct e3dImage image;
};

void e3dTransImageInit(struct e3dTransImage *this);
void e3dTransImageDraw(struct e3dTransImage *this, struct engineMathMatrix44 *mat, double r, double g, double b, double a);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

