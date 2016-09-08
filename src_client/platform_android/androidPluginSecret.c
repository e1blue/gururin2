#include "../core/library.h"
#include "native.h"
#include "../core/plugin/pluginSecret.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 秘密取得
JNIEXPORT jstring JNICALL Java_com_totetero_fuhaha_AndroidPluginSecret_gamePluginSecretGet(JNIEnv *env, jobject obj, jstring key, jint id){
	const char *cstr = (*env)->GetStringUTFChars(env, key, 0);
	jstring value = (*env)->NewStringUTF(env, gamePluginSecretGet(cstr, id));
	(*env)->ReleaseStringUTFChars(env, key, cstr);
	return value;	
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

