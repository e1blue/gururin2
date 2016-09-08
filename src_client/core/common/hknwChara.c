#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 初期化
struct hknwChara *hknwCharaInit(enum hknwCharaKind charaKind){
	struct hknwChara *this = (struct hknwChara*)engineUtilMemoryCalloc(1, sizeof(struct hknwChara));
	switch(charaKind){
		case HKNWCHARAKIND_PLAYERGURU: hknwCharaInitDataHuman(this, 1, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERGURU_X, TEXPOS_SYSTEM_PLAYERGURU_Y); break;
		case HKNWCHARAKIND_PLAYERKUMA: hknwCharaInitDataKuma(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERKUMA_X, TEXPOS_SYSTEM_PLAYERKUMA_Y); break;
		case HKNWCHARAKIND_PLAYERRIBB: hknwCharaInitDataHuman(this, 2, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERRIBB_X, TEXPOS_SYSTEM_PLAYERRIBB_Y); break;
		case HKNWCHARAKIND_PLAYERPEBU: hknwCharaInitDataBuggy(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERPEBU_X, TEXPOS_SYSTEM_PLAYERPEBU_Y); break;
		case HKNWCHARAKIND_PLAYERMAMI: hknwCharaInitDataHuman(this, 3, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERMAMI_X, TEXPOS_SYSTEM_PLAYERMAMI_Y); break;
		case HKNWCHARAKIND_PLAYERHAND: hknwCharaInitDataHand(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERHAND_X, TEXPOS_SYSTEM_PLAYERHAND_Y); break;
		case HKNWCHARAKIND_PLAYERBEAR: hknwCharaInitDataBear(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERBEAR_X, TEXPOS_SYSTEM_PLAYERBEAR_Y); break;
		case HKNWCHARAKIND_PLAYERGUDA: hknwCharaInitDataGuda(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERGUDA_X, TEXPOS_SYSTEM_PLAYERGUDA_Y); break;
		case HKNWCHARAKIND_PLAYERROBO: hknwCharaInitDataRobo(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERROBO_X, TEXPOS_SYSTEM_PLAYERROBO_Y); break;
		case HKNWCHARAKIND_PLAYERPIKA: hknwCharaInitDataHuman(this, 0, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_PLAYERPIKA_X, TEXPOS_SYSTEM_PLAYERPIKA_Y); break;
		case HKNWCHARAKIND_ENEMYKORO: hknwCharaInitDataBall(this, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_ENEMYKORO_X, TEXPOS_SYSTEM_ENEMYKORO_Y); break;
		case HKNWCHARAKIND_ENEMYBONE: hknwCharaInitDataHuman(this, 0, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_ENEMYBONE_X, TEXPOS_SYSTEM_ENEMYBONE_Y); break;
		case HKNWCHARAKIND_ENEMYARMO: hknwCharaInitDataHuman(this, 0, TEXSIZ_SYSTEM_WH, TEXPOS_SYSTEM_ENEMYARMO_X, TEXPOS_SYSTEM_ENEMYARMO_Y); break;
	}
	return this;
}

// 文字列からキャラクタ種類取得
enum hknwCharaKind hknwCharaConvertKind(char *kind){
	if(!strcmp(kind, "guru")){return HKNWCHARAKIND_PLAYERGURU;}
	if(!strcmp(kind, "kuma")){return HKNWCHARAKIND_PLAYERKUMA;}
	if(!strcmp(kind, "ribb")){return HKNWCHARAKIND_PLAYERRIBB;}
	if(!strcmp(kind, "pebu")){return HKNWCHARAKIND_PLAYERPEBU;}
	if(!strcmp(kind, "mami")){return HKNWCHARAKIND_PLAYERMAMI;}
	if(!strcmp(kind, "hand")){return HKNWCHARAKIND_PLAYERHAND;}
	if(!strcmp(kind, "bear")){return HKNWCHARAKIND_PLAYERBEAR;}
	if(!strcmp(kind, "guda")){return HKNWCHARAKIND_PLAYERGUDA;}
	if(!strcmp(kind, "robo")){return HKNWCHARAKIND_PLAYERROBO;}
	if(!strcmp(kind, "pika")){return HKNWCHARAKIND_PLAYERPIKA;}
	if(!strcmp(kind, "koro")){return HKNWCHARAKIND_ENEMYKORO;}
	if(!strcmp(kind, "bone")){return HKNWCHARAKIND_ENEMYBONE;}
	if(!strcmp(kind, "armo")){return HKNWCHARAKIND_ENEMYARMO;}
	return settingPuppetGet();
}

