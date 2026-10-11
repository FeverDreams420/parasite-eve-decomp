/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle.h"

#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"

/* MASPSX_FORCE_G0 keeps the game-state flags absolute, as retail; the COMMON
 * script sound cursor and count supply the small-data metadata of the two
 * $gp-relative bytes. */
u8 D_8009D1D4;
u8 D_8009CE3C;

void Battle_FlushScriptSounds(void) {
    unsigned int index;
    int flags;
    int mask;
    int item;

    index = D_8009D1D4;
    while ((index & 0xFF) < D_8009CE3C) {
        item = (u16)D_800BE830[index & 0xFF].command;
        if ((unsigned int)(item - 3) < 0x180) {
            Inv_AddItem((short)item - 3);
        }

        index++;
    }

    flags = g_GameStateFlags;
    mask = -0x101;
    D_8009CE3C = 0;
    D_8009D1D4 = 0;
    g_GameStateFlags = flags & mask;
}
