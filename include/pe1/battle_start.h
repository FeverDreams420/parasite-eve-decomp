#ifndef PE1_BATTLE_START_H
#define PE1_BATTLE_START_H

#include "pe1/entity_animation.h"
#include "pe1/save_slot_metadata.h"

extern Combatant *D_8009D278;
extern BattleEntity *D_8009D254;
extern BattleRewardSlot D_800A7FF0[10];
extern BattleEntity *g_BattleActionActors[7];
extern int D_8009D1E8;
extern int D_8009D28C;
extern u8 D_8009CE7C, D_8009CE78, D_8009D288, D_8009CE74;
extern u8 D_8009D1CE, D_8009D235[16];
extern u32 D_8009D304;
extern u16 D_8009D21C;

extern unsigned int D_8009D1AC;
extern unsigned int D_8009D1A0;
extern unsigned int D_8009D2E8;
extern s32 D_8009D290[16];
extern u32 D_8009D250;
extern s16 D_8009D27C; /* battle camera height: player's model Y - 100 */

void Save_ResetGlobalFlags(void);
void srand(unsigned int seed);
void Battle_ResetEnemyStats(int mode);
void Battle_SetupEnemyAnims(void);
void Battle_CheckDropChance(void);
int Entity_CheckActionIdMatch(void);
void Battle_SetWindowBoundsByMode(int mode);
void Battle_StartEncounter(int mode);

#endif