// 文字列からポーズ取得
enum hknwCharaPosesId hknwCharaConvertPoseId(char *pose){
	if(!strcmp(pose, "squat")){return HKNWCHARAPOSESID_SQUAT;}
	if(!strcmp(pose, "happy")){return HKNWCHARAPOSESID_HAPPY;}
	return HKNWCHARAPOSESID_STAND;
}

// ----------------------------------------------------------------

// ハコニワキャラクタ作成用データのパーツ作成
void hknwCharaPartsSet(struct hknwCharaParts *this, enum hknwCharaPartsId partsId, double x, double y, double z, int texx, int texy, int texs, int type, bool isActive){
	if(!isActive){return;}
	this->partsId = partsId;
	this->x = x;
	this->y = y;
	this->z = z;
	this->texx = texx;
	this->texy = texy;
	this->texs = texs;
	this->type = type;
}

// ハコニワキャラクタ作成用データのポーズ作成
void hknwCharaPosesSet(struct hknwCharaPoses *this, enum hknwCharaPosesId posesId, int stepIndex, enum hknwCharaPartsId partsId, double x, double y, double z, double r, int type, bool isActive){
	if(!isActive){return;}
	this->posesId = posesId;
	this->partsId = partsId;
	this->stepIndex = stepIndex;
	this->x = x;
	this->y = y;
	this->z = z - 0.05; // ハコニワキャラクタのサイドビュー補正
	this->r = r;
	this->type = type;
}

// ----------------------------------------------------------------

// 配列にハコニワキャラクタの描画情報を追加
void hknwCharaCreateArray(struct hknwChara *this){
	this->faceIndex = engineGraphicBufferFaceIndexGet();
	this->faceNum = (this->partsNum * 4) * 2;

	// パーツのテクスチャ座標データを作成
	for(int i = 0; i < this->partsNum; i++){
		struct hknwCharaParts *parts = this->parts + i;
		int s = parts->texs;
		double vs = s / 128.0;
		for(int j = 0; j < 4; j++){
			engineGraphicBufferPushTetraFace(engineGraphicBufferVretIndexGet());
			engineGraphicBufferPushTetraVert(-vs, -vs, vs * 2, vs * 2);
			engineGraphicBufferPushTetraTexc(this->imgw, this->imgh, parts->texx + s * j, parts->texy + s, s, -s);
		}
	}
}

// ----------------------------------------------------------------

// ポーズのステップ数確認
int hknwCharaGetPoseStepNum(struct hknwChara *this, enum hknwCharaPosesId poseId){
	int stepNum = 0;
	for(int i = 0; i < this->posesNum; i++){
		if(this->poses[i].posesId == poseId && stepNum < this->poses[i].stepIndex){
			stepNum = this->poses[i].stepIndex;
		}
	}
	return stepNum + 1;
}

// ----------------------------------------------------------------

#define HKNWCHARATEMPPARTS_MAXLENGTH 16

// 描画パーツ構造体
struct hknwCharaTempParts{
	double x;
	double y;
	double z;
	int faceIndex;
	int yswap;
	int zswap;
};

// 並べ替え関数
static int hknwCharaTempParts_sort(const void *a, const void *b){
	struct hknwCharaTempParts *parts1 = *(struct hknwCharaTempParts**)a;
	struct hknwCharaTempParts *parts2 = *(struct hknwCharaTempParts**)b;
	double z1 = parts1->z;
	double z2 = parts2->z;
	return (z1 > z2) ? 1 : (z1 < z2) ? -1 : 0;
}

// テクスチャ垂直軸角度フレーム
static void drawCharaAngleV(double rot, int *anglev, int *anglevTire){
	*anglev = (int)utilMathRound(1 + (CONST_RAD2DEG * rot) / 90) % 4;
	while(*anglev < 0){*anglev += 4;};

	// タイヤ用
	*anglevTire = (int)utilMathRound(2 - (CONST_RAD2DEG * rot) / 45) % 4;
	while(*anglevTire < 0){*anglevTire += 4;};
}

// キャラクタ描画のタイプによる反転など
static void drawCharaTypeSwap(int partsIndex, int poseType, int partsType, int anglev, int anglevTire, struct hknwCharaTempParts *parts){
	parts->yswap = 1;
	parts->zswap = 1;
	if(!!(poseType & 0x01)){
		// パーツセットの上下反転
		parts->zswap *= -1;
		parts->z *= -1;
	}
	if(!!(poseType & 0x10)){
		// パーツセットの前後反転
		anglev = (6 - anglev) % 4;
		parts->yswap *= -1;
		parts->x *= -1;
	}
	if(!!(partsType & 0x10)){
		// パーツの左右反転
		anglev = (4 - anglev) % 4;
		parts->yswap *= -1;
	}
	// フレーム確認
	if(!!(partsType & 0x01)){anglev = 0;}
	if(!!(partsType & 0x02)){anglev = anglevTire;}
	parts->faceIndex = (partsIndex * 4 + anglev) * 2;
}

