#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジリスト
static struct pageCartridge *pageCartridgeList = NULL;
// 最初のシステム画像ロードが完了したか
static bool isLoadedSystem = false;

// ----------------------------------------------------------------

// ページカートリッジ初期化
void pageCartridgeGameInit(){
	if(pageCartridgeList == NULL){isLoadedSystem = false;}
}

// ページカートリッジメインループ計算
void pageCartridgeGameCalc(){
	if(!isLoadedSystem){
		// 最初のシステム画像ロード完了確認
		if(platformPluginUtilIsLoading()){
			// 最初のシステム画像ロード中
			engineGraphicEngineClearAll();
			engineGraphicEngineFlush();
			return;
		}
		isLoadedSystem = true;
		// 最初のページカートリッジ装填
		if(pageCartridgeList == NULL){FIRSTCARTRIDGE();}
	}

	struct pageCartridge *cartridge = pageCartridgeList;
	while(cartridge != NULL){
		cartridge->calc(cartridge);
		if(cartridge->exist){break;}

		pageCartridgeList = cartridge->next;
		cartridge->pause(cartridge);
		cartridge->dispose(cartridge);

		cartridge = pageCartridgeList;
		if(cartridge != NULL){cartridge->init(cartridge);}
	};

	// ページカートリッジ描画
	if(cartridge != NULL){cartridge->draw(cartridge);}
}

// ページカートリッジ一時停止処理
// initを通ったら必ずpauseを通る
void pageCartridgeGamePause(){
	struct pageCartridge *cartridge = pageCartridgeList;
	if(cartridge != NULL){cartridge->pause(cartridge);}
}

// ページカートリッジ終了処理
// pushをしたら必ずdisposeを通る
void pageCartridgeGameFinal(){
	while(pageCartridgeList != NULL){
		struct pageCartridge *cartridge = pageCartridgeList;
		pageCartridgeList = cartridge->next;
		cartridge->dispose(cartridge);
	}
}

// ----------------------------------------------------------------

// ページカートリッジ装填
void pageCartridgePush(struct pageCartridge *cartridge){
	if(pageCartridgeList == NULL){
		pageCartridgeList = cartridge;
		cartridge->init(cartridge);
	}else{
		struct pageCartridge *temp = pageCartridgeList;
		while(temp->next != NULL){temp = temp->next;}
		temp->next = cartridge;
	}
	cartridge->next = NULL;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

