#include "../includeAll.h"
#include "pageStory.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

static double jsonNumberActive(struct engineDataJsonArray *arr, char *name, int activeIndex, int *activeValue){
	struct engineDataJsonValue *val = engineDataJsonObjectGetValue(&arr->value, name);
	bool isActive = (val != NULL && (val->type == ENGINEDATAJSONTYPE_INT || val->type == ENGINEDATAJSONTYPE_FLOAT));
	if(isActive){*activeValue |= (1 << activeIndex);}
	else{*activeValue &= ~(1 << activeIndex);}
	return isActive ? engineDataJsonGetFloat(val, 0.0) : 0.0;
}

static void jsonMapchip(struct engineDataJsonArray *arr, struct pageStoryEngineStep *stepData){
	struct engineDataJsonValue *mapchip = engineDataJsonObjectGetValue(&arr->value, "mapchip");
	struct engineDataJsonArray *arrx, *arry, *arrz;
	int xsize = 64;
	int ysize = 64;
	int zsize = 64;
	// 数を数える
	int ztemp = 0;
	arrz = (mapchip->type == ENGINEDATAJSONTYPE_ARRAY) ? mapchip->jArray : NULL;
	while(arrz != NULL){
		int ytemp = 0;
		arry = (arrz->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arrz->value.jArray : NULL;
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
		ztemp++;
		arrz = arrz->next;
	}
	if(zsize > ztemp){zsize = ztemp;}
	// 領域確保
	int *mapData = (int*)engineUtilMemoryMalloc(xsize * ysize * zsize * sizeof(int));
	// 要素を読み込む
	int zindex = 0;
	arrz = (mapchip->type == ENGINEDATAJSONTYPE_ARRAY) ? mapchip->jArray : NULL;
	while(arrz != NULL && zindex < zsize){
		int yindex = 0;
		arry = (arrz->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arrz->value.jArray : NULL;
		while(arry != NULL && yindex < ysize){
			int xindex = 0;
			arrx = (arry->value.type == ENGINEDATAJSONTYPE_ARRAY) ? arry->value.jArray : NULL;
			while(arrx != NULL && xindex < xsize){
				mapData[xsize * (ysize * zindex + yindex) + xindex] = (int)engineDataJsonGetInt(&arrx->value, 0);
				xindex++;
				arrx = arrx->next;
			}
			yindex++;
			arry = arry->next;
		}
		zindex++;
		arrz = arrz->next;
	}
	// ステップに設定
	stepData->createMap.xsize = xsize;
	stepData->createMap.ysize = ysize;
	stepData->createMap.zsize = zsize;
	stepData->createMap.mapchip = mapData;
}

static enum pageStoryEmotion jsonCharaEmotion(struct engineDataJsonArray *arr, char *name){
	char *emotion = engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, name), NULL);
	if(!strcmp(emotion, "happy")){return PAGESTORYEMOTION_HAPPY;}
	if(!strcmp(emotion, "exclamation")){return PAGESTORYEMOTION_EXCLAMATION;}
	if(!strcmp(emotion, "question")){return PAGESTORYEMOTION_QUESTION;}
	if(!strcmp(emotion, "sad")){return PAGESTORYEMOTION_SAD;}
	if(!strcmp(emotion, "angry")){return PAGESTORYEMOTION_ANGRY;}
	return PAGESTORYEMOTION_NONE;
}

static int jsonItemType(struct engineDataJsonArray *arr, char *name){
	char *type = engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, name), NULL);
	if(!strcmp(type, "star")){return 1;}
	if(!strcmp(type, "superStar")){return 2;}
	return 0;
}