// キャラクタの描画 2D用 使えるパーツ数制限あり
void hknwCharaDrawChara2D(struct hknwChara *this, struct engineMathMatrix44 *mat, double r, double s, enum hknwCharaPosesId poseId, int step){
	if(this->faceNum <= 0){return;}
	double roth = CONST_DEG2RAD * 0;
	struct engineMathMatrix44 tempmat1;

	// 並べ替え用構造体
	struct hknwCharaTempParts tempParts[HKNWCHARATEMPPARTS_MAXLENGTH];
	struct hknwCharaTempParts *tempPartsSort[HKNWCHARATEMPPARTS_MAXLENGTH];
	int tempPartsLength = 0;

	// パーツの位置を計算
	double c2 = utilMathCos(r);
	double s2 = utilMathSin(r);
	double c3 = utilMathCos(roth);
	double s3 = utilMathSin(roth);
	for(int i = 0; i < this->posesNum; i++){
		if(this->poses[i].posesId == poseId && step == this->poses[i].stepIndex){
			double x0 = this->poses[i].x;
			double y0 = this->poses[i].z;
			double z0 = this->poses[i].y;

			// テクスチャ垂直軸角度フレーム
			int anglev0 = 0;
			int anglev1 = 0;
			drawCharaAngleV(0 - r - this->poses[i].r, &anglev0, &anglev1);

			double c1 = utilMathCos(this->poses[i].r);
			double s1 = utilMathSin(this->poses[i].r);
			for(int j = 0; j < this->partsNum; j++){
				if(this->poses[i].partsId == this->parts[j].partsId){
					struct hknwCharaTempParts *parts = &tempParts[tempPartsLength++];
					parts->x = this->parts[j].x;
					parts->y = this->parts[j].z;
					parts->z = this->parts[j].y;
					// キャラクタ描画のタイプによる反転など
					drawCharaTypeSwap(j, this->poses[i].type, this->parts[j].type, anglev0, anglev1, parts);

					double x1 = x0 + parts->x * c1 - parts->y * s1;
					double y1 = y0 + parts->x * s1 + parts->y * c1;
					double z1 = z0 + parts->z;
					double x2 = (x1 * c2 - z1 * s2);
					double y2 = y1;
					double z2 = (x1 * s2 + z1 * c2);
					parts->x = x2;
					parts->y = y2 * c3 - z2 * s3;
					parts->z = y2 * s3 + z2 * c3;
				}
				if(tempPartsLength >= HKNWCHARATEMPPARTS_MAXLENGTH){break;}
			}
		}
		if(tempPartsLength >= HKNWCHARATEMPPARTS_MAXLENGTH){break;}
	}

	// 並べ替え
	for(int i = 0; i < tempPartsLength; i++){tempPartsSort[i] = &tempParts[i];}
	qsort((void*)tempPartsSort, tempPartsLength, sizeof(struct hknwCharaTempParts*), hknwCharaTempParts_sort);

	// 描画
	for(int i = 0; i < tempPartsLength; i++){
		engineMathMat4Copy(&tempmat1, mat);
		engineMathMat4Scale(&tempmat1, 35 * s, -35 * s, 1);
		engineMathMat4Translate(&tempmat1, tempPartsSort[i]->x, tempPartsSort[i]->y, 0);
		engineMathMat4Scale(&tempmat1, tempPartsSort[i]->yswap, tempPartsSort[i]->zswap, 1);
		engineGraphicEngineSetMatrix(&tempmat1);
		engineGraphicEngineDrawIndex((this->faceIndex + tempPartsSort[i]->faceIndex) * 3, 2 * 3);
	}
}

