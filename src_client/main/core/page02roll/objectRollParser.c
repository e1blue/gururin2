#include "../includeAll.h"
#include "pageRoll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// キャラクター設定
static void setupCharacter(struct pageRollStatus *this, struct pageRollStatusChara *chara, int team, enum hknwCharaKind kind, int hexx, int hexy, double scale){
	// キャラクター描画データ作成もしくは再利用
	struct hknwChara *draw = hknwCharaManagerGet(&this->charaManager, kind);

	// データ作成
	chara->team = team;
	chara->hp = 1;
	chara->kind = kind;
	chara->pose = HKNWCHARAPOSESID_FALL;
	chara->scale = scale;
	chara->rotv = CONST_PI * 0.5;
	chara->roth = CONST_PI * 0.0;
	chara->data.x = objectRollMapCoordX(this->mapList[0], hexx, hexy);
	chara->data.y = objectRollMapCoordY(this->mapList[0], hexx, hexy);
	chara->data.vx = 0;
	chara->data.vy = 0;
	chara->data.nx = utilMathCos(chara->roth - CONST_PI * 0.5);
	chara->data.ny = utilMathSin(chara->roth - CONST_PI * 0.5);
	chara->data.rad = draw->radius2d * chara->scale;
	chara->isExist = true;
}

// アイテム設定
static void setupItem(struct pageRollStatus *this, struct pageRollStatusItem *item, int type, int hexx, int hexy){
	item->type = type;
	item->x = objectRollMapCoordX(this->mapList[0], hexx, hexy);
	item->y = objectRollMapCoordY(this->mapList[0], hexx, hexy);
	item->isExist = true;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static int jsonItemType(struct engineDataJsonArray *arr, char *name){
	char *type = engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, name), NULL);
	if(!strcmp(type, "star")){return 1;}
	return 0;
}

static void jsonMapchip(struct pageRollStatus *this, struct engineDataJsonValue *val){
	// 領域確保
	this->mapLength = engineDataJsonArrayGetLength(val);
	this->mapList = (struct objectRollMap**)engineUtilMemoryCalloc(this->mapLength, sizeof(struct objectRollMap*));
	// マップ作成
	int index = 0;
	struct engineDataJsonArray *arr = (val->type == ENGINEDATAJSONTYPE_ARRAY) ? val->jArray : NULL;
	while(arr != NULL){
		struct engineDataJsonArray *arrx, *arry;
		int xsize = 64;
		int ysize = 64;
		// 数を数える
		int ytemp = 0;
		arry = (arr->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arr->value.jArray : NULL;
		while(arry != NULL){
			int xtemp = 0;
			arrx = (arry->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arry->value.jArray : NULL;
			while(arrx != NULL){
				xtemp++;
				arrx = arrx->next;
			}
			if(xsize > xtemp){xsize = xtemp;}
			ytemp++;
			arry = arry->next;
		}
		if(ysize > ytemp){ysize = ytemp;}
		// 領域確保
		int *hexChip = (int*)engineUtilMemoryMalloc(xsize * ysize * sizeof(int));
		// 要素を読み込む
		int yindex = 0;
		arry = (arr->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arr->value.jArray : NULL;
		while(arry != NULL && yindex < ysize){
			int xindex = 0;
			arrx = (arry->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arry->value.jArray : NULL;
			while(arrx != NULL && xindex < xsize){
				hexChip[xsize * yindex + xindex] = (int)engineDataJsonGetInt(&arrx->value, 0);
				xindex++;
				arrx = arrx->next;
			}
			yindex++;
			arry = arry->next;
		}
		// マップ作成
		this->mapList[index++] = objectRollMapInit(this, xsize, ysize, hexChip);
		engineUtilMemoryFree(hexChip);
		arr = arr->next;
	}
}

static void jsonCharacter(struct pageRollStatus *this, struct engineDataJsonValue *pval, struct engineDataJsonValue *eval){
	// 領域確保
	this->charaLength = engineDataJsonArrayGetLength(pval) + engineDataJsonArrayGetLength(eval);
	this->charaList = (struct pageRollStatusChara*)engineUtilMemoryCalloc(this->charaLength, sizeof(struct pageRollStatusChara));
	// キャラクター作成
	int index = 0;
	struct engineDataJsonArray *parr = (pval->type == ENGINEDATAJSONTYPE_ARRAY) ? pval->jArray : NULL;
	struct engineDataJsonArray *earr = (eval->type == ENGINEDATAJSONTYPE_ARRAY) ? eval->jArray : NULL;
	while(parr != NULL){
		int x = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&parr->value, "x"), 0);
		int y = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&parr->value, "y"), 0);
		double scale = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&parr->value, "scale"), 1.5);
		struct pageRollStatusChara *chara = &this->charaList[index++];
		setupCharacter(this, chara, 0, this->playerKind, x, y, scale);
		chara->hp = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&parr->value, "hp"), 1);
		chara->mapIndex = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&parr->value, "mapIndex"), 0);
		parr = parr->next;
	}
	while(earr != NULL){
		enum hknwCharaKind kind = hknwCharaConvertKind(engineDataJsonGetString(engineDataJsonObjectGetValue(&earr->value, "kind"), NULL));
		int x = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&earr->value, "x"), 0);
		int y = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&earr->value, "y"), 0);
		double scale = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&earr->value, "scale"), 1.5);
		struct pageRollStatusChara *chara = &this->charaList[index++];
		setupCharacter(this, chara, 1, kind, x, y, scale);
		chara->hp = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&earr->value, "hp"), 1);
		chara->mapIndex = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&earr->value, "mapIndex"), 0);
		earr = earr->next;
	}
}

