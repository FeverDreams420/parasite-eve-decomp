/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_hud_colors.h"

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
s8 D_8009CE30;
u16 D_8009D298;

#include "Battle_InitEntityColors_externs.h"
void Entity_SetActionMode(BattleEntity *entity, int mode);
/* The flag store goes through a byte-offset scalar access: as a field store
 * GCC would move the D_8009D298 store above it. */
#define COMBATANT_FIELD(ptr, type, member) \
    (*(type *)((u8 *)(ptr) + PE1_OFFSETOF(Combatant, member)))

void Battle_InitEntityColors(void) {
    s32 flags;

    Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
    flags = D_8009D278->stateFlags;
    D_8009CE30 = 0;
    COMBATANT_FIELD(D_8009D278, s32, stateFlags) = flags & ~0x2000;
    D_8009D298 = 0;
    D_800B0158[0] = 0xFF;
    D_800B0159[0] = 0x3D;
    D_800B015A[0] = 0x81;
    D_800B0160[0] = 0x83;
    D_800B0161[0] = 0x13;
    D_800B0162[0] = 1;
    D_800B0168[0] = 0xFF;
    D_800B0169[0] = 0x3D;
    D_800B016A[0] = 0x81;
    D_800B0170[0] = 0x83;
    D_800B0171[0] = 0x13;
    D_800B0172[0] = 1;
    D_800B01A0[0] = 0xFF;
    D_800B01A1[0] = 0x3D;
    D_800B01A2[0] = 0x81;
    D_800B01A8[0] = 0x83;
    D_800B01A9[0] = 0x13;
    D_800B01AA[0] = 1;
    D_800B01B0[0] = 0xFF;
    D_800B01B1[0] = 0x3D;
    D_800B01B2[0] = 0x81;
    D_800B01B8[0] = 0x83;
    D_800B01B9[0] = 0x13;
    D_800B01BA[0] = 1;
}

#undef COMBATANT_FIELD