// エンジン初期化
void objectStoryEngineInit(struct pageStoryStatus *this, byte *json, size_t size){
	// マップデータ初期化
	this->mapLength = 3;
	this->mapList = (struct pageStoryStatusMap*)engineUtilMemoryCalloc(this->mapLength, sizeof(struct pageStoryStatusMap));

	// キャラクター実態初期化
	this->charaLength = 3;
	this->charaList = (struct pageStoryStatusChara*)engineUtilMemoryCalloc(this->charaLength, sizeof(struct pageStoryStatusChara));

	// アイテム初期化
	this->itemLength = 3;
	this->itemList = (struct pageStoryStatusItem*)engineUtilMemoryCalloc(this->itemLength, sizeof(struct pageStoryStatusItem));

	// jsonの準備
	struct engineDataJsonValue jsonValue;
	engineDataJsonParse(&jsonValue, json, size);

	// ストーリー情報
	this->icon = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "icon"), this->id);
	this->wall = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&jsonValue, "wall"), 82);

	// ストーリーステップ初期化
	struct engineDataJsonValue *story = engineDataJsonObjectGetValue(&jsonValue, "story");
	this->stepLength = engineDataJsonArrayGetLength(story);
	this->stepList = (struct pageStoryEngineStep*)engineUtilMemoryCalloc(this->stepLength, sizeof(struct pageStoryEngineStep));
	int stepIndex = 0;
	struct engineDataJsonArray *arr = (story->type == ENGINEDATAJSONTYPE_ARRAY) ? story->jArray : NULL;
	while(arr != NULL){
		char *command = engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, "command"), NULL);
		struct pageStoryEngineStep *stepData = &this->stepList[stepIndex++];
		if(!strcmp(command, "createMap")){
			stepData->type = PAGESTORYENGINESTEPTYPE_CREATEMAP;
			stepData->createMap.mapId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "mapId"), 0);
			jsonMapchip(arr, stepData);
		}else if(!strcmp(command, "setCamera")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SETCAMERA;
			stepData->setCamera.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->setCamera.x = jsonNumberActive(arr, "x", 0, &stepData->setCamera.active);
			stepData->setCamera.y = jsonNumberActive(arr, "y", 1, &stepData->setCamera.active);
			stepData->setCamera.z = jsonNumberActive(arr, "z", 2, &stepData->setCamera.active);
			stepData->setCamera.scale = jsonNumberActive(arr, "scale", 3, &stepData->setCamera.active);
		}else if(!strcmp(command, "setGravity")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SETGRAVITY;
			stepData->setGravity.time = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "time"), 0.0);
			stepData->setGravity.rotg = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "rotg"), 0.0) * CONST_DEG2RAD;
		}else if(!strcmp(command, "createChara")){
			stepData->type = PAGESTORYENGINESTEPTYPE_CREATECHARA;
			stepData->createChara.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->createChara.kind = hknwCharaConvertKind(engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, "kind"), NULL));
			stepData->createChara.scale = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "scale"), 1.5);
			stepData->createChara.isShadow = engineDataJsonGetBool(engineDataJsonObjectGetValue(&arr->value, "isShadow"), true);
		}else if(!strcmp(command, "setCharaPose")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SETCHARAPOSE;
			stepData->setCharaPose.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->setCharaPose.pose = hknwCharaConvertPoseId(engineDataJsonGetString(engineDataJsonObjectGetValue(&arr->value, "pose"), NULL));
		}else if(!strcmp(command, "setCharaEmotion")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SETCHARAEMOTION;
			stepData->setCharaEmotion.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->setCharaEmotion.emotion = jsonCharaEmotion(arr, "emotion");
			stepData->setCharaEmotion.scale = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "scale"), 1.0);
		}else if(!strcmp(command, "setCharaStatus")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SETCHARASTATUS;
			stepData->setCharaStatus.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->setCharaStatus.time = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "time"), 0.0);
			stepData->setCharaStatus.x = jsonNumberActive(arr, "x", 0, &stepData->setCharaStatus.active);
			stepData->setCharaStatus.y = jsonNumberActive(arr, "y", 1, &stepData->setCharaStatus.active);
			stepData->setCharaStatus.z = jsonNumberActive(arr, "z", 2, &stepData->setCharaStatus.active);
			stepData->setCharaStatus.r = jsonNumberActive(arr, "r", 3, &stepData->setCharaStatus.active) * CONST_DEG2RAD;
			stepData->setCharaStatus.rotg = jsonNumberActive(arr, "rotg", 4, &stepData->setCharaStatus.active) * CONST_DEG2RAD;
		}else if(!strcmp(command, "actCharaWalk")){
			stepData->type = PAGESTORYENGINESTEPTYPE_ACTCHARAWALK;
			stepData->actCharaWalk.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->actCharaWalk.time = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "time"), 0.0);
			stepData->actCharaWalk.x = jsonNumberActive(arr, "x", 0, &stepData->actCharaWalk.active);
			stepData->actCharaWalk.y = jsonNumberActive(arr, "y", 1, &stepData->actCharaWalk.active);
			stepData->actCharaWalk.z = jsonNumberActive(arr, "z", 2, &stepData->actCharaWalk.active);
			stepData->actCharaWalk.speed = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "speed"), 1.0);
		}else if(!strcmp(command, "actCharaJump")){
			stepData->type = PAGESTORYENGINESTEPTYPE_ACTCHARAJUMP;
			stepData->actCharaJump.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
			stepData->actCharaJump.time = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "time"), 0.0);
			stepData->actCharaJump.x = jsonNumberActive(arr, "x", 0, &stepData->actCharaJump.active);
			stepData->actCharaJump.y = jsonNumberActive(arr, "y", 1, &stepData->actCharaJump.active);
			stepData->actCharaJump.z = jsonNumberActive(arr, "z", 2, &stepData->actCharaJump.active);
			stepData->actCharaJump.height = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "height"), 1.0);
		}else if(!strcmp(command, "removeChara")){
			stepData->type = PAGESTORYENGINESTEPTYPE_REMOVECHARA;
			stepData->removeChara.charaId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "charaId"), 0);
		}else if(!strcmp(command, "createItem")){
			stepData->type = PAGESTORYENGINESTEPTYPE_CREATEITEM;
			stepData->createItem.itemId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "itemId"), 0);
			stepData->createItem.type = jsonItemType(arr, "type");
			stepData->createItem.x = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "x"), 0.0);
			stepData->createItem.y = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "y"), 0.0);
			stepData->createItem.z = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "z"), 0.0);
		}else if(!strcmp(command, "removeItem")){
			stepData->type = PAGESTORYENGINESTEPTYPE_REMOVEITEM;
			stepData->removeItem.itemId = (int)engineDataJsonGetInt(engineDataJsonObjectGetValue(&arr->value, "itemId"), 0);
		}else if(!strcmp(command, "wait")){
			stepData->type = PAGESTORYENGINESTEPTYPE_WAIT;
			stepData->wait.time = engineDataJsonGetFloat(engineDataJsonObjectGetValue(&arr->value, "time"), 0.0);
		}else if(!strcmp(command, "skipBreak")){
			stepData->type = PAGESTORYENGINESTEPTYPE_SKIPBREAK;
		}else{
			stepData->type = PAGESTORYENGINESTEPTYPE_NONE;
		}
		arr = arr->next;
	}

	// jsonの解放
	//engineDataJsonTrace(&jsonValue);
	engineDataJsonFree(&jsonValue);

	this->camerax = this->cameraxNext = 0;
	this->cameray = this->camerayNext = 0;
	this->cameraz = this->camerazNext = 0;
	this->cameraDist = this->cameraDistNext = 10;
	this->rotg = CONST_DEG2RAD * 0;
}

