/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle.h"
#include "pe1/battle_runtime.h"

/* COMMON supplies small-data metadata to MASPSX. */
u8 D_8009CE3C, D_8009CE44, D_8009CE60;
u8 D_8009D2D8, D_8009D1DC;

/* Step the turn index back to the first slot of the action that ends at
 * initial. The test before the loop is the loop test, so GCC merges the
 * two and enters the decrement loop through it. The halfword parameter
 * makes the rewind compute its own initial - 1 instead of reusing the one
 * from the command lookup. */
static inline void RewindTurn(s16 initial)
{
    D_8009CE3C = initial - 1;
    if (D_8009CE3C == 0 ||
        D_800BE830[D_8009CE3C].actionGeneration !=
        D_800BE830[D_8009CE3C - 1].actionGeneration)
        return;
    do {
        D_8009CE3C--;
    } while (D_8009CE3C != 0 &&
        D_800BE830[D_8009CE3C].actionGeneration ==
        D_800BE830[D_8009CE3C - 1].actionGeneration);
}

static inline void RestorePending(int index)
{
    if (D_8009CE60) {
        D_8009CE60 = 0;
        D_8009CE3C = index;
    }
}

void Battle_AdvanceTurnSlot(void)
{
    int savedIndex = D_8009CE3C;
    int kind;
    s16 i;
    s8 generation;
    D_8009CE44 = 0;
    kind = D_800BE830[savedIndex - 1].command;
    if (kind == 1) {
        if (D_8009D1DC == (D_8009D278->action->turnWord & 15)) {
            D_8009D2D8++;
            RewindTurn(savedIndex);
            RestorePending(savedIndex);
        } else {
            RewindTurn(savedIndex);
        }
        D_8009D1DC = D_8009D278->action->turnWord & 15;
    } else if (kind == 2) {
        int mode = D_8009D278->action->turnWord & 0xC0;
        if (mode == 0xC0) {
            D_8009D2D8++;
            D_8009CE3C = savedIndex - (u8)D_8009D2B0;
        } else if (mode == 0x40) {
            int word;
            D_8009D2D8++;
            word = D_8009D278->action->turnWord;
            D_8009CE3C = savedIndex - (((word & 15) * 3) >> 1);
        }
        D_8009D1DC = D_8009D278->action->turnWord & 15;
    } else if (kind < 0x197) {
        D_8009D2D8++;
        if (!D_8009CE60)
            BattleCmd_UndoPending();
        savedIndex = D_8009CE3C;
        {
            u8 current;
            D_8009CE3C = savedIndex - 1;
            for (;;) {
                current = D_8009CE3C;
                if (current == 0 ||
                    D_800BE830[current].actionGeneration != D_800BE830[current - 1].actionGeneration)
                    break;
                D_8009CE3C = current - 1;
            }
        }
        RestorePending(savedIndex);
        D_8009D1DC = D_8009D278->action->turnWord & 15;
    }
    generation = D_8009D2D8;
    for (i = 0; i < 45; i++) {
        if (D_800BE830[i].actionGeneration == generation) {
            D_800BE830[i].actor = 0;
            (&D_800BE830[i])->actionGeneration = 0;
            D_800BE830[i].command = 0;
        }
    }
}
