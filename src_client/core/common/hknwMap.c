#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// マップチップの範囲外チェック
static int hknwMapGetChip(struct hknwMap *this, int x, int y, int z){
	if(x < 0 || this->xsize <= x){return -1;}
	if(y < 0 || this->ysize <= y){return -1;}
	if(z < 0 || this->zsize <= z){return -1;}
	return *(this->mapchip + x + this->xsize * (y + this->ysize * z));
}

// マップチップの可視チェック
static int hknwMapIsChipVisible(struct hknwMap *this, int x, int y, int z){
	int chip = hknwMapGetChip(this, x, y, z);
	return chip > 0;
}

// マップチップの透過チェック
static int hknwMapIsChipSkeleton(struct hknwMap *this, int x, int y, int z){
	int chip = hknwMapGetChip(this, x, y, z);
	return chip != -1 && chip != 0 && chip != 1 && !(57 <= chip && chip <= 64);
}

//// マップチップの衝突チェック
//static int hknwMapIsChipHit(struct hknwMap *this, int x, int y, int z){
//	int chip = hknwMapGetChip(this, x, y, z);
//	return chip != 0;
//}

// ----------------------------------------------------------------

// 影の確認関数
static void hknwMapInitSetTopColor(struct hknwMap *this, int x1, int y1, int z1, int x2, int y2, int z2, int x3, int y3, int z3){
	double color = 1.0;
	if(hknwMapIsChipSkeleton(this, x1, y1, z1)){color -= 0.2;}
	if(hknwMapIsChipSkeleton(this, x2, y2, z2)){color -= 0.2;}
	if(hknwMapIsChipSkeleton(this, x3, y3, z3)){color -= 0.2;}
	engineGraphicBufferPushClor(color, color, color);
}

// 面の頂点配列作成関数
static void hknwMapInitSetSurfaces(struct hknwMap *this, int x0, int y0, int z0, int s){
	int chip = hknwMapGetChip(this, x0, y0, z0) - 1;
	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;
	int xm = x0 - 1;
	int ym = y0 - 1;
	int zm = z0 - 1;
	int u = TEXPOS_SYSTEM_IMAGEMAPCHIP_X + (chip % 4) * 16 * 4;
	int v = TEXPOS_SYSTEM_IMAGEMAPCHIP_Y + utilMathFloor(chip / 4.0) * 16;
	engineGraphicBufferPushTetraFace(engineGraphicBufferVretIndexGet());
	switch(s){
		case 1:
			u += 16 * 0;
			engineGraphicBufferPushVert(x0, z1, y0); hknwMapInitSetTopColor(this, x0, ym, z1, xm, y0, z1, xm, ym, z1);
			engineGraphicBufferPushVert(x0, z1, y1); hknwMapInitSetTopColor(this, x0, y1, z1, xm, y0, z1, xm, y1, z1);
			engineGraphicBufferPushVert(x1, z1, y1); hknwMapInitSetTopColor(this, x0, y1, z1, x1, y0, z1, x1, y1, z1);
			engineGraphicBufferPushVert(x1, z1, y0); hknwMapInitSetTopColor(this, x0, ym, z1, x1, y0, z1, x1, ym, z1);
			break;
		case 2:
			u += 16 * 3;
			engineGraphicBufferPushVert(x0, z0, y1); hknwMapInitSetTopColor(this, x0, y1, zm, xm, y0, zm, xm, y1, zm);
			engineGraphicBufferPushVert(x0, z0, y0); hknwMapInitSetTopColor(this, x0, ym, zm, xm, y0, zm, xm, ym, zm);
			engineGraphicBufferPushVert(x1, z0, y0); hknwMapInitSetTopColor(this, x0, ym, zm, x1, y0, zm, x1, ym, zm);
			engineGraphicBufferPushVert(x1, z0, y1); hknwMapInitSetTopColor(this, x0, y1, zm, x1, y0, zm, x1, y1, zm);
			break;
		case 3:
			u += 16 * 1;
			engineGraphicBufferPushVert(x0, z1, y1); hknwMapInitSetTopColor(this, x0, y1, z1, xm, y1, z0, xm, y1, z1);
			engineGraphicBufferPushVert(x0, z0, y1); hknwMapInitSetTopColor(this, x0, y1, zm, xm, y1, z0, xm, y1, zm);
			engineGraphicBufferPushVert(x1, z0, y1); hknwMapInitSetTopColor(this, x0, y1, zm, x1, y1, z0, x1, y1, zm);
			engineGraphicBufferPushVert(x1, z1, y1); hknwMapInitSetTopColor(this, x0, y1, z1, x1, y1, z0, x1, y1, z1);
			break;
		case 4:
			u += 16 * 1;
			engineGraphicBufferPushVert(x1, z1, y0); hknwMapInitSetTopColor(this, x0, ym, z1, x1, ym, z0, x1, ym, z1);
			engineGraphicBufferPushVert(x1, z0, y0); hknwMapInitSetTopColor(this, x0, ym, zm, x1, ym, z0, x1, ym, zm);
			engineGraphicBufferPushVert(x0, z0, y0); hknwMapInitSetTopColor(this, x0, ym, zm, xm, ym, z0, xm, ym, zm);
			engineGraphicBufferPushVert(x0, z1, y0); hknwMapInitSetTopColor(this, x0, ym, z1, xm, ym, z0, xm, ym, z1);
			break;
		case 5:
			u += 16 * 2;
			engineGraphicBufferPushVert(x1, z1, y1); hknwMapInitSetTopColor(this, x1, y0, z1, x1, y1, z0, x1, y1, z1);
			engineGraphicBufferPushVert(x1, z0, y1); hknwMapInitSetTopColor(this, x1, y0, zm, x1, y1, z0, x1, y1, zm);
			engineGraphicBufferPushVert(x1, z0, y0); hknwMapInitSetTopColor(this, x1, y0, zm, x1, ym, z0, x1, ym, zm);
			engineGraphicBufferPushVert(x1, z1, y0); hknwMapInitSetTopColor(this, x1, y0, z1, x1, ym, z0, x1, ym, z1);
			break;
		case 6:
			u += 16 * 2;
			engineGraphicBufferPushVert(x0, z1, y0); hknwMapInitSetTopColor(this, xm, y0, z1, xm, ym, z0, xm, ym, z1);
			engineGraphicBufferPushVert(x0, z0, y0); hknwMapInitSetTopColor(this, xm, y0, zm, xm, ym, z0, xm, ym, zm);
			engineGraphicBufferPushVert(x0, z0, y1); hknwMapInitSetTopColor(this, xm, y0, zm, xm, y1, z0, xm, y1, zm);
			engineGraphicBufferPushVert(x0, z1, y1); hknwMapInitSetTopColor(this, xm, y0, z1, xm, y1, z0, xm, y1, z1);
			break;
	}
	engineGraphicBufferPushTetraTexc(TEXSIZ_SYSTEM_WH, u, v, 16, 16);
	this->faceNum += 2;
}

