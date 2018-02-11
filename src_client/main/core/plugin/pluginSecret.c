#include "../includeAll.h"
#include "pluginSecret.h"
#include "pluginSecretCoded.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

//#define __DEBUG__

#if defined __APPLE__
	#if defined TARGET_OS_IPHONE
		#define __IOS__
	#elif defined TARGET_IPHONE_SIMULATOR
		#define __IOS__
	#elif defined TARGET_OS_MAC
		#define __MAC__
	#else
		#define __IOS__
	#endif
#endif

#if defined __ANDROID__
	#if defined __DEBUG__
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_ANDROID_DEBUG_UNIT_ID
	#else
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_ANDROID_RELEASE_UNIT_ID
	#endif
	#define PLUGINSECRET_AD_TEST_DEVICE1 PLUGINSECRET_AD_ANDROID_TEST_DEVICE1
	#define PLUGINSECRET_AD_TEST_DEVICE2 PLUGINSECRET_AD_ANDROID_TEST_DEVICE2
	#define PLUGINSECRET_AD_TEST_DEVICE3 PLUGINSECRET_AD_ANDROID_TEST_DEVICE3
#elif defined __IOS__
	#if defined __DEBUG__
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_IOS_DEBUG_UNIT_ID
	#else
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_IOS_RELEASE_UNIT_ID
	#endif
	#define PLUGINSECRET_AD_TEST_DEVICE1 PLUGINSECRET_AD_IOS_TEST_DEVICE1
	#define PLUGINSECRET_AD_TEST_DEVICE2 PLUGINSECRET_AD_IOS_TEST_DEVICE2
	#define PLUGINSECRET_AD_TEST_DEVICE3 PLUGINSECRET_AD_IOS_TEST_DEVICE3
#else
	#if defined __DEBUG__
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_UNKNOWN_DEBUG_UNIT_ID
	#else
		#define PLUGINSECRET_AD_UNIT_ID PLUGINSECRET_AD_UNKNOWN_RELEASE_UNIT_ID
	#endif
	#define PLUGINSECRET_AD_TEST_DEVICE1 PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE1
	#define PLUGINSECRET_AD_TEST_DEVICE2 PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE2
	#define PLUGINSECRET_AD_TEST_DEVICE3 PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE3
#endif

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 秘密取得 返値文字列は揮発性バッファで解放禁止
char *gamePluginSecretGet(const char *key, int id){
	if(strcmp(key, "goodfox")){return NULL;}
	char *value = NULL;
	char *unitId = PLUGINSECRET_AD_UNIT_ID;
	char *deviceId1 = PLUGINSECRET_AD_TEST_DEVICE1;
	char *deviceId2 = PLUGINSECRET_AD_TEST_DEVICE2;
	char *deviceId3 = PLUGINSECRET_AD_TEST_DEVICE3;
	if(id == 10010001){value = unitId;}
	else if(id == 10020001){value = deviceId1;}
	else if(id == 10020002){value = deviceId2;}
	else if(id == 10020003){value = deviceId3;}
	if(value == NULL){return NULL;}
	return (char*)engineDataBase64decodeChar(value, NULL);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

