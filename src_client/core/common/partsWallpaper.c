#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static void setTexpos(int xin, int yin, int win, int hin, int *xout, int *yout, int *wout, int *hout){
	*xout = xin;
	*yout = yin;
	*wout = win;
	*hout = hin;
}

// 配列に移動する壁紙の描画情報を追加
void partsWallpaperCreateArray(struct e3dImage *image, int type, int *width, int *height){
	int x, y, w, h;
	switch(type){
		case 11: setTexpos(TEXPOS_SYSTEM_WALLPAPER11_XYWH, &x, &y, &w, &h); break;
		case 12: setTexpos(TEXPOS_SYSTEM_WALLPAPER12_XYWH, &x, &y, &w, &h); break;
		case 21: setTexpos(TEXPOS_SYSTEM_WALLPAPER21_XYWH, &x, &y, &w, &h); break;
		case 22: setTexpos(TEXPOS_SYSTEM_WALLPAPER22_XYWH, &x, &y, &w, &h); break;
		case 31: setTexpos(TEXPOS_SYSTEM_WALLPAPER31_XYWH, &x, &y, &w, &h); break;
		case 32: setTexpos(TEXPOS_SYSTEM_WALLPAPER32_XYWH, &x, &y, &w, &h); break;
		case 41: setTexpos(TEXPOS_SYSTEM_WALLPAPER41_XYWH, &x, &y, &w, &h); break;
		case 42: setTexpos(TEXPOS_SYSTEM_WALLPAPER42_XYWH, &x, &y, &w, &h); break;
		case 51: setTexpos(TEXPOS_SYSTEM_WALLPAPER51_XYWH, &x, &y, &w, &h); break;
		case 52: setTexpos(TEXPOS_SYSTEM_WALLPAPER52_XYWH, &x, &y, &w, &h); break;
		case 61: setTexpos(TEXPOS_SYSTEM_WALLPAPER61_XYWH, &x, &y, &w, &h); break;
		case 62: setTexpos(TEXPOS_SYSTEM_WALLPAPER62_XYWH, &x, &y, &w, &h); break;
		case 71: setTexpos(TEXPOS_SYSTEM_WALLPAPER71_XYWH, &x, &y, &w, &h); break;
		case 72: setTexpos(TEXPOS_SYSTEM_WALLPAPER72_XYWH, &x, &y, &w, &h); break;
		case 81: setTexpos(TEXPOS_SYSTEM_WALLPAPER81_XYWH, &x, &y, &w, &h); break;
		case 82: setTexpos(TEXPOS_SYSTEM_WALLPAPER82_XYWH, &x, &y, &w, &h); break;
		default: setTexpos(TEXPOS_SYSTEM_TEST_XYWH, &x, &y, &w, &h); break;
	}
	int tu = x + 1;
	int tv = y + 1;
	int tw = w - 2;
	int th = h - 2;
	int xsize = utilMathCeil((double)global.screen.w / tw) + 1;
	int ysize = utilMathCeil((double)global.screen.h / th) + 1;
	if(width != NULL){*width = tw;}
	if(height != NULL){*height = tw;}

	int faceIndex = engineGraphicBufferFaceIndexGet();
	for(int i = 0; i < xsize; i++){
		for(int j = 0; j < ysize; j++){
			e3dImageCreateArray(NULL, TEXSIZ_SYSTEM_WH, tw * i, th * j, tw, th, tu, tv, tw, th);
		}
	}
	image->faceIndex = faceIndex;
	image->faceNum = xsize * ysize * 2;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