// 初期化
struct hknwMap *hknwMapInit(int xsize, int ysize, int zsize, int *mapchip){
	struct hknwMap *this = (struct hknwMap*)engineUtilMemoryCalloc(1, sizeof(struct hknwMap));
	this->xsize = xsize;
	this->ysize = ysize;
	this->zsize = zsize;

	int size = sizeof(int) * this->xsize * this->ysize * this->zsize;
	this->mapchip = (int*)engineUtilMemoryMalloc(size);
	memcpy(this->mapchip, mapchip, size);

	// バッファ作成開始
	engineGraphicBufferBegin();
	this->faceNum = 0;

	// 頂点データを生成
	for(int i = 0; i < this->xsize; i++){
		for(int j = 0; j < this->ysize; j++){
			for(int k = 0; k < this->zsize; k++){
				if(hknwMapIsChipVisible(this, i, j, k)){
					if(!hknwMapIsChipSkeleton(this, i, j, k + 1)){hknwMapInitSetSurfaces(this, i, j, k, 1);}
					if(!hknwMapIsChipSkeleton(this, i, j, k - 1)){hknwMapInitSetSurfaces(this, i, j, k, 2);}
					if(!hknwMapIsChipSkeleton(this, i, j + 1, k)){hknwMapInitSetSurfaces(this, i, j, k, 3);}
					if(!hknwMapIsChipSkeleton(this, i, j - 1, k)){hknwMapInitSetSurfaces(this, i, j, k, 4);}
					if(!hknwMapIsChipSkeleton(this, i + 1, j, k)){hknwMapInitSetSurfaces(this, i, j, k, 5);}
					if(!hknwMapIsChipSkeleton(this, i - 1, j, k)){hknwMapInitSetSurfaces(this, i, j, k, 6);}
				}
			}
		}
	}

	// バッファ作成完了
	engineGraphicBufferEnd(&this->e3dIdVert, &this->e3dIdClor, &this->e3dIdTexc, &this->e3dIdFace);

	return this;
}

// ----------------------------------------------------------------

// 描画
void hknwMapDraw(struct hknwMap *this, struct engineMathMatrix44 *mat){
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_HKNW);
	engineGraphicEngineBindVertVBO(this->e3dIdVert);
	engineGraphicEngineBindClorVBO(this->e3dIdClor);
	engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
	engineGraphicEngineBindFaceIBO(this->e3dIdFace);
	engineGraphicEngineSetMatrix(mat);
	engineGraphicEngineSetColorRgba(1.0, 1.0, 1.0, 1.0);
	engineGraphicEngineDrawIndex(0 * 3, this->faceNum * 3);
}

// 破棄
void hknwMapDispose(struct hknwMap *this){
	if(this == NULL){return;}
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdClor);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->mapchip);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

