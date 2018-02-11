#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// システム画像
engineGraphicTextureId e3dIdSystemImageLinear = 0;
engineGraphicTextureId e3dIdSystemImageNearest = 0;

// 行列データ保管場所
struct engineMathMatrix44 e3dMatOrtho;
struct engineMathMatrix44 e3dMatFrustum;

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// エンジンユーティリティ初期化
void e3dUtilGameInit(){
	if(e3dIdSystemImageLinear <= 0){e3dIdSystemImageLinear = engineGraphicTextureCreateLocal("img/system.png", ENGINEGRAPHICTEXTURETYPE_LINEAR);}
	if(e3dIdSystemImageNearest <= 0){e3dIdSystemImageNearest = engineGraphicTextureCreateLocal("img/system.png", ENGINEGRAPHICTEXTURETYPE_NEAREST);}
}

// エンジンユーティリティ終了処理
void e3dUtilGameFinal(){
	engineGraphicTextureDispose(e3dIdSystemImageLinear);
	engineGraphicTextureDispose(e3dIdSystemImageNearest);
	e3dIdSystemImageLinear = 0;
	e3dIdSystemImageNearest = 0;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