// ----------------------------------------------------------------

// マップ作成
static void commandCreateMap(struct pageStoryStatus *this, int mapId, int xsize, int ysize, int zsize, int *mapchip){
	if(mapId < 0){return;}
	// 上書き対象もしくは空いているデータを探す
	int index = -1;
	for(int i = 0; i < this->mapLength; i++){if(this->mapList[i].mapId == mapId){index = i; break;}}
	if(index < 0){for(int i = 0; i < this->mapLength; i++){if(this->mapList[i].map == NULL){index = i; break;}}}
	if(index < 0){return;}
	struct pageStoryStatusMap *mapData = &this->mapList[index];
	// 古いデータが残っていたら除去
	if(mapData->map != NULL){hknwMapDispose(mapData->map);}

	// データ作成
	mapData->mapId = mapId;
	mapData->map = hknwMapInit(xsize, ysize, zsize, mapchip);
}

// キャラクター作成
static void commandCreateChara(struct pageStoryStatus *this, int charaId, enum hknwCharaKind kind, double scale, bool isShadow){
	if(charaId < 0){return;}
	// 上書き対象もしくは空いているデータを探す
	int index = -1;
	for(int i = 0; i < this->charaLength; i++){if(this->charaList[i].charaId == charaId){index = i; break;}}
	if(index < 0){for(int i = 0; i < this->charaLength; i++){if(!this->charaList[i].isExist){index = i; break;}}}
	if(index < 0){return;}
	struct pageStoryStatusChara *charaData = &this->charaList[index];

	// キャラクター描画データ作成もしくは再利用
	struct hknwChara *draw = hknwCharaManagerGet(&this->charaManager, kind);

	// データ作成
	charaData->charaId = charaId;
	charaData->isExist = true;
	charaData->isShadow = isShadow;
	charaData->scale = scale;
	charaData->x = 4;
	charaData->y = 4;
	charaData->z = 1;
	charaData->r = CONST_DEG2RAD * 180;
	charaData->rotg = CONST_DEG2RAD * 0;
	charaData->rad = draw->radius3d * charaData->scale;
	charaData->kind = kind;
	charaData->pose = HKNWCHARAPOSESID_STAND;
	charaData->step = 0;
}

