#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ユーティリティ初期化
void utilGameInit(){
	utilRandomInit((unsigned long)utilTimeGet());
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 回転の減衰計算
double utilCalcRotateDamper(double rot, double rotNext, double param){
	double dr = rot - rotNext;
	while(dr < -CONST_PI){dr += CONST_PI * 2;}
	while(dr >  CONST_PI){dr -= CONST_PI * 2;}
	if(utilMathAbs(dr) > 0.01){rot -= dr * param;}
	while(rot < -CONST_PI){rot += CONST_PI * 2;}
	while(rot >  CONST_PI){rot -= CONST_PI * 2;}
	return rot;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// プラットフォーム取得
char *utilPlatformGet(){return platformPluginUtilPlatformGet();}
// ユーザーID取得
char *utilUidGet(){return platformPluginUtilUidGet();}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 時間獲得
long long int utilTimeGet(){return (long long int)platformPluginUtilTimeGet();}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// web用体験版ロック
int utilLockedSelectIndex(){return platformLockedSelectIndex();}

// web用編集ステージ
int utilEditedExist(){return platformEditedExist();}
char *utilEditedGet(){return platformEditedGet();}

// web用リプレイモード
int utilReplayExist(){return platformReplayExist();}
int utilReplayGetStageId(){return platformReplayGetStageId();}
int utilReplayGetPuppetId(){return platformReplayGetPuppetId();}
char *utilReplayGetGravity(unsigned long *seed, int *length){return platformReplayGetGravity(seed, length);}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// twitter投稿

void utilTwitter(char *post){platformTwitter(post);}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

