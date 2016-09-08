#include "../library.h"
#include "../includeAll.h"
#include "pageMenu.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページカートリッジ構造体
struct pageCartridgeMenu{
	struct pageCartridge super;
	struct pageMenuStatus *stat;

	struct e3dTrans transRoot;
	struct e3dTrans transBack;
	struct e3dTransImage imgPool[4];
	struct e3dTransListManager transManager;

	struct e3dImage imgMask;

	int bufferStatus;
	int bufferScreen;
	int e3dIdVert;
	int e3dIdTexc;
	int e3dIdFace;

	int reqIdProgress;
	int step;
};

// ----------------------------------------------------------------

// 読み込み開始
static void load(struct pageCartridgeMenu *this){
	engineDataRequestFreeAll();
	this->reqIdProgress = engineDataRequestLocal("dat/progress.json");
}

// 読み込み完了
static int loaded(struct pageCartridgeMenu *this){
	// 読み込み確認
	size_t size;
	byte *loadedProgress;
	bool isErrorProgress = !engineDataRequestResponse(this->reqIdProgress, (void**)&loadedProgress, &size);
	isErrorProgress = isErrorProgress || (loadedProgress == NULL);
	if(isErrorProgress){return PARTSPOPUPLOADING_LOADED_RETRY;}

	// 読み込み成功 json化
	struct engineDataJsonValue jsonProgress;
	engineDataJsonParse(&jsonProgress, loadedProgress, size);
	engineDataRequestFree(this->reqIdProgress);

	// 進行度読み込み
	storageProgressGet(&this->stat->progress);

	// ステージストーリーデータの数を数える
	struct engineDataJsonValue *jsonSelectList = engineDataJsonObjectGetValue(&jsonProgress, "selectList");
	this->stat->selectLength = engineDataJsonArrayGetLength(jsonSelectList);
	this->stat->selectList = (struct pageMenuStatusSelect*)engineUtilMemoryCalloc(this->stat->selectLength, sizeof(struct pageMenuStatusSelect));
	// ステージストーリーデータ作成
	int index = 0;
	int selectIndex = 0;
	int stageIndex = 0;
	int storyIndex = 0;
	int totalStar = 0;
	struct engineDataJsonArray *selectArr = (jsonSelectList->type == ENGINEDATAJSONTYPE_ARRAY) ? jsonSelectList->jArray : NULL;
	while(selectArr != NULL){
		struct pageMenuStatusSelect *select = &this->stat->selectList[index];
		select->index = selectIndex;

		char *type = engineDataJsonGetString(engineDataJsonObjectGetValue(&selectArr->value, "type"), NULL);
		if(!strcmp(type, "stage")){
			// ステージデータ作成
			if(this->stat->progress.openStage[stageIndex] & 0x01){
				select->isStage = true;
				select->stage.index = stageIndex;
				select->stage.id = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "id"), 1);
				select->stage.icon = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "icon"), stageIndex);
				select->stage.mark = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "mark"), stageIndex);
				storageStageGet(&select->stage.storage, select->stage.id);
				totalStar += (int)utilMathFloor(select->stage.storage.score / 1000);
				index++;
			}
			stageIndex++;
		}else if(!strcmp(type, "story")){
			// ストーリーデータ作成
			if(this->stat->progress.openStory[storyIndex] & 0x01){
				select->isStage = false;
				select->story.index = storyIndex;
				select->story.id = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "id"), 1);
				select->story.icon = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&selectArr->value, "icon"), storyIndex);
				index++;
			}
			storyIndex++;
		}

		selectIndex++;
		selectArr = selectArr->next;
	}
	this->stat->selectLength = index;

	// 総合獲得星数がずれていたら調整
	if((int)this->stat->progress.totalStar != totalStar){
		this->stat->progress.totalStar = totalStar;
		storageProgressSet(&this->stat->progress);
		storageCommit();
	}

	// パペットデータの数を数える
	struct engineDataJsonValue *jsonPuppetList = engineDataJsonObjectGetValue(&jsonProgress, "puppetList");
	this->stat->puppetLength = engineDataJsonArrayGetLength(jsonPuppetList);
	this->stat->puppetList = (struct pageMenuStatusPuppet*)engineUtilMemoryCalloc(this->stat->puppetLength, sizeof(struct pageMenuStatusPuppet));
	// パペットデータ作成
	int puppetIndex = 0;
	struct engineDataJsonArray *puppetArr = (jsonPuppetList->type == ENGINEDATAJSONTYPE_ARRAY) ? jsonPuppetList->jArray : NULL;
	while(puppetArr != NULL){
		struct pageMenuStatusPuppet *puppet = &this->stat->puppetList[puppetIndex];
		puppet->kind = hknwCharaConvertKind(engineDataJsonGetString(engineDataJsonObjectGetValue(&puppetArr->value, "kind"), NULL));
		puppet->isActive = (this->stat->progress.openPuppet[puppetIndex] & 0x01);
		puppet->openStar = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&puppetArr->value, "openStar"), 0);
		puppetIndex++;
		puppetArr = puppetArr->next;
	}
	this->stat->puppetLength = puppetIndex;

	// jsonの解放
	//engineDataJsonTrace(&jsonProgress);
	engineDataJsonFree(&jsonProgress);
	return PARTSPOPUPLOADING_LOADED_OK;
}