// アイテム作成
static void commandCreateItem(struct pageStoryStatus *this, int itemId, int type, double x, double y, double z){
	if(itemId < 0){return;}
	// 上書き対象もしくは空いているデータを探す
	int index = -1;
	for(int i = 0; i < this->itemLength; i++){if(this->itemList[i].itemId == itemId){index = i; break;}}
	if(index < 0){for(int i = 0; i < this->itemLength; i++){if(!this->itemList[i].isExist){index = i; break;}}}
	if(index < 0){return;}
	struct pageStoryStatusItem *itemData = &this->itemList[index];

	// データ作成
	itemData->itemId = itemId;
	itemData->type = type;
	itemData->isExist = true;
	itemData->x = x;
	itemData->y = y;
	itemData->z = z;
}

// キャラクター獲得
static struct pageStoryStatusChara *getChara(struct pageStoryStatus *this, int charaId){
	for(int i = 0; i < this->charaLength; i++){
		struct pageStoryStatusChara *charaData = &this->charaList[i];
		if(charaData->charaId == charaId && charaData->isExist){return charaData;}
	}
	return NULL;
}

// アイテム獲得
static struct pageStoryStatusItem *getItem(struct pageStoryStatus *this, int itemId){
	for(int i = 0; i < this->itemLength; i++){
		struct pageStoryStatusItem *itemData = &this->itemList[i];
		if(itemData->itemId == itemId && itemData->isExist){return itemData;}
	}
	return NULL;
}

// コマンド カメラ設定
static void commandSetCamera(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	if(this->stepActed){return;}else{this->stepActed = true;}
	// 値設定
	struct pageStoryStatusChara *charaData = getChara(this, stepData->setCamera.charaId);
	if(charaData != NULL){
		this->cameraCharaId = stepData->setCamera.charaId;
		this->cameraxNext = charaData->x;
		this->camerayNext = charaData->y;
		this->camerazNext = charaData->z;
	}else if(stepData->setCamera.active & ((1 << 0) | (1 << 1) | (1 << 2))){
		this->cameraCharaId = -1;
		if(stepData->setCamera.active & (1 << 0)){this->cameraxNext = stepData->setCamera.x;}
		if(stepData->setCamera.active & (1 << 1)){this->camerayNext = stepData->setCamera.y;}
		if(stepData->setCamera.active & (1 << 2)){this->camerazNext = stepData->setCamera.z;}
	}
	if((stepData->setCamera.active & (1 << 3)) && stepData->setCamera.scale > 0){this->cameraDistNext = 10 / stepData->setCamera.scale;}
	if(!this->isInitCamera){
		// 最初のwaitに突入するまでは初期化状態でカメラの移動演出を行わない
		this->camerax = this->cameraxNext;
		this->cameray = this->camerayNext;
		this->cameraz = this->camerazNext;
		this->cameraDist = this->cameraDistNext;
	}
	stepData->step = -1;
}

// コマンド 全体の重力方向設定
static void commandSetGravity(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	if(this->stepActed){return;}else{this->stepActed = true;}
	if(stepData->step == 0){this->stepRotg = this->rotg;}
	// コマンド進捗割合計算
	double param = (stepData->setGravity.time > 0) ? (++stepData->step / (stepData->setGravity.time * 60)) : 1;
	if(param < 1){this->isWait = true;}else{param = 1; stepData->step = -1; this->stepActed = false;}
	// 値設定
	double drotg = this->stepRotg - stepData->setGravity.rotg;
	while(drotg < -CONST_PI){drotg += CONST_PI * 2;}
	while(drotg >  CONST_PI){drotg -= CONST_PI * 2;}
	this->rotg = this->stepRotg - drotg * param;
}

