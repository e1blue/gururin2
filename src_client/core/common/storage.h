#ifndef __storage_h_
#define __storage_h_

// バージョン情報構造体
struct storageVersion{
	unsigned char major;
	unsigned char miner;
};

void storageCommit();

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ステージ情報

// 進行管理構造体 ver.00.01
struct storageProgress0001{
	struct storageVersion version;
	signed char openStage[32];
	signed char openStory[16];
	signed char openPuppet[16];
	unsigned int totalStar;
};

// ステージ記録構造体 ver.00.01
struct storageStage0001{
	struct storageVersion version;
	signed short id;
	unsigned int score;
	signed long long replayId;
	signed short replayPuppet;
	unsigned long replaySeed[4];
	unsigned int replayLength;
};

void storageProgressGet(struct storageProgress0001 *data);
void storageProgressSet(struct storageProgress0001 *data);
void storageStageGet(struct storageStage0001 *data, int id);
void storageStageSet(struct storageStage0001 *data, short *replayList);

short *storageReplayBase64Decode(char *b64Buff, int replayLength);
short *storageReplayGet(struct storageStage0001 *data);

// ----------------------------------------------------------------
// 設定情報

double settingBgmVolumeGet();
bool settingBgmVolumeSet(double volume);

double settingSeVolumeGet();
bool settingSeVolumeSet(double volume);

enum hknwCharaKind settingPuppetGet();
bool settingPuppetSet(enum hknwCharaKind kind);

int settingSelectGet();
bool settingSelectSet(int index);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

