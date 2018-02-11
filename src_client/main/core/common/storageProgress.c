#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// 進行状態取得
void storageProgressGet(struct storageProgress0001 *data){
	// バージョンリセット
	struct storageVersion *version = &data->version;
	version->major = 0x00;
	version->miner = 0x00;

	// 保存データ読み出し
	char key[32];
	snprintf(key, sizeof(key), "storage_progress");
	char *b64Buff = platformPluginPreferenceGet(key);
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
		memcpy(data, rawBuff, sizeof(struct storageProgress0001));
	}else{
		// 初期化
		memset(data, 0, sizeof(struct storageProgress0001));
		data->openStage[0] = 0x01 | 0x02;
		data->openStory[0] = 0x01 | 0x02;
		data->openPuppet[0] = 0x01 | 0x02;
	}
	data->version.major = latestMajor;
	data->version.miner = latestMiner;
}

// 進行状態保存
void storageProgressSet(struct storageProgress0001 *data){
	// base64エンコード
	char *buff = engineDataBase64encode((unsigned char*)data, sizeof(struct storageProgress0001));
	// 保存データ書き込み
	char key[32];
	snprintf(key, sizeof(key), "storage_progress");
	platformPluginPreferenceSet(key, buff);
}

// ----------------------------------------------------------------

// ステージ記録取得
void storageStageGet(struct storageStage0001 *data, int id){
	// バージョンリセット
	struct storageVersion *version = &data->version;
	version->major = 0x00;
	version->miner = 0x00;

	// 保存データ読み出し
	char key[32];
	snprintf(key, sizeof(key), "storage_progress_stage_%04d", id);
	char *b64Buff = platformPluginPreferenceGet(key);
	// base64デコード
	int rawLen = 0;
	unsigned char *rawBuff = NULL;
	if(b64Buff != NULL){
		rawBuff = engineDataBase64decodeChar(b64Buff, &rawLen);
		version = (struct storageVersion*)rawBuff;
	}

	// バージョンチェック
	int latestMajor = 0x01;
	int latestMiner = 0x03;
	if(version->major == latestMajor && version->miner == latestMiner){
		// 最新バージョン
		memcpy(data, rawBuff, sizeof(struct storageStage0001));
	}else if(version->major == 0x01 && version->miner == 0x02){
		// バージョン1.2 リプレイをリセット
		memcpy(data, rawBuff, sizeof(struct storageStage0001));
		data->replayLength = 0;
	}else if(version->major == 0x01 && version->miner == 0x01){
		// バージョン1.1 リプレイをリセット
		memcpy(data, rawBuff, sizeof(struct storageStage0001));
		data->replayLength = 0;
	}else if(version->major == 0x00 && version->miner == 0x01){
		// バージョン0.1 リプレイをリセット
		memcpy(data, rawBuff, sizeof(struct storageStage0001));
		data->replayLength = 0;
	}else{
		// 初期化
		memset(data, 0, sizeof(struct storageStage0001));
		data->id = id;
		data->replayId = -1;
	}
	data->version.major = latestMajor;
	data->version.miner = latestMiner;
}

// ステージ記録保存
void storageStageSet(struct storageStage0001 *data, short *replayList){
	// base64エンコード
	char *buff = engineDataBase64encode((unsigned char*)data, sizeof(struct storageStage0001));
	// 保存データ書き込み
	char key[32];
	snprintf(key, sizeof(key), "storage_progress_stage_%04d", data->id);
	platformPluginPreferenceSet(key, buff);

	if(replayList != NULL){
		// base64エンコード
		int repLength = data->replayLength * sizeof(short);
		char *repBuff = engineDataBase64encode((unsigned char*)replayList, repLength);
		// 保存データ書き込み
		char repKey[32];
		snprintf(repKey, sizeof(repKey), "storage_progress_replay_%04d", data->id);
		platformPluginPreferenceSet(repKey, repBuff);
	}
}

// ----------------------------------------------------------------

// リプレイデータ取得 base64形式を変換
short *storageReplayBase64Decode(char *b64Buff, int replayLength){
	if(b64Buff != NULL){
		// base64デコード
		int rawLen = 0;
		unsigned char *rawBuff = engineDataBase64decodeChar(b64Buff, &rawLen);

		// データ領域確保
		int length = replayLength * sizeof(short);
		if(rawLen == length){
			void *repBuff = engineUtilMemoryMalloc(length);
			memcpy(repBuff, rawBuff, length);
			return (short*)repBuff;
		}
	}

	return NULL;
}

// リプレイデータ取得
short *storageReplayGet(struct storageStage0001 *data){
	// 保存データ読み出し
	char key[32];
	snprintf(key, sizeof(key), "storage_progress_replay_%04d", data->id);
	char *b64Buff = platformPluginPreferenceGet(key);

	return storageReplayBase64Decode(b64Buff, data->replayLength);
}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

