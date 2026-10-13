#ifndef PE1_BATTLE_LEVEL_UP_H
#define PE1_BATTLE_LEVEL_UP_H

#include "pe1/entity_animation.h"

extern BattleEntity *D_8009D254;
extern BattleRewardSlot D_800A7FF0[10];
extern u8 D_8009CE74;
extern s16 D_8009D2A4;
extern u32 D_8009D304;
extern u16 D_8009D21C;
extern int g_BattleModeState;

extern u32 g_GameStateFlags;

void Pm_StopAllBoth(void);
void Aya_SetTotalExp(u32 exp, u16 pe_bonus, void *rewards);
int Render_BeginSceneLoad(void);
int CdRom_StepReadState(int mode);
void Battle_SetupPlayerPalette(void);
void Battle_StepLevelUp(void);

#endif