// コマンド キャラクターポーズ設定
static void commandSetCharaPose(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->setCharaPose.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}
	// 値設定
	charaData->pose = stepData->setCharaPose.pose;
	charaData->step = 0;
	stepData->step = -1;
}

// コマンド キャラクター感情設定
static void commandSetCharaEmotion(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->setCharaEmotion.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}
	// 値設定
	charaData->emotion = stepData->setCharaEmotion.emotion;
	charaData->emotionScale = stepData->setCharaEmotion.scale;
	charaData->stepBalloon = 0;
	stepData->step = -1;
}

// コマンド キャラクター状態設定
static void commandSetCharaStatus(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->setCharaStatus.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}else{charaData->stepActed = true;}
	if(stepData->step == 0){
		// コマンド初期化
		charaData->stepx = charaData->x; if(!(stepData->setCharaStatus.active & (1 << 0))){stepData->setCharaStatus.x = charaData->x;}
		charaData->stepy = charaData->y; if(!(stepData->setCharaStatus.active & (1 << 1))){stepData->setCharaStatus.y = charaData->y;}
		charaData->stepz = charaData->z; if(!(stepData->setCharaStatus.active & (1 << 2))){stepData->setCharaStatus.z = charaData->z;}
		charaData->stepr = charaData->r; if(!(stepData->setCharaStatus.active & (1 << 3))){stepData->setCharaStatus.r = charaData->r;}
		charaData->stepRotg = charaData->rotg; if(!(stepData->setCharaStatus.active & (1 << 4))){stepData->setCharaStatus.rotg = charaData->rotg;}
	}
	// コマンド進捗割合計算
	double param = (stepData->setCharaStatus.time > 0) ? (++stepData->step / (stepData->setCharaStatus.time * 60)) : 1;
	if(param < 1){this->isWait = true;}else{param = 1; stepData->step = -1; charaData->stepActed = false;}
	// 値設定
	double dr = charaData->stepr - stepData->setCharaStatus.r;
	double drotg = charaData->stepRotg - stepData->setCharaStatus.rotg;
	while(dr < -CONST_PI){dr += CONST_PI * 2;}
	while(dr >  CONST_PI){dr -= CONST_PI * 2;}
	while(drotg < -CONST_PI){drotg += CONST_PI * 2;}
	while(drotg >  CONST_PI){drotg -= CONST_PI * 2;}
	charaData->x = charaData->stepx * (1 - param) + stepData->setCharaStatus.x * param;
	charaData->y = charaData->stepy * (1 - param) + stepData->setCharaStatus.y * param;
	charaData->z = charaData->stepz * (1 - param) + stepData->setCharaStatus.z * param;
	charaData->r = charaData->stepr - dr * param;
	charaData->rotg = charaData->stepRotg  - drotg * param;
}

// コマンド キャラクター歩行指示
static void commandActCharaWalk(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->actCharaWalk.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}else{charaData->stepActed = true;}
	if(stepData->step == 0){
		// コマンド初期化
		charaData->stepx = charaData->x; if(!(stepData->actCharaWalk.active & (1 << 0))){stepData->actCharaWalk.x = charaData->x;}
		charaData->stepy = charaData->y; if(!(stepData->actCharaWalk.active & (1 << 1))){stepData->actCharaWalk.y = charaData->y;}
		charaData->stepz = charaData->z; if(!(stepData->actCharaWalk.active & (1 << 2))){stepData->actCharaWalk.z = charaData->z;}
		double x = stepData->actCharaWalk.x - charaData->stepx;
		double y = stepData->actCharaWalk.y - charaData->stepy;
		double z = stepData->actCharaWalk.z - charaData->stepz;
		double gx = x * utilMathCos(-charaData->rotg) - z * utilMathSin(-charaData->rotg);
		if(stepData->setCharaStatus.time <= 0){
			stepData->setCharaStatus.time = utilMathSqrt(x * x + y * y + z * z) / stepData->actCharaWalk.speed;
		}
		if(engineMathAbs(gx) >= DBL_EPSILON || engineMathAbs(y) >= DBL_EPSILON){charaData->r = utilMathAtan2(y, gx);}
		charaData->pose = HKNWCHARAPOSESID_WALK;
		charaData->step = 0;
	}
	// コマンド進捗割合計算
	double param = (stepData->setCharaStatus.time > 0) ? (++stepData->step / (stepData->setCharaStatus.time * 60)) : 1;
	if(param < 1){this->isWait = true;}else{param = 1; stepData->step = -1; charaData->stepActed = false; charaData->pose = HKNWCHARAPOSESID_STAND;}
	// 値設定
	charaData->x = charaData->stepx * (1 - param) + stepData->actCharaWalk.x * param;
	charaData->y = charaData->stepy * (1 - param) + stepData->actCharaWalk.y * param;
	charaData->z = charaData->stepz * (1 - param) + stepData->actCharaWalk.z * param;
}