// ----------------------------------------------------------------

// 初期化
static void init(struct pageCartridgeMenu *this){
	// BGM設定
	soundBgmPlayMenu();
}

// ----------------------------------------------------------------

// 計算
static void calc(struct pageCartridgeMenu *this){
	// メモリートレースボタン計算
	partsButtonMemoryCalc();

	this->step++;

	// ポップアップ処理
	if(popupManagerCalc(&this->stat->popup, this)){return;}

	// ポップアップが尽きたらページ遷移
	this->super.exist = false;
}

// ----------------------------------------------------------------

// バッファ作成
static void createBuffer(struct pageCartridgeMenu *this){
	int status = 1;
	int screen = (global.screen.w & 0xffff) | ((global.screen.h & 0xffff) << 16);

	if(this->bufferStatus != status || this->bufferScreen != screen){
		this->bufferStatus = status;
		this->bufferScreen = screen;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// 親の変形描画構造体設定
		e3dTransInit(&this->transRoot);
		e3dTransInit(&this->transBack);
		e3dTransSetBuffer(&this->transRoot, &e3dIdSystemImageLinear, &this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
		this->transBack.parent = &this->transRoot;
		// 変形描画構造体登録準備
		int imgPoolIndex = 0;
		struct e3dTransImage *transImage;
		e3dTransListManagerReset(&this->transManager);

		// 表示マスク作成
		e3dImageWhiteCreateArray(&this->imgMask, 0, 0, global.screen.w, global.screen.h);

		// 壁紙作成
		e3dTransImageInit(transImage = &this->imgPool[imgPoolIndex++]);
		e3dTransListManagerPushProperty(&this->transManager, &transImage->super, &this->transBack, 0);
		partsWallpaperCreateArray(&transImage->image, 82, NULL, NULL);

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdVert, NULL, &this->e3dIdTexc, &this->e3dIdFace);
	}
}