// キャラクタの描画
void hknwCharaDrawChara3D(struct hknwChara *this, struct engineMathMatrix44 *mat, double rotv, double roth, double r, double s, enum hknwCharaPosesId poseId, int step){
	if(this->faceNum <= 0){return;}
	struct engineMathMatrix44 tempmat1;
	struct engineMathMatrix44 tempmat2;

	// 行列作成
	engineMathMat4Copy(&tempmat1, mat);
	engineMathMat4RotateY(&tempmat1, -r);
	engineMathMat4Scale(&tempmat1, s, s, s);

	for(int i = 0; i < this->posesNum; i++){
		if(this->poses[i].posesId == poseId && step == this->poses[i].stepIndex){
			double x0 = this->poses[i].x;
			double y0 = this->poses[i].y;
			double z0 = this->poses[i].z;
			double r0 = this->poses[i].r;

			// テクスチャ垂直軸角度フレーム
			int anglev0 = 0;
			int anglev1 = 0;
			drawCharaAngleV(rotv - r - r0, &anglev0, &anglev1);

			for(int j = 0; j < this->partsNum; j++){
				if(this->poses[i].partsId == this->parts[j].partsId){
					struct hknwCharaTempParts parts;
					parts.x = this->parts[j].x;
					parts.y = this->parts[j].y;
					parts.z = this->parts[j].z;
					// キャラクタ描画のタイプによる反転など
					drawCharaTypeSwap(j, this->poses[i].type, this->parts[j].type, anglev0, anglev1, &parts);

					// 行列作成
					engineMathMat4Copy(&tempmat2, &tempmat1);
					engineMathMat4Translate(&tempmat2, x0, z0, y0);
					engineMathMat4RotateY(&tempmat2, -r0);
					engineMathMat4Translate(&tempmat2, parts.x, parts.z, parts.y);
					engineMathMat4RotateY(&tempmat2, -rotv + r + r0);
					engineMathMat4RotateX(&tempmat2, -roth);
					engineMathMat4Scale(&tempmat2, parts.yswap, parts.zswap, 1);
					engineGraphicEngineSetMatrix(&tempmat2);

					engineGraphicEngineDrawIndex((this->faceIndex + parts.faceIndex) * 3, 2 * 3);
				}
			}
		}
	}
}

// ----------------------------------------------------------------

// 破棄
void hknwCharaDispose(struct hknwChara *this){
	if(this == NULL){return;}
	engineUtilMemoryFree(this->parts);
	engineUtilMemoryFree(this->poses);
	engineUtilMemoryFree(this);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ハコニワキャラクタ管理構造体に要素取得 なければ追加
struct hknwChara *hknwCharaManagerGet(struct hknwCharaManager *this, enum hknwCharaKind charaKind){
	// 同じ要素がすでにあるか確認
	for(int i = 0; i < this->drawLength; i++){
		if(this->drawList[i].kind == charaKind && this->drawList[i].draw != NULL){return this->drawList[i].draw;}
	}
	// データの挿入先を探す
	int index = -1;
	// リストの空きを探す
	for(int i = 0; i < this->drawLength; i++){
		struct hknwCharaManagerDraw *temp = &this->drawList[i];
		if(temp->draw == NULL){index = i; break;}
	}
	if(index < 0){
		// リストに空きがなかったらリスト拡張
		index = this->drawLength;
		int addNum = 10;
		int length = index + addNum;
		void *list = engineUtilMemoryCalloc(length, sizeof(*this->drawList));
		if(this->drawLength > 0){
			memcpy(list, this->drawList, this->drawLength * sizeof(*this->drawList));
			engineUtilMemoryFree(this->drawList);
		}
		this->drawLength = length;
		this->drawList = list;
	}
	// データ挿入
	struct hknwCharaManagerDraw *unit = &this->drawList[index];
	unit->kind = charaKind;
	unit->draw = hknwCharaInit(charaKind);
	this->bufferCharaStatus = 0;
	return unit->draw;
}

// キャラクターバッファ作成
void hknwCharaManagerCreateBuffer(struct hknwCharaManager *this){
	int status = 1;

	if(this->bufferCharaStatus != status){
		this->bufferCharaStatus = status;

		// バッファ作成開始
		engineGraphicBufferBegin();

		// キャラクター作成
		for(int i = 0; i < this->drawLength; i++){
			if(this->drawList[i].draw != NULL){
				hknwCharaCreateArray(this->drawList[i].draw);
			}
		}

		// バッファ作成完了
		engineGraphicBufferEnd(&this->e3dIdCharaVert, NULL, &this->e3dIdCharaTexc, &this->e3dIdCharaFace);
	}
}

// 破棄
void hknwCharaManagerDispose(struct hknwCharaManager *this){
	for(int i = 0; i < this->drawLength; i++){
		if(this->drawList[i].draw != NULL){
			hknwCharaDispose(this->drawList[i].draw);
		}
	}
	engineUtilMemoryFree(this->drawList);
	this->drawList = NULL;
	this->drawLength = 0;
	engineGraphicObjectVBODispose(this->e3dIdCharaVert);
	engineGraphicObjectVBODispose(this->e3dIdCharaTexc);
	engineGraphicObjectIBODispose(this->e3dIdCharaFace);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