// コマンド キャラクター跳躍指示
static void commandActCharaJump(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->actCharaJump.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}else{charaData->stepActed = true;}
	double c = utilMathCos(-charaData->rotg);
	double s = utilMathSin(-charaData->rotg);
	if(stepData->step == 0){
		// コマンド初期化
		charaData->stepx = charaData->x; if(!(stepData->actCharaJump.active & (1 << 0))){stepData->actCharaJump.x = charaData->x;}
		charaData->stepy = charaData->y; if(!(stepData->actCharaJump.active & (1 << 1))){stepData->actCharaJump.y = charaData->y;}
		charaData->stepz = charaData->z; if(!(stepData->actCharaJump.active & (1 << 2))){stepData->actCharaJump.z = charaData->z;}
		double x = stepData->actCharaJump.x - charaData->stepx;
		double y = stepData->actCharaJump.y - charaData->stepy;
		double z = stepData->actCharaJump.z - charaData->stepz;
		double gx = x * c - z * s;
		double gy = x * s + z * c;
		if(stepData->setCharaStatus.time <= 0){
			double maxHeight = stepData->actCharaJump.height + utilMathAbs(gy) * 0.5;
			if(maxHeight < gy){maxHeight = gy;}
			stepData->actCharaJump.time = 0.5 * utilMathSqrt(maxHeight);
		}
		if(engineMathAbs(gx) >= DBL_EPSILON || engineMathAbs(y) >= DBL_EPSILON){charaData->r = utilMathAtan2(y, gx);}
		
		// エフェクト
		double ex = charaData->x + 0.3 * s;
		double ey = charaData->y;
		double ez = charaData->z + 0.3 * c;
		objectStoryEffectManagerPushDust(&this->effectManager, ex, ey, ez);
	}
	// コマンド進捗割合計算
	double param = (stepData->actCharaJump.time > 0) ? (++stepData->step / (stepData->actCharaJump.time * 60)) : 1;
	charaData->pose = (param < 0.5) ? HKNWCHARAPOSESID_JUMP : HKNWCHARAPOSESID_FALL;
	if(param < 1){this->isWait = true;}else{param = 1; stepData->step = -1; charaData->stepActed = false; charaData->pose = HKNWCHARAPOSESID_STAND;}
	// 値設定
	double jump = 4 * param * (1 - param) * stepData->actCharaJump.height;
	charaData->x = charaData->stepx * (1 - param) + stepData->actCharaJump.x * param + jump * s;
	charaData->y = charaData->stepy * (1 - param) + stepData->actCharaJump.y * param;
	charaData->z = charaData->stepz * (1 - param) + stepData->actCharaJump.z * param + jump * c;
}

// コマンド キャラクター除去
static void commandRemoveChara(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusChara *charaData = getChara(this, stepData->removeChara.charaId);
	if(charaData == NULL){stepData->step = -1; return;}
	if(charaData->stepActed){return;}
	// 値設定
	charaData->isExist = false;
	stepData->step = -1;
}

// コマンド アイテム除去
static void commandRemoveItem(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	struct pageStoryStatusItem *itemData = getItem(this, stepData->removeItem.itemId);
	if(itemData == NULL){stepData->step = -1; return;}
	// 値設定
	itemData->isExist = false;
	stepData->step = -1;
}

