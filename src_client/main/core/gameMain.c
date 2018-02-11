#include "includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// opengl初期化
void gameMainSurfaceCreated(void){
	engineGraphicGameInit();
	e3dUtilGameInit();
	utilGameInit();
	soundGameInit();
	pageCartridgeGameInit();
}

// opengl描画
void gameMainSurfaceDrawFrame(void){
	engineCtrlTouchCalc();
	pageCartridgeGameCalc();
}

// opengl中断
void gameMainSurfacePause(void){
	pageCartridgeGamePause();
}

// opengl終了
void gameMainSurfaceDestroy(void){
	pageCartridgeGameFinal();
	e3dUtilGameFinal();
	engineGraphicGameExit();
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 戻るボタンの制御を行うか
bool gameMainEventKeyIsBack(void){return !platformReplayExist();}

// 傾きを使うかどうか
bool gameMainEventIsAcceleration(void){return true;}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

