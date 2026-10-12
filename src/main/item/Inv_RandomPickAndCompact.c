/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/inventory_slots.h"
#include "pe1/random.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
s32 g_ItemDropRngIndex;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvPendingCompactCount;
int g_SaveMetadataWindowIndex;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecord(g_InvItemPtr[index]);
    return 0;
}
static inline void RestoreList(int storage) {
    if (storage && g_InvActiveListOverride != 0) {
        g_InvItemPtr = g_InvActiveListOverride;
        g_InvSelectionBits = g_InvStorageSelectionBits;
        g_InvSelectionBitWords = 4;
        g_InvSlotLimit = g_InvOverrideSlotLimit;
    } else {
        g_InvItemPtr = g_AyaInventoryItems;
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
        g_InvSelectionBits = g_InvSelectionBitStorage;
        g_InvSelectionBitWords = 2;
    }
}

static inline int ListCount(void) {
    if (g_InvActiveListOverride) return 2;
    return 1;
}


static inline int Eligible(int index,int mask) {
    ItemDataRecord *item;
    if (index >= 0 && index < g_InvSlotLimit) item=Inv_LookupItemRecord(g_InvItemPtr[index]);
    else item=0;
    return item && ((mask >> item->kind) & 1) && item->tailCount != 0;
}
int Inv_SetupSlotDisplay(int mask) {
    int i=0, count=0;
    int wasStorage=g_InvItemPtr != g_AyaInventoryItems;
    RestoreList(0);
    for (; i<g_InvSlotLimit; ++i) count += Eligible(i,mask);
    if (ListCount() >= 2 && !(mask & 0x100)) {
        RestoreList(1);
        for (i=0; i<g_InvSlotLimit; ++i) count += Eligible(i,mask);
    }
    RestoreList(wasStorage);
    return count>=2;
}

static inline int RandomByte(void) {
    int i = 0;
    if (++g_ItemDropRngIndex >= 521) {
        for (; i < 32; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i + 489];
        for (i = 32; i < 521; ++i)
            g_ItemDropRngTable[i] ^= g_ItemDropRngTable[i - 32];
        g_ItemDropRngIndex = 0;
    }
    return g_ItemDropRngTable[g_ItemDropRngIndex];
}

static inline int IsTracked(int index) {
    int result = 0;
    if (g_InvItemPtr == g_AyaInventoryItems) {
        if (D_800C0E20.tracked[0] == index || D_800C0E20.tracked[2] == index)
            result = 1;
    }
    return result;
}
int Inv_PickRandomItem(int unused) {
    s16 *out = g_InvPackedList;
    int i;
    ItemDataRecord *item;
    for (i = 0; i < g_InvSlotLimit; ++i) {
        item = LookupActiveItem(i);
        if (item && (item->flags & 128) && !IsTracked(i)) *out++ = i;
    }
    g_InvPackedListCursor = 0;
    g_InvPackedListCount = out - g_InvPackedList;
    if (!g_InvPackedListCount) return 0;
    return g_InvItemPtr[g_InvPackedList[(g_InvPackedListCount * RandomByte()) >> 8]];
}

/* Historical name: formats the item message into the shared FF-terminated buffer. */
static inline void CopyText(u8 *dest, const u8 *src) {
    while ((*dest++ = *src++) != 255) {}
}
static inline void AppendText(u8 *dest, const u8 *src) {
    while (*dest++ != 255) {}
    --dest;
    while ((*dest++ = *src++) != 255) {}
}
u8 *Inv_GetItemEffectData(int item, int mode) {
    if (!item) CopyText(D_800A1B50, Str_LookupTable4(19));
    else if (mode >= 2) CopyText(D_800A1B50, Str_LookupTable4(18));
    else if (!g_SaveMetadataWindowIndex) {
        CopyText(D_800A1B50, Str_LookupTable8(item - 1));
        AppendText(D_800A1B50, Str_LookupTable4(mode + 16));
    } else {
        CopyText(D_800A1B50, Str_LookupTable4(mode + 16));
        AppendText(D_800A1B50, Str_LookupTable8(item - 1));
    }
    return D_800A1B50;
}

void Inv_CompactActiveListSlots(void) {
    int index, limit, from;
    register int slot;
    short *p, *end;

    while (g_InvPendingCompactCount > 0) {
        p = g_InvItemPtr;
        end = p + g_InvSlotLimit;
        if (p < end) {
            do {
                if (!*p) break;
                p++;
            } while (p < end);
            if (p < g_InvItemPtr + g_InvSlotLimit) {
                slot = p - g_InvItemPtr;
            } else {
                slot = -1;
            }
        } else {
            slot = -1;
        }
        index = slot;
        if (index >= 0) {
            if (g_InvBaseCapacity + Inv_GetBonusSlotCount() < 51) {
                limit = g_InvBaseCapacity + Inv_GetBonusSlotCount();
            } else {
                limit = 50;
            }
            from = limit - g_InvPendingCompactCount;
            g_InvItemPtr[index] = g_InvItemPtr[from];
            g_InvItemPtr[from] = 0;
            if (g_InvTrackedSlots[0] == from) {
                g_InvTrackedSlots[0] = index;
            } else if (g_InvTrackedSlots[2] == from) {
                g_InvTrackedSlots[2] = index;
            }
        }
        g_InvPendingCompactCount--;
    }
}
