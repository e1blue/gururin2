#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ポップアップ初期化
static void initCartridge(struct popupCartridge *cartridge){
	cartridge->init(cartridge);
	cartridge->openStep = cartridge->openStepMax;
	cartridge->isOpening = true;
	cartridge->isClosing = false;
	cartridge->openParam = 1.0;
	cartridge->openSize = 1.5;
	cartridge->openAlpha = 0.0;
	cartridge->backParam = cartridge->isContinuousOpen ? 0.0 : 1.0;
	cartridge->backAlpha = cartridge->isContinuousOpen ? 1.0 : 0.0;
}

// ポップアップ展開計算
static bool calcOpenCartridge(struct popupCartridge *cartridge, void *parent){
	if(cartridge->openStepMax > 0){
		// 開遷移開始確認
		bool isOpenStart = (cartridge->openStep >= cartridge->openStepMax);
		if(isOpenStart){cartridge->openStep = -cartridge->openStepMax;}
		// 遷移処理
		if(cartridge->openStep != 0){cartridge->openStep++;}
		// 通常処理
		cartridge->calc(cartridge);
		// 閉遷移開始確認
		bool isCloseStart = (cartridge->openStep <= 0 && !cartridge->exist);
		if(isCloseStart && cartridge->onClose != NULL){cartridge->onClose(parent, cartridge);}
		if(isCloseStart && cartridge->openStep == 0){cartridge->openStep = 1;}
		if(isCloseStart && cartridge->openStep < 0){cartridge->openStep *= -1;}
		// 遷移描画状態計算
		double param = (double)cartridge->openStep / cartridge->openStepMax;
		cartridge->isOpening = (cartridge->openStep < 0 || cartridge->openStepMax <= cartridge->openStep);
		cartridge->isClosing = (0 < cartridge->openStep && cartridge->openStep < cartridge->openStepMax);
		cartridge->openParam = param;
		cartridge->openSize = 1 + 0.5 * param * param;
		cartridge->openAlpha = 1 - param * param;
		bool isContinuous = ((cartridge->isOpening && cartridge->isContinuousOpen) || (cartridge->isClosing && cartridge->isContinuousClose));
		cartridge->backParam = isContinuous ? 0.0 : param;
		cartridge->backAlpha = (isContinuous ? 1.0 : cartridge->openAlpha) * 0.5;
		// 後遷移完了確認
		bool isCloseEnd = (cartridge->openStep >= cartridge->openStepMax);
		return !isCloseEnd;
	}else{
		// 遷移無し
		cartridge->calc(cartridge);
		bool isClose = !cartridge->exist;
		if(isClose && cartridge->onClose != NULL){cartridge->onClose(parent, cartridge);}
		return !isClose;
	}
}

// ポップアップカートリッジ計算
bool popupManagerCalc(struct popupManager *this, void *parent){
	struct popupCartridge *cartridge = this->popupCartridgeList;
	while(cartridge != NULL){
		bool exist = calcOpenCartridge(cartridge, parent);
		if(exist){break;}

		this->popupCartridgeList = cartridge->next;
		cartridge->pause(cartridge);
		cartridge->dispose(cartridge);

		cartridge = this->popupCartridgeList;
		if(cartridge == NULL){break;}
		initCartridge(cartridge);
	}

	return (cartridge != NULL);
}

// ポップアップカートリッジ描画
void popupManagerDraw(struct popupManager *this){
	struct popupCartridge *cartridge = this->popupCartridgeList;
	if(cartridge != NULL){cartridge->draw(cartridge);}
}

// ポップアップカートリッジ一時停止処理
void popupManagerPause(struct popupManager *this){
	struct popupCartridge *cartridge = this->popupCartridgeList;
	if(cartridge != NULL){cartridge->pause(cartridge);}
}

// ポップアップカートリッジ終了処理
void popupManagerDispose(struct popupManager *this){
	while(this->popupCartridgeList != NULL){
		struct popupCartridge *cartridge = this->popupCartridgeList;
		this->popupCartridgeList = cartridge->next;
		cartridge->dispose(cartridge);
	}
}

// ----------------------------------------------------------------

// ポップアップカートリッジ装填
void popupManagerPush(struct popupManager *this, struct popupCartridge *cartridge, void(*onClose)(void*, struct popupCartridge*)){
	cartridge->next = NULL;
	cartridge->openStepMax = 10;
	cartridge->onClose = onClose;

	if(this->popupCartridgeList == NULL){
		this->popupCartridgeList = cartridge;
		initCartridge(cartridge);
	}else{
		struct popupCartridge *temp = this->popupCartridgeList;
		while(temp->next != NULL){temp = temp->next;}
		temp->next = cartridge;
		temp->isContinuousClose = true;
		cartridge->isContinuousOpen = true;
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

