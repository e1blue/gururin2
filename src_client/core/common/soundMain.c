#include "../library.h"
#include "../includeAll.h"

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

void soundGameInit(){
	platformPluginSoundBgmLoad(1, "snd/bgm/8bit_rpg_town_02");
	platformPluginSoundBgmLoad(2, "snd/bgm/8bit_action_athletic_02");
	platformPluginSoundBgmLoad(3, "snd/bgm/8bit_action_athletic_01");
	platformPluginSoundBgmLoad(4, "snd/bgm/8bit_action_athletic_04");
	platformPluginSoundBgmLoad(5, "snd/bgm/8bit_action_athletic_03");
	platformPluginSoundSeLoad(1, "snd/se/system_ok_07");
	platformPluginSoundSeLoad(2, "snd/se/system_cancel_02");
	platformPluginSoundSeLoad(3, "snd/se/noise_coin_15");
	platformPluginSoundSeLoad(4, "snd/se/action_jump_06");
	platformPluginSoundSeLoad(5, "snd/se/action_jump_02");
	platformPluginSoundSeLoad(6, "snd/se/action_jump_01");
	platformPluginSoundBgmVolume(settingBgmVolumeGet());
	platformPluginSoundSeVolume(settingSeVolumeGet());
}

void soundBgmStop(){platformPluginSoundBgmPlay(-1);}
void soundBgmPlayMenu(){platformPluginSoundBgmPlay(1);}
void soundBgmPlayRoll(int id){platformPluginSoundBgmPlay(1 + id);}
void soundBgmToneDown(){platformPluginSoundBgmToneDown(0.5);}

void soundSePlayOK(){platformPluginSoundSePlay(1);}
void soundSePlayNG(){platformPluginSoundSePlay(2);}
void soundSePlayCoin(){platformPluginSoundSePlay(3);}
void soundSePlayAttack(){platformPluginSoundSePlay(4);}
void soundSePlayDamage(){platformPluginSoundSePlay(5);}
void soundSePlayDash(){platformPluginSoundSePlay(6);}

// ----------------------------------------------------------------
// ----------------------------------------------------------------
// ----------------------------------------------------------------

