/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/aya.h"
#include "pe1/inventory_slots.h"

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
s16 *D_8009D048;
s32 D_8009D050;

void Battle_SyncEquipSlots(void);
void *Aya_GetLevelExpTable(void);

void Inv_InitMaxLevelInventory(s32 arg0) {
    u16 *nextStat;
    u16 *stat;
    s32 allocation;
    s32 category;
    u16 maxHp;
    s32 mode;
    s16 *items;
    AyaSaveState *save;
    s32 unlockedSpells;
    AyaLevelStats *levelStats;
    s32 *expTable;

    mode = arg0;
    items = g_AyaInventoryItems;
    g_AyaInventorySlotCount[0] = 0x32;
    if (g_InvItemPtr == items) {
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    }
    g_AyaSaveLevel[0] = 0x62;
    expTable = (s32 *)Aya_GetLevelExpTable();
    g_AyaSaveTotalExp[0] = expTable[D_800C0E00.level];
    levelStats = Aya_LookupLevelStats(0x62);
    unlockedSpells = 0xFFFFF;
    save = (AyaSaveState *)((u8 *)items - PE1_OFFSETOF(AyaSaveState, inventory_items));
    nextStat = save->stats.levels;
    maxHp = levelStats->hp;
    category = 0;
    g_AyaParasiteSpellFlags[0] = unlockedSpells;
    g_AyaHpCurrent[0] = maxHp;
    g_AyaHpMax[0] = maxHp;
    do {
        stat = nextStat;
        nextStat = stat + 1;
        if (mode == 0) {
            allocation = 0x3E8;
        } else {
            allocation = category - 1U < 2U;
            if (allocation != 0) {
                allocation = 0;
            } else {
                allocation = 0x3E8;
            }
        }
        *stat = allocation;
        category += 1;
    } while (category < 7);
    Battle_SyncEquipSlots();
}