// コマンド 待機
static bool commandWait(struct pageStoryStatus *this, struct pageStoryEngineStep *stepData){
	bool isWait = this->isWait || ((stepData->wait.time > 0) ? (++stepData->step < stepData->wait.time * 60) : false);
	if(!isWait){stepData->step = -1;}
	return isWait;
}

// エンジン計算
void objectStoryEngineCalc(struct pageStoryStatus *this){
	// ステップループ
	do{
		// ステップ初期化
		this->isWait = false;
		this->stepActed = false;
		for(int i = 0; i < this->charaLength; i++){
			struct pageStoryStatusChara *charaData = &this->charaList[i];
			charaData->step++;
			charaData->stepBalloon++;
			charaData->stepActed = false;
		}
		// ステップメイン処理
		bool waitBreak = false;
		for(int i = 0; i < this->stepLength; i++){
			struct pageStoryEngineStep *stepData = &this->stepList[i];
			if(stepData->step < 0){continue;}
			switch(stepData->type){
				case PAGESTORYENGINESTEPTYPE_NONE: break;
				case PAGESTORYENGINESTEPTYPE_CREATEMAP: stepData->step = -1; commandCreateMap(this, stepData->createMap.mapId, stepData->createMap.xsize, stepData->createMap.ysize, stepData->createMap.zsize, stepData->createMap.mapchip); break;
				case PAGESTORYENGINESTEPTYPE_SETCAMERA: commandSetCamera(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_SETGRAVITY: commandSetGravity(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_CREATECHARA: stepData->step = -1; commandCreateChara(this, stepData->createChara.charaId, stepData->createChara.kind, stepData->createChara.scale, stepData->createChara.isShadow); break;
				case PAGESTORYENGINESTEPTYPE_SETCHARAPOSE: commandSetCharaPose(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_SETCHARAEMOTION: commandSetCharaEmotion(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_SETCHARASTATUS: commandSetCharaStatus(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_ACTCHARAWALK: commandActCharaWalk(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_ACTCHARAJUMP: commandActCharaJump(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_REMOVECHARA: commandRemoveChara(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_CREATEITEM: stepData->step = -1; commandCreateItem(this, stepData->createItem.itemId, stepData->createItem.type, stepData->createItem.x, stepData->createItem.y, stepData->createItem.z); break;
				case PAGESTORYENGINESTEPTYPE_REMOVEITEM: commandRemoveItem(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_WAIT: waitBreak = commandWait(this, stepData); break;
				case PAGESTORYENGINESTEPTYPE_SKIPBREAK: stepData->step = -1; this->isSkip = false; break;
			}
			if(waitBreak){break;}
		}
		if(!waitBreak && !this->isWait){
			// すべてのステップが完了
			if(this->isSkip){this->isExit = true;}
			this->isSkip = false;
			this->isFinish = true;
		}
	}while(this->isSkip);

	// カメラ
	this->isInitCamera = true;
	struct pageStoryStatusChara *cameraCharaData = getChara(this, this->cameraCharaId);
	if(cameraCharaData != NULL){
		this->cameraxNext = cameraCharaData->x;
		this->camerayNext = cameraCharaData->y;
		this->camerazNext = cameraCharaData->z;
	}
	// カメラ位置の減衰計算
	this->camerax -= 0.1 * (this->camerax - this->cameraxNext);
	this->cameray -= 0.1 * (this->cameray - this->camerayNext);
	this->cameraz -= 0.1 * (this->cameraz - this->camerazNext);
	this->cameraDist -= 0.1 * (this->cameraDist - this->cameraDistNext);
}

// ----------------------------------------------------------------

// エンジン破棄
void objectStoryEngineDispose(struct pageStoryStatus *this){
	for(int i = 0; i < this->stepLength; i++){
		struct pageStoryEngineStep *stepData = &this->stepList[i];
		if(stepData->type == PAGESTORYENGINESTEPTYPE_CREATEMAP){engineUtilMemoryFree(stepData->createMap.mapchip);}
	}
	for(int i = 0; i < this->mapLength; i++){if(this->mapList[i].map != NULL){hknwMapDispose(this->mapList[i].map);}}
	hknwCharaManagerDispose(&this->charaManager);
	engineUtilMemoryFree(this->stepList);
	engineUtilMemoryFree(this->mapList);
	engineUtilMemoryFree(this->charaList);
	engineUtilMemoryFree(this->itemList);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

