#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static struct{
	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;
	int x;
	int y;
	int w;
	int h;
	bool isActive;
} localGlobal = {0};

// ----------------------------------------------------------------

// メモリートレースボタン計算
void partsButtonMemoryCalcImplement(){
	struct engineCtrlTouch *t = engineCtrlTouchGet(ENGINECTRLTOUCHID_BUTTON);
	if(t == NULL){
		// タッチ無し
		localGlobal.isActive = false;
	}else if(t->dn){
		// ボタン押下中
		double x0 = localGlobal.x - localGlobal.w * 0.5;
		double y0 = localGlobal.y - localGlobal.h * 0.5;
		double x1 = x0 + localGlobal.w;
		double y1 = y0 + localGlobal.h;
		localGlobal.isActive = (x0 < t->screen.x && t->screen.x < x1 && y0 < t->screen.y && t->screen.y < y1);

		if(!t->active && localGlobal.isActive){
			// タッチ開始時
			engineCtrlTouchOwn();
		}
	}else{
		// タッチ終了時
		engineCtrlTouchFree();

		if(localGlobal.isActive){
			// ボタンを離した瞬間
			localGlobal.isActive = false;

			// メモリートレース
			engineUtilMemoryTrace("partsButtonMemory");
		}
	}
}

// バッファ作成
static void createBuffer(){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(localGlobal.bufferStatus != status || localGlobal.bufferScreen != screen){
		localGlobal.bufferStatus = status;
		localGlobal.bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		int w = localGlobal.w = 30;
		int h = localGlobal.h = 30;
		int x = (localGlobal.x = 20) - (int)(w * 0.5);
		int y = (localGlobal.y = 20) - (int)(h * 0.5);
		e3dImageBoxCreateArray(NULL, x, y, w, h, TEXPOS_SYSTEM_BUTTONBASICNORMAL_XYWH);
		e3dImageBoxCreateArray(NULL, x, y, w, h, TEXPOS_SYSTEM_BUTTONBASICACTIVE_XYWH);
		e3dImageTextCreateArray(NULL, localGlobal.x, localGlobal.y, "M", 1.0, 0, 0);

		// バッファ作成完了
		engineGraphicBufferEnd(&localGlobal.e3dIdVert, NULL, &localGlobal.e3dIdTexc, &localGlobal.e3dIdFace);
	}
}

// メモリートレースボタン描画
void partsButtonMemoryDrawImplement(){
	createBuffer();
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
	engineGraphicEngineBindTexture(e3dIdSystemImageLinear);
	engineGraphicEngineBindVertVBO(localGlobal.e3dIdVert);
	engineGraphicEngineBindTexcVBO(localGlobal.e3dIdTexc);
	engineGraphicEngineBindFaceIBO(localGlobal.e3dIdFace);
	// 
	engineGraphicEngineSetMatrix(&e3dMatOrtho);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	engineGraphicEngineDrawIndex((18 * (localGlobal.isActive ? 1 : 0)) * 3, 18 * 3);
	// 
	struct engineMathMatrix44 tempmat;
	engineMathMat4Copy(&tempmat, &e3dMatOrtho);
	engineMathMat4Translate(&tempmat, 0, (localGlobal.isActive ? 1 : -1), 0);
	engineGraphicEngineSetMatrix(&tempmat);
	engineGraphicEngineSetColorRgba(0.0, 0.0, 0.0, 1.0);
	engineGraphicEngineDrawIndex((18 * 2) * 3, 2 * 3);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

