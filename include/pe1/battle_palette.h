#ifndef PE1_BATTLE_PALETTE_H
#define PE1_BATTLE_PALETTE_H

#include "common.h"
#include "pe1/battle_hud_colors.h"

/* Wide declarations keep palette bytes out of GCC -G8 small-data lowering. */
typedef struct BattlePaletteSymbol {
    u8 bytes[16];
} BattlePaletteSymbol;



extern BattlePaletteSymbol D_8009D254_palette_view __asm__("D_8009D254");
extern BattlePaletteSymbol D_800915E0;
extern BattlePaletteSymbol D_800B0CE6;
extern BattlePaletteSymbol D_8009D1A0_read __asm__("D_8009D1A0");
extern BattlePaletteSymbol D_8009D1A0_write __asm__("D_8009D1A0");
extern BattlePaletteSymbol D_8009D2E8_read __asm__("D_8009D2E8");
extern BattlePaletteSymbol D_8009D2E8_write __asm__("D_8009D2E8");
extern u8 D_8009D2EC;
extern u8 D_8009D2A0;

void Battle_FlushScriptSounds(void);
void BattleCmd_SyncActiveAmmo(void);
void Battle_SetupPlayerPalette(void);

#endif
