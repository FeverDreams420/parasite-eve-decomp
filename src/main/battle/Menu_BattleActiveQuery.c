/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle.h"

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
unsigned char g_BattleActionQueueCount;
#include "pe1/game_state.h"
#include "pe1/battle_entity_anim.h"

int Battle_IsActive(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}

int Pad_IsMenuConfirmAvailable(void) {
    if (g_BattleActionQueueCount != 0) {
        return 0;
    }
    if ((g_GameStateFlags & 2) == 0 || (g_ActiveActor->stateFlags & 0x10000) == 0) {
        return 1;
    }
    return 0;
}
