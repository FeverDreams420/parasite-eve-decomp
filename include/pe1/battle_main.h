#ifndef PE1_BATTLE_MAIN_H
#define PE1_BATTLE_MAIN_H

#include "common.h"
#include "pe1/battle.h"

/* Battle turn, status and map-selection flow (src/main/main). */

/* 20-byte record copied as one unit by the enemy effect flow. */
typedef struct { u8 bytes[0x14]; } __attribute__((packed)) Tbl20;
/* 18-byte status symbol sprite template. */
typedef struct StatusSymbolTemplate {
    u8 bytes[18];
} StatusSymbolTemplate;

extern const BattleActionSoundTable D_80010760;
extern s32 g_BattleResultFlags[];
extern u8 D_8009D274, D_8009CE39, D_8009CE3A;
extern s16 D_8009D27C;
extern const Tbl20 D_8001074C;
extern u8 D_8009CE3B;
extern u16 D_800BE834[];
extern u8 D_8009E360[];
extern u16 D_8009E362[];
extern u8 g_BattleStatusRowPosY[];
extern u8 D_8009E508[];
extern u8 D_8009E516[];
extern StatusSymbolTemplate D_80010DFC;
extern StatusSymbolTemplate D_80010E10;
extern StatusSymbolTemplate D_80010E24;
extern u8 D_8009E93C[];
extern u8 D_8009E93D[];
extern u8 D_8009E930[];

#endif /* PE1_BATTLE_MAIN_H */