// 描画
static void draw(struct pageCartridgeMenu *this){
	engineGraphicEngineClearAll();

	// 描画準備
	createBuffer(this);
	hknwCharaManagerCreateBuffer(&this->stat->charaManager);

	// 射影行列作成
	engineMathMat4Ortho(&e3dMatOrtho, -global.screen.offset.x, global.screen.w + global.screen.offset.x, global.screen.h + global.screen.offset.y, -global.screen.offset.y, -1, 1);

	// 2D描画準備
	engineGraphicEngineSetDrawMode(ENGINEGRAPHICENGINEMODEDRAW_2D);
	engineGraphicEngineBindVertVBO(this->e3dIdVert);
	engineGraphicEngineBindTexcVBO(this->e3dIdTexc);
	engineGraphicEngineBindFaceIBO(this->e3dIdFace);
	// 全体マスク作成
	engineGraphicEngineSetMatrix(&e3dMatOrtho);
	engineGraphicEngineSetStencilMode(ENGINEGRAPHICENGINEMODESTENCIL_MASK_1);
	e3dImageDraw(&this->imgMask);
	engineGraphicEngineSetStencilMode(ENGINEGRAPHICENGINEMODESTENCIL_READ_EQ1);

	// リスト設定
	e3dTransSetTranslate(&this->transBack, -((this->step / 2) % 32), -((this->step / 8) % 32), 0.0);

	// リスト描画
	e3dTransListManagerDraw(&this->transManager, &e3dMatOrtho, 1.0, 1.0 ,1.0, 1.0);

	// ポップアップ描画
	popupManagerDraw(&this->stat->popup);

	// ステンシルマスク解除
	engineGraphicEngineSetStencilMode(ENGINEGRAPHICENGINEMODESTENCIL_NONE);

	// メモリートレースボタン描画
	partsButtonMemoryDraw();

	engineGraphicEngineFlush();
}

// ----------------------------------------------------------------

// 一時停止
static void pause(struct pageCartridgeMenu *this){
	popupManagerPause(&this->stat->popup);
}

// 破棄
static void dispose(struct pageCartridgeMenu *this){
	popupManagerDispose(&this->stat->popup);
	hknwCharaManagerDispose(&this->stat->charaManager);
	e3dTransListManagerDispose(&this->transManager);
	engineGraphicObjectVBODispose(this->e3dIdVert);
	engineGraphicObjectVBODispose(this->e3dIdTexc);
	engineGraphicObjectIBODispose(this->e3dIdFace);
	engineUtilMemoryFree(this->stat->selectList);
	engineUtilMemoryFree(this->stat->puppetList);
	engineUtilMemoryFree(this->stat);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------

// ページカートリッジ作成
static struct pageCartridge *cartridgeMenu(struct pageMenuStatus *stat){
	struct pageCartridgeMenu *this = (struct pageCartridgeMenu*)engineUtilMemoryCalloc(1, sizeof(struct pageCartridgeMenu));
	this->super.exist = true;
	this->stat = stat;

	// 読み込み開始 TODO initに移行したい popupMenuSelectOpenとの順番の問題
	popupManagerPush(&stat->popup, partsPopupLoading(this, (void(*)(void*))load, (int(*)(void*))loaded, NULL), NULL);

	struct pageCartridge *cartridge = (struct pageCartridge*)this;
	cartridge->init = (void(*)(struct pageCartridge*))init;
	cartridge->calc = (void(*)(struct pageCartridge*))calc;
	cartridge->draw = (void(*)(struct pageCartridge*))draw;
	cartridge->pause = (void(*)(struct pageCartridge*))pause;
	cartridge->dispose = (void(*)(struct pageCartridge*))dispose;
	return cartridge;
}

// ページ状態初期化 タイトルポップアップ
void pageMenuInitTitle(){
	if(utilEditedExist() || utilReplayExist()){
		// 編集ステージやリプレイモードではメニューを表示しない
		pageRollInit(0, false);
	}else{
		// 通常モード
		struct pageMenuStatus *stat = (struct pageMenuStatus*)engineUtilMemoryCalloc(1, sizeof(struct pageMenuStatus));
		struct pageCartridge *cartridge = cartridgeMenu(stat);
		popupMenuTitleOpen(stat);
		pageCartridgePush(cartridge);
	}
}

// ページ状態初期化 選択ポップアップ
void pageMenuInitSelect(){
	if(utilEditedExist() || utilReplayExist()){
		// 編集ステージやリプレイモードではメニューを表示しない
		pageRollInit(0, false);
	}else{
		// 通常モード
		struct pageMenuStatus *stat = (struct pageMenuStatus*)engineUtilMemoryCalloc(1, sizeof(struct pageMenuStatus));
		struct pageCartridge *cartridge = cartridgeMenu(stat);
		popupMenuSelectOpen(stat);
		pageCartridgePush(cartridge);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

