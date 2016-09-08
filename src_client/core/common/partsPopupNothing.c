#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップカートリッジ構造体
struct partsPopupNothing{
	struct popupCartridge super;
};

// ----------------------------------------------------------------

// 初期化
static void init(struct partsPopupNothing *this){
	this->super.openStepMax = 0;
}

// ----------------------------------------------------------------

// 計算
static void calc(struct partsPopupNothing *this){
	this->super.exist = false;
}

// ----------------------------------------------------------------

// 描画
static void draw(struct partsPopupNothing *this){
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct partsPopupNothing *this){
}

// 破棄
static void dispose(struct partsPopupNothing *this){
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ポップアップ作成
struct popupCartridge *partsPopupNothing(){
	struct partsPopupNothing *this = (struct partsPopupNothing*)engineUtilMemoryCalloc(1, sizeof(struct partsPopupNothing));
	this->super.exist = true;

	struct popupCartridge *cartridge = (struct popupCartridge*)this;
	cartridge->init = (void(*)(struct popupCartridge*))init;
	cartridge->calc = (void(*)(struct popupCartridge*))calc;
	cartridge->draw = (void(*)(struct popupCartridge*))draw;
	cartridge->pause = (void(*)(struct popupCartridge*))pause;
	cartridge->dispose = (void(*)(struct popupCartridge*))dispose;
	return cartridge;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

