#include "common.h"
#include "pe1/save.h"
#include "pe1/inventory_state.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

/* Item-drop random table: seeding from the play clock and the per-draw
 * lagged-xor refresh of the 0x209-byte table. */

int g_ItemDropRngIndex;

#define D_800A76A4_VALUE (D_800A76A4[0])

#define SCRAMBLE_RANDOM_BYTES()                    \
    do {                                           \
        int scramble_i_;                           \
        for (scramble_i_ = 0; scramble_i_ < 0x20; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyLow[scramble_i_]; \
        }                                          \
        for (scramble_i_ = 0x20; scramble_i_ < 0x209; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyHigh[scramble_i_]; \
        }                                          \
    } while (0)

void Inv_SetDefaultItemId(void) {
    u32 state[0x209];
    u32 seed;
    u32 bits;
    int bit;
    int index;

    seed = D_800A76A4_VALUE / 60;
    bits = 0;

    for (index = 0; index < 0x11; index++) {
        for (bit = 0x1F; bit >= 0; bit--) {
            seed = seed * 0x5D588B65 + 1;
            bits = (bits >> 1) | (seed & 0x80000000);
        }
        state[index] = bits;
    }

    state[0x10] = (state[0x10] << 23) ^ (state[0] >> 9) ^ state[0xF];

    for (index = 0x11; index < 0x209; index++) {
        state[index] = (state[index - 0x11] << 23) ^ (state[index - 0x10] >> 9) ^ state[index - 1];
    }

    for (index = 0; index < 0x209; index++) {
        g_ItemDropRngTable[index] = state[index];
    }

    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();
    SCRAMBLE_RANDOM_BYTES();

    g_ItemDropRngIndex = 0x208;
}

int Inv_ScrambleGrid(void)
{
    int i;

    g_ItemDropRngIndex++;
    if (g_ItemDropRngIndex >= 0x209) {
        for (i = 0; i < 0x20; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyLow[i];
        }
        for (i = 0x20; i < 0x209; i++) {
            g_ItemDropRngTable[i] = g_ItemDropRngTable[i] ^ g_ItemDropRngXorKeyHigh[i];
        }
        g_ItemDropRngIndex = 0;
    }

    return g_ItemDropRngTable[g_ItemDropRngIndex];
}
