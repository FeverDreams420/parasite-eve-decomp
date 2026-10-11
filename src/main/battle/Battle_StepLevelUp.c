/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_level_up.h"
#include "pe1/game_state.h"

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
u8 D_8009CE74;
s16 D_8009D2A4;
u32 D_8009D304;
u16 D_8009D21C;
int D_8009D28C;

void Battle_StepLevelUp(void) {
    u8 step;
    BattleEntity *entity;
    u32 flags;

    step = D_8009CE74;
    switch (step) {
    case 0:
        entity = D_8009D254;
        if ((u16)(entity->animFrame >> 16) != 0xA && (D_8009D1A0 & 0x800) == 0) {
            break;
        }
        {
            u32 entity_flags;
            entity_flags = entity->entityFlags;
            entity->entityFlags = entity_flags | 0x100;
        }
        Pm_StopAllBoth();
        Aya_SetTotalExp(D_8009D304, D_8009D21C, D_800A7FF0);
        Render_BeginSceneLoad();
        D_8009CE74++;
        break;

    case 1:
        if (D_8009D2A4 == 0x3E8) {
            entity = D_8009D254;
            D_8009CE74 = step + 1;
            entity->entityFlags &= -0x101;
        }
        break;

    case 2:
        entity = D_8009D254;
        if (entity->animLastFrame == entity->animPrev.parts.integer) {
            flags = D_8009D1A0;
            if ((flags & 0x1800) == 0) {
                Entity_SetActionMode(entity, 0x15);
            } else {
                u32 cleared_flags;
                cleared_flags = flags & -0x1801;
                D_8009D1A0 = cleared_flags;
                Entity_SetActionMode(entity, 0x18);
            }
            D_8009CE74++;
        }
        break;

    case 3:
        if (CD_StepReadState(0) != 1) {
            Battle_SetupPlayerPalette();
            D_8009D28C = 9;
            g_GameState.flags &= 0xFFFF7FFF;
        }
        break;
    }
}
