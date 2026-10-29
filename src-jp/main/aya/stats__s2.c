/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_runtime.h"
#include "pe1/game_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_sound.h"
#include "pe1/player_entity.h"
#include "pe1/stat_modifiers.h"
#include "pe1/textbox.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvBonusSlotCount;

void Menu_PlayConfirmSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44C, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayCancelSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44D, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayMoveSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44E, 0x100, 0x80, 0x7F);
    }
}

void Menu_PlayErrorSound(void) {
    if (g_AkaoBgmHandle[0] != 0) {
        Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x44F, 0x100, 0x80, 0x7F);
    }
}