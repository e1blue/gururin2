#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "createSecret.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 揮発性一時バッファ 返値領域は解放禁止
void *corePluginUtilMemoryTemporary(size_t size){
	static void *staticBuff = NULL;
	static size_t staticSize = 0;
	if(staticSize < size){
		if(staticSize > 0){free(staticBuff);}
		staticSize = size;
		staticBuff = malloc(staticSize);
	}
	return staticBuff;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

typedef unsigned char byte;

static char localGlobal_enc[0x41] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
static char localGlobal_dec[0x80] = "";

// base64形式に変換 返値文字列は揮発性バッファで解放禁止
char *engineDataBase64encode(byte *data, size_t size){
	int rawLen = (int)size;
	int b64Len = ((rawLen + 2) / 3) * 4;
	size_t buffSize = (b64Len + 1) * sizeof(byte);
	byte *buff = (byte*)corePluginUtilMemoryTemporary(buffSize);

	// base64エンコード
	int x = 0;
	int i = 0;
	int j = 0;
	while(i < rawLen){
		x = (x << 8) | data[i++];
		switch(i % 3){
			case 1: buff[j++] = localGlobal_enc[(x >> 2) & 0x3f]; break;
			case 2: buff[j++] = localGlobal_enc[(x >> 4) & 0x3f]; break;
			case 0: buff[j++] = localGlobal_enc[(x >> 6) & 0x3f]; buff[j++] = localGlobal_enc[x & 0x3f]; break;
		}
	}
	switch(i % 3){
		case 1: buff[j++] = localGlobal_enc[(x << 4) & 0x3f]; buff[j++] = localGlobal_enc[0x40]; buff[j++] = localGlobal_enc[0x40]; break;
		case 2: buff[j++] = localGlobal_enc[(x << 2) & 0x3f]; buff[j++] = localGlobal_enc[0x40]; break;
		case 0: break;
	}
	buff[j++] = '\0';

	return (char*)buff;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

void encode(FILE *fp, char *key, char *value){
	fputs("#define ", fp);
	fputs(key, fp);
	fputs(" \"", fp);
	fputs(engineDataBase64encode((byte*)value, (size_t)(strlen(value) + 1)), fp);
	fputs("\"\n", fp);
}

// main関数 処理はここから始まる
int main(int argc, char *argv[]){
	if(argc != 2){return 0;}

	FILE *fp;
	fp = fopen(argv[1], "w");
	if(fp == NULL){return 0;}

	encode(fp, "PLUGINSECRET_AD_ANDROID_DEBUG_UNIT_ID",   PLUGINSECRET_AD_ANDROID_DEBUG_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_ANDROID_RELEASE_UNIT_ID", PLUGINSECRET_AD_ANDROID_RELEASE_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_ANDROID_TEST_DEVICE1",    PLUGINSECRET_AD_ANDROID_TEST_DEVICE1);
	encode(fp, "PLUGINSECRET_AD_ANDROID_TEST_DEVICE2",    PLUGINSECRET_AD_ANDROID_TEST_DEVICE2);
	encode(fp, "PLUGINSECRET_AD_ANDROID_TEST_DEVICE3",    PLUGINSECRET_AD_ANDROID_TEST_DEVICE3);
	encode(fp, "PLUGINSECRET_AD_IOS_DEBUG_UNIT_ID",       PLUGINSECRET_AD_IOS_DEBUG_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_IOS_RELEASE_UNIT_ID",     PLUGINSECRET_AD_IOS_RELEASE_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_IOS_TEST_DEVICE1",        PLUGINSECRET_AD_IOS_TEST_DEVICE1);
	encode(fp, "PLUGINSECRET_AD_IOS_TEST_DEVICE2",        PLUGINSECRET_AD_IOS_TEST_DEVICE2);
	encode(fp, "PLUGINSECRET_AD_IOS_TEST_DEVICE3",        PLUGINSECRET_AD_IOS_TEST_DEVICE3);
	encode(fp, "PLUGINSECRET_AD_UNKNOWN_DEBUG_UNIT_ID",   PLUGINSECRET_AD_UNKNOWN_DEBUG_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_UNKNOWN_RELEASE_UNIT_ID", PLUGINSECRET_AD_UNKNOWN_RELEASE_UNIT_ID);
	encode(fp, "PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE1",    PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE1);
	encode(fp, "PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE2",    PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE2);
	encode(fp, "PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE3",    PLUGINSECRET_AD_UNKNOWN_TEST_DEVICE3);

	fclose(fp);
	return 0;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

