#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 設定構造体 ver.00.01
struct storageSetting0001{
	struct storageVersion version;
	unsigned char bgmVolume;
	unsigned char seVolume;
	signed short puppet;
	signed short select;
};

static struct{
	struct storageSetting0001 settingData;
} localGlobal;

// 設定取得
static void init(){
	static bool isInit = false;
	if(!isInit){
		isInit = true;

		// バージョンリセット
		struct storageVersion *version = &localGlobal.settingData.version;
		version->major = 0x00;
		version->miner = 0x00;

		// 保存データ読み出し
		char *b64Buff = platformPluginPreferenceGet("storage_setting");
		// base64デコード
		int rawLen = 0;
		unsigned char *rawBuff = NULL;
		if(b64Buff != NULL){
			rawBuff = engineDataBase64decodeChar(b64Buff, &rawLen);
			version = (struct storageVersion*)rawBuff;
		}

		// バージョンチェック
		int latestMajor = 0x00;
		int latestMiner = 0x01;
		if(version->major == latestMajor && version->miner == latestMiner){
			// 最新バージョン
			memcpy(&localGlobal.settingData, rawBuff, sizeof(struct storageSetting0001));
		}else{
			// 初期化
			localGlobal.settingData.bgmVolume = 50;
			localGlobal.settingData.seVolume = 50;
			localGlobal.settingData.puppet = (int)HKNWCHARAKIND_PLAYERGURU;
			localGlobal.settingData.select = 1;
		}
		localGlobal.settingData.version.major = latestMajor;
		localGlobal.settingData.version.miner = latestMiner;
	}
}

// 設定保存
static void save(){
	char *buff = engineDataBase64encode((unsigned char*)&localGlobal.settingData, sizeof(struct storageSetting0001));
	platformPluginPreferenceSet("storage_setting", buff);
}

// ----------------------------------------------------------------

// BGM音量取得
double settingBgmVolumeGet(){
	init();
	return localGlobal.settingData.bgmVolume / 100.0;
}

// BGM音量設定
bool settingBgmVolumeSet(double volume){
	init();

	int intvol = (int)(volume * 100);
	bool isSet = localGlobal.settingData.bgmVolume != intvol;
	if(isSet){
		// 設定保存
		localGlobal.settingData.bgmVolume = intvol;
		save();

		// 設定反映
		platformPluginSoundBgmVolume(volume);
	}
	return isSet;
}

// ----------------------------------------------------------------

// SE音量取得
double settingSeVolumeGet(){
	init();
	return localGlobal.settingData.seVolume / 100.0;
}

// SE音量設定
bool settingSeVolumeSet(double volume){
	init();

	int intvol = (int)(volume * 100);
	bool isSet = localGlobal.settingData.seVolume != intvol;
	if(isSet){
		// 設定保存
		localGlobal.settingData.seVolume = intvol;
		save();

		// 設定反映
		platformPluginSoundSeVolume(volume);
	}
	return isSet;
}

// ----------------------------------------------------------------

// 使用パペット取得
enum hknwCharaKind settingPuppetGet(){
	init();
	return (enum hknwCharaKind)localGlobal.settingData.puppet;
}

// 使用パペット設定
bool settingPuppetSet(enum hknwCharaKind kind){
	init();

	int puppet = (int)kind;
	bool isSet = localGlobal.settingData.puppet != puppet;
	if(isSet){
		// 設定保存
		localGlobal.settingData.puppet = puppet;
		save();
	}
	return isSet;
}

// ----------------------------------------------------------------

// 最終実行ステージ取得
int settingSelectGet(){
	init();
	return localGlobal.settingData.select;
}

// 最終実行ステージ設定
bool settingSelectSet(int index){
	init();

	bool isSet = localGlobal.settingData.select != index;
	if(isSet){
		// 設定保存
		localGlobal.settingData.select = index;
		save();
	}
	return isSet;
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

