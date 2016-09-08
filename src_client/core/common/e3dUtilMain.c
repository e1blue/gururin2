#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// システム画像
engineGraphicObjectTexId e3dIdSystemImageLinear = 0;
engineGraphicObjectTexId e3dIdSystemImageNearest = 0;

// 行列データ保管場所
struct engineMathMatrix44 e3dMatOrtho;
struct engineMathMatrix44 e3dMatFrustum;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// エンジンユーティリティ初期化
void e3dUtilGameInit(){
	if(e3dIdSystemImageLinear <= 0){e3dIdSystemImageLinear = engineGraphicObjectTexCreate("img/system.png", ENGINEGRAPHICOBJECTTEXTYPE_LINEAR);}
	if(e3dIdSystemImageNearest <= 0){e3dIdSystemImageNearest = engineGraphicObjectTexCreate("img/system.png", ENGINEGRAPHICOBJECTTEXTYPE_NEAREST);}
}

// エンジンユーティリティ終了処理
void e3dUtilGameFinal(){
	engineGraphicObjectTexDispose(e3dIdSystemImageLinear);
	engineGraphicObjectTexDispose(e3dIdSystemImageNearest);
	e3dIdSystemImageLinear = 0;
	e3dIdSystemImageNearest = 0;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

