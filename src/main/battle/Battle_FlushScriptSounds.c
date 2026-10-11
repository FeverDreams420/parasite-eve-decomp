/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/battle.h"

#include "Battle_FlushScriptSounds_externs.h"

#define D_8009D1A0_R (D_8009D1A0_r.flags)
#define D_8009D1A0_W (D_8009D1A0_w.flags)

int Inv_AddItem(int slot);

void Battle_FlushScriptSounds(void) {
    unsigned int index;
    int flags;
    int mask;
    int item;

    index = D_8009D1D4;
    while ((index & 0xFF) < D_8009CE3C) {
        item = *(unsigned short *)((char *)D_800BE834 + ((index & 0xFF) << 3));
        if ((unsigned int)(item - 3) < 0x180) {
            Inv_AddItem((short)item - 3);
        }

        index++;
    }

    flags = D_8009D1A0_R;
    mask = -0x101;
    D_8009CE3C = 0;
    D_8009D1D4 = 0;
    D_8009D1A0_W = flags & mask;
}
