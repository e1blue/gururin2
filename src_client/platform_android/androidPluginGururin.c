#include "../core/library.h"
#include "native.h"
#include "../core/plugin/pluginGururin.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// web用体験版ロック スマートフォンでは何もしない
int platformLockedSelectIndex(){return 0;}

// web用編集ステージ情報取得 スマートフォンでは何もしない
int platformEditedExist(){return 0;}
char *platformEditedGet(){return NULL;}

// web用リプレイモード情報取得 スマートフォンでは何もしない
int platformReplayExist(){return 0;}
int platformReplayGetStageId(void){return 0;}
int platformReplayGetPuppetId(void){return 0;}
char *platformReplayGetGravity(unsigned long *seed, int *length){return NULL;}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// twitter投稿
void platformTwitter(char *post){
	JNIEnv *env = getJNIEnv();
	jclass cls = (*env)->FindClass(env, "com/totetero/fuhaha/AndroidPluginGururin");
	jmethodID mid = (*env)->GetStaticMethodID(env, cls, "jniTwitter", "(Ljava/lang/String;)V");
	jstring str = (*env)->NewStringUTF(env, post);
	(*env)->CallStaticVoidMethod(env, cls, mid, str);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

