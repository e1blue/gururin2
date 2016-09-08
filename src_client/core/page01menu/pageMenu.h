#ifndef __pageMenu_h_
#define __pageMenu_h_

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

// ページ状態構造体
struct pageMenuStatus{
	struct popupManager popup;

	// ステージストーリーデータ
	struct storageProgress0001 progress;
	struct pageMenuStatusSelect{
		int index;
		bool isStage;
		union{
			struct{
				int index;
				int id;
				int icon;
				int mark;
				struct storageStage0001 storage;
			} stage;
			struct{
				int index;
				int id;
				int icon;
			} story;
		};
	} *selectList;
	int selectLength;

	// パペットデータ
	struct pageMenuStatusPuppet{
		enum hknwCharaKind kind;
		bool isActive;
		int openStar;
	} *puppetList;
	int puppetLength;
	struct hknwCharaManager charaManager;
};

// ----------------------------------------------------------------
// ポップアップ関数

// メニューページカートリッジ用ポップアップ
void popupMenuTitleOpen(struct pageMenuStatus *stat);
void popupMenuSelectOpen(struct pageMenuStatus *stat);
void popupMenuPlayerOpen(struct pageMenuStatus *stat);
void popupMenuSettingOpen(struct pageMenuStatus *stat);
void popupMenuStartStageOpen(struct pageMenuStatus *stat, int id, bool isReplay);
void popupMenuStartStoryOpen(struct pageMenuStatus *stat, int id);

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

#endif