static void jsonItem(struct pageRollStatus *this, struct engineDataJsonValue *val){
	// 領域確保
	this->itemLength = engineDataJsonArrayGetLength(val);
	this->itemList = (struct pageRollStatusItem*)engineUtilMemoryCalloc(this->itemLength, sizeof(struct pageRollStatusItem));
	// アイテム作成
	int index = 0;
	struct engineDataJsonArray *arr = (val->type == ENGINEDATAJSONTYPE_ARRAY) ? val->jArray : NULL;
	while(arr != NULL){
		int type = jsonItemType(arr, "type");
		int x = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "x"), 0);
		int y = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "y"), 0);
		setupItem(this, &this->itemList[index++], type, x, y);
		arr = arr->next;
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// データ解釈
void objectRollParserParse(struct pageRollStatus *this, byte *json, size_t size){
	// jsonの準備
	struct engineDataJsonValue jsonValue;
	engineDataJsonParse(&jsonValue, json, size);

	// ステージ情報
	this->icon = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "icon"), this->id);
	this->mark = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "mark"), this->id);
	this->wall1 = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "wall1"), 81);
	this->wall2 = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "wall2"), 82);
	int bgmId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "bgmId"), 1);
	// BGM設定
	soundBgmPlayRoll(bgmId);

	// プレイヤーステータス設定
	this->health = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "health"), 9);
	this->gravityPower = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&jsonValue, "gravityPower"), 0.1);
	this->openStageIndex = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "openStage"), 0) - 1;
	this->openStoryIndex = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "openStory"), 0) - 1;
	this->openPuppetIndex = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "openPuppet"), 0) - 1;
	this->playerKind = hknwCharaConvertKind("default");
	if(this->isReplayWeb){this->playerKind = utilReplayGetPuppetId();}
	if(this->isReplayStorage){this->playerKind = this->storage.replayPuppet;}
	// マップ設定
	jsonMapchip(this, engineDataJsonObjectGetValue(&jsonValue, "map"));
	// キャラクター設定
	jsonCharacter(this, engineDataJsonObjectGetValue(&jsonValue, "player"), engineDataJsonObjectGetValue(&jsonValue, "enemy"));
	// アイテム設定
	jsonItem(this, engineDataJsonObjectGetValue(&jsonValue, "item"));

	// jsonの解放
	//engineDataJsonTrace(&jsonValue);
	engineDataJsonFree(&jsonValue);

	// カメラ初期化
	double playerx = 0;
	double playery = 0;
	int playerNum = 0;
	for(int i = 0; i < this->charaLength; i++){
		struct pageRollStatusChara *chara = &this->charaList[i];
		if(chara->team != 0 || chara->hp <= 0){continue;}
		playerx += chara->data.x;
		playery += chara->data.y;
		playerNum++;
	}
	this->camerax = this->cameraxNext = (playerNum > 0) ? (playerx / playerNum) : 0;
	this->cameray = this->camerayNext = (playerNum > 0) ? (playery / playerNum) : 0;
	this->cameraRot = this->cameraRotNext = 0;
	this->cameraScale = this->cameraScaleNext = 1.0;
	// 重力初期化
	this->dispDeviceRot = this->deviceRot = 0;
	this->dispGravityRot = this->gravityRot = 0;

	// リプレイ配列設定
	if(this->isReplayWeb){
		// webリプレイモード
		char *gravity = utilReplayGetGravity(this->replaySeed, &this->gravityListLength);
		this->gravityList = storageReplayBase64Decode(gravity, this->gravityListLength);
		this->gravityListIndex = 0;
		engineUtilMemoryFree(gravity);
		// 開始前の乱数記録
		if(!this->isSetSeed){utilRandomGetSeed(this->beforeSeed); this->isSetSeed = true;}
		// 乱数再現
		utilRandomSetSeed(this->replaySeed);
	}else if(this->isReplayStorage){
		// 自分の保持するリプレイデータ読み込み
		this->gravityListIndex = 0;
		this->gravityListLength = this->storage.replayLength;
		this->gravityList = storageReplayGet(&this->storage);
		// 開始前の乱数記録
		if(!this->isSetSeed){utilRandomGetSeed(this->beforeSeed); this->isSetSeed = true;}
		// 乱数再現
		this->replaySeed[0] = this->storage.replaySeed[0];
		this->replaySeed[1] = this->storage.replaySeed[1];
		this->replaySeed[2] = this->storage.replaySeed[2];
		this->replaySeed[3] = this->storage.replaySeed[3];
		utilRandomSetSeed(this->replaySeed);
	}else{
		// リプレイ用重力方向データ配列初期化 1体力1200フレーム
		this->gravityListIndex = 0;
		this->gravityListLength = this->health * (1200 + 1);
		this->gravityList = (short*)engineUtilMemoryCalloc(this->gravityListLength, sizeof(short));
		// 乱数記録
		utilRandomGetSeed(this->replaySeed);
	}
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 作成したデータを初期化
void objectRollParserReset(struct pageRollStatus *this){
	objectRollEffectManagerReset(&this->effectManager);
	for(int i = 0; i < this->mapLength; i++){objectRollMapDispose(this->mapList[i]);}
	engineUtilMemoryFree(this->mapList);
	engineUtilMemoryFree(this->charaList);
	engineUtilMemoryFree(this->itemList);
	engineUtilMemoryFree(this->gravityList);
	this->mapList = NULL;
	this->charaList = NULL;
	this->itemList = NULL;
	this->gravityList = NULL;
	this->mapLength = 0;
	this->charaLength = 0;
	this->itemLength = 0;
	this->gravityListLength = 0;

	// パラメータリセット
	this->nextStageId = 0;
	this->nextStoryId = 0;
	this->getCoin = 0;
	this->getStar = 0;
	this->getPlayerStar = 0;
	this->getEnemyStar = 0;
}

// 作成したデータを破棄
void objectRollParserDispose(struct pageRollStatus *this){
	hknwCharaManagerDispose(&this->charaManager);
	objectRollParserReset(this);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

