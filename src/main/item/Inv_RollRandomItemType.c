/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory_slots.h"
#include "pe1/random.h"

static inline int RandomByte(void) {
    int i;
    if (++g_ItemDropRngIndex >= 521) {
        for (i = 0; i < 32; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i + 489];
        for (i = 32; i < 521; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i - 32];
        g_ItemDropRngIndex = 0;
    }
    return g_ItemDropRngTable[g_ItemDropRngIndex];
}
void Inv_RollRandomItemType(s16 *type, s16 *amount) {
    /* Retail builds this list but only uses its count when choosing a type. */
    int available[3];
    int count;
    int i;
    u16 *stock;
    if (type && amount) {
        if (!*type) {
            count = 0;
            for (i = 0; i < 3; ++i)
                if (g_InvSpecialItemRecords[i].ammo) available[count++] = i;
            if (count) *type = ((count * RandomByte()) >> 8) + 1;
        }
        {
            u16 chosen = *type;
            if (chosen - 1U < 3U) {
                stock = &g_InvSpecialItemRecords[(s16)chosen - 1].ammo;
                if (!*amount) *amount = (*stock * RandomByte()) >> 8;
                else if (*amount < 0) *amount = (*stock * -*amount) / 100;
                if (*amount > *stock) *amount = *stock;
                *stock -= *amount;
            }
        }
    }
}
