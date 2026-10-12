/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/inventory_slots.h"
#include "pe1/aya.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_state.h"
#include "pe1/menu_queue.h"
#include "pe1/memcard.h"
#include "pe1/game_state.h"
#include "pe1/save.h"
#include "pe1/text.h"
#include "pe1/psyq_bios.h"
#include "pe1/draw_state.h"

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
int g_InvCategoryBaseItemId;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 D_8009D0CC;
s32 D_8009D0D0;
int g_MenuBattleEquipMode;
u8 *g_SaveMetadataCursor;
ItemDataRecord *g_SaveMetadataSourceRecord;
int g_SaveMetadataWindowIndex;
int g_SaveMetadataWindowLength;


static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
    return 0;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < g_InvSelectionBitWords; i++) g_InvSelectionBits[i] = 0;
}

/* Build the threshold-qualified set; negate its count if a tagged record exists. */
int Inv_RebuildWithBonusSlots(int minSecond, int minThird) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(D_8009D0CC ? 0x1FE : 0x200);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->baseStats[0] + item->bonusStats[0] >= D_8009D0D0 &&
            item->baseStats[1] + item->bonusStats[1] >= minSecond &&
            item->baseStats[2] + item->bonusStats[2] >= minThird &&
            item->itemId != 0x93 && item->itemId != 0x61) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    if (count) {
        ItemDataRecord *scan;
        if (D_8009D0CC) {
            scan = D_800C0E20.equipment;
            for (; scan < D_800C0E20.equipment + 128 &&
                   !(scan->iconId && scan->kind < 9 && (scan->flags & 0x10));
                 scan++)
                ;
        } else {
            scan = D_800C0E20.equipment;
            for (; scan < D_800C0E20.equipment + 128 &&
                   !(scan->iconId && scan->kind == 9 && (scan->flags & 0x10));
                 scan++)
                ;
        }
        if (scan < D_800C0E20.equipment + 128) count = -count;
    }
    g_InvPackedListCursor = count != 0;
    return count;
}

/* Historical name: mark filtered records whose tail count is below byte 1. */
int Inv_TransferToStorage(void) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(0x3FE);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->tailCount < item->tailCapacity) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    return count;
}

static inline void SelectAya(void) {
    g_InvItemPtr = D_800C0E00.inventory_items;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
}

/* Carry a tagged record forward, removing its first storage-list reference. */
static inline int TakeTagged(int armor) {
    ItemDataRecord *p;
    int id = 0;
    for (p = D_800C0E20.equipment; p < D_800C0E20.equipment + 128; p++) {
        if (p->iconId && (armor ? p->kind == 9 : p->kind != 9) && (p->flags & 16)) break;
    }
    if (p < D_800C0E20.equipment + 128) {
        s16 *slot;
        id = (p - D_800C0E20.equipment) + 256;
        for (slot = D_800C0E20.specialStorage;
             slot < D_800C0E20.specialStorage + 82; slot++)
            if (*slot == id) break;
        if (slot < D_800C0E20.specialStorage + 82) *slot = 0;
    }
    return id;
}

void Inv_InitNewGameInventory(void) {
    int i, capacity;
    AyaLevelStats *stats;
    u16 *growth = g_AyaStatAgility;
    for (i = 0; i < 7; i++) *growth++ = *(u16 *)Stat_GetGrowthTable(i);
    stats = Aya_LookupLevelStats(0);
    D_800C0E00.max_hp = D_800C0E00.current_hp = stats->hp;
    D_800C0E00.inventory_slot_count = stats->inventoryCapacity;
    D_800C0E00.parasite_spell_flags = 1;
    D_800A1E64[0].ammo = D_800A1E64[1].ammo = D_800A1E64[2].ammo = 0;
    SelectAya();
    capacity = stats->inventoryCapacity;
    if (capacity > 50) capacity = 50;
    D_800C0E00.inventory_slot_count = capacity;
    if (g_InvItemPtr == D_800C0E00.inventory_items)
        g_InvSlotLimit = Inv_GetAyaSlotLimit();
    SelectAya();
    for (i = 49; i >= 0; i--) g_InvItemPtr[i] = 0;
    Inv_AddItem(0x44);
    Inv_AddItem(0x96);
    Inv_AddItem(0x3F);
    Inv_AddItem(1);
    Inv_AddItem(6);
    i = TakeTagged(0);
    if (i) Inv_AddItem(i);
    i = TakeTagged(1);
    if (i) Inv_AddItem(i);
    D_800C0E20.tracked[2] = 1;
    D_800C0E20.tracked[0] = 0;
    D_800C0E00.menu_clamp_value = 61;
    Menu_ClampRange(61);
    D_800C0E00.total_exp = 0;
    D_800C0E00.level = 0;
    Menu_SaveBgInitFade();
}



/* Remove the selected record, preserving armor-capacity bookkeeping. */
static inline void RemoveItem(int index) {
    int required, id;
    if (g_InvItemPtr == g_AyaInventoryItems && D_800C0E20.tracked[2] == index)
        Inv_GetActiveSlotCount(&required);
    id = g_InvItemPtr[index];
    g_InvItemPtr[index] = 0;
    if (id >= 256) D_800C0E20.equipment[id - 256].iconId = 0;
    if (g_InvItemPtr == g_AyaInventoryItems && D_800C0E20.tracked[2] == index) {
        D_800C0E20.tracked[2] = -1;
        Inv_CheckFreeSlotCapacity(required);
        Inv_CompactActiveListSlots();
        Inv_SetActiveList(3, 0);
    }
}

static inline int FindKind(int kind, int excluded) {
    /* excluded is -1 when no slot is skipped; the address then lies before
     * the list and never matches. */
    s16 *skip = g_InvItemPtr + excluded;
    s16 *p = g_InvItemPtr;
    while (p < g_InvItemPtr + g_InvSlotLimit) {
        if (p != skip && Inv_LookupItemRecordScaled(*p)->kind == kind) break;
        p++;
    }
    if (p < g_InvItemPtr + g_InvSlotLimit) return p - g_InvItemPtr;
    return -1;
}

void Inv_MergeStorageToSlot(void) {
    int i;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    for (i = 0; i < g_InvSlotLimit; i++) {
        unsigned short id = g_InvItemPtr[i];
        if (id - 256U < 128U &&
            D_800C0E20.equipment[(short)id - 256].itemId == 0x61) break;
    }
    if (i < g_InvSlotLimit) RemoveItem(i);
    Inv_AddItem(0x93);
    D_800C0E20.tracked[0] = FindKind(7, -1);
    g_MenuBattleEquipMode = 0;
    Inv_SetActiveList(2, 0);
}

/* The caller supplies a fourth script argument; these commands ignore it. */
static inline int FindItem(int id) {
    s16 *p = g_InvItemPtr;
    while (p < g_InvItemPtr + g_InvSlotLimit) {
        if (*p == id) break;
        p++;
    }
    if (p < g_InvItemPtr + g_InvSlotLimit) return p - g_InvItemPtr;
    return -1;
}

static inline void SetCapacity(int value) {
    if (value > 50) value = 50;
    D_800C0E00.inventory_slot_count = value;
    if (g_InvItemPtr == g_AyaInventoryItems) g_InvSlotLimit = Inv_GetAyaSlotLimit();
}

int Inv_DispatchCommand(int command, int value, int other, int *unused) {
    switch (command) {
    case INV_CMD_COUNT_OCCUPIED: {
        s16 *p;
        int count = 0;
        SelectAya();
        for (p = g_InvItemPtr; p < g_InvItemPtr + g_InvSlotLimit; p++)
            count += (*p != 0);
        return count;
    }
    case INV_CMD_GET_CAPACITY: {
        int bonus = Inv_GetBonusSlotCount();
        if (g_InvBaseCapacity + bonus < 51) {
            bonus = Inv_GetBonusSlotCount();
            return g_InvBaseCapacity + bonus;
        }
        return 50;
    }
    case INV_CMD_COUNT_ITEM: {
        int count = 0;
        s16 *p;
        if (value >= g_InvCategoryBaseItemId && value < g_InvCategoryBaseItemId + 3)
            count = D_800A1E64[value - g_InvCategoryBaseItemId].ammo;
        else for (p = g_InvItemPtr; p < g_InvItemPtr + g_InvSlotLimit; p++) {
            unsigned short id = *p;
            if (id - 256U < 128U)
                count += (D_800C0E20.equipment[(short)id - 256].itemId == value);
            else
                count += ((short)id == value);
        }
        return count;
    }
    case INV_CMD_SET_REBUILD_FILTER:
        D_8009D0CC = value;
        D_8009D0D0 = other;
        return 0;
    case INV_CMD_REBUILD_SLOTS:
        return Inv_RebuildWithBonusSlots(value, other);
    case INV_CMD_SET_CAPACITY:
        SetCapacity(value);
        return 0;
    case INV_CMD_GET_CURRENT_HP:
        return D_800C0E00.current_hp;
    case INV_CMD_GET_MAX_HP:
        return D_800C0E00.max_hp;
    case INV_CMD_SET_CURRENT_HP:
        BattleCmd_SetCurrentHP(value);
        return 0;
    case INV_CMD_GET_REMAINING_AMMO:
        return BattleCmd_GetRemainingAmmo(0);
    case INV_CMD_GET_AMMO_VALUE: {
        int result;
        BattleCmd_GetRemainingAmmo(&result);
        return result;
    }
    case INV_CMD_SET_CURRENT_MP:
        BattleCmd_SetCurrentMP(value);
        return 0;
    case INV_CMD_REMOVE_ITEM: {
        int index;
        SelectAya();
        index = FindItem(value);
        if (index < 0) return index;
        Inv_RemoveActiveListItem(index);
        return 0;
    }
    case INV_CMD_OPEN_STORAGE:
        Inv_TransferToStorage();
        Queue_Init();
        Menu_StepInventoryRoot(0, -3, -1);
        Menu_CreateContextHelpPanel();
        return 0;
    case INV_CMD_COMPUTE_GAMMA:
        Menu_ComputeGammaLut(value, other);
        return 0;
    case INV_CMD_INIT_MEMCARD:
        MemCard_InitSlotState();
        return 0;
    case INV_CMD_START_SAVE_FADE:
        Menu_SaveBgStartFadeOut();
        return 0;
    case INV_CMD_START_NEW_GAME:
        Inv_RebuildSelectionBitset();
        g_GameState.pending_story_day = 1;
        Menu_SetMemCardConfirmPending();
        if (D_800C0E00.pad_0B + 1 < 100) D_800C0E00.pad_0B++;
        else D_800C0E00.pad_0B = 99;
        Inv_InitNewGameInventory();
        return 0;
    case INV_CMD_INIT_BONUS_POINTS:
        Menu_InitBonusPointAllocState(value);
        return 0;
    case INV_CMD_CLEAR_STORAGE_ITEM: {
        s16 *p;
        int missing;
        for (p = D_800C0E20.storage; p < D_800C0E20.storage + 100; p++)
            if (*p == value) break;
        missing = !(p < D_800C0E20.storage + 100);
        if (!missing) *p = 0;
        return missing;
    }
    case INV_CMD_MERGE_STORAGE:
        Inv_MergeStorageToSlot();
        /* Fall through: this command returns 0. */
    }
    return 0;
}
static inline void CopyName(u8 *dest, u8 *source) {
    while ((*dest++ = *source++) != 255) {}
}

static inline void InitName(u8 *buffer, int length, ItemDataRecord *item) {
    u8 *p, *source, *dest;
    g_SaveMetadataWindowIndex = 1;
    g_SaveMetadataSourceRecord = item;
    g_SaveMetadataCursor = buffer;
    g_SaveMetadataWindowLength = length;
    for (p = g_SaveMetadataCursor;
         p < g_SaveMetadataCursor + g_SaveMetadataWindowLength; p++)
        *p = 255;
    dest = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord)
        source = Str_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
    else
        source = Str_LookupTable4(30);
    CopyName(dest, source);
}

void Save_InitMetadataState(void) {
    bzero(&g_SaveRuntimeState, sizeof(g_SaveRuntimeState));
    /* Retail initializes the same window twice, including both lookups. */
    InitName(g_SaveRuntimeState.windows.metadata[1].text, 8, 0);
    InitName(g_SaveRuntimeState.windows.metadata[1].text, 8, 0);
    Menu_CopyPromptCodes(Str_LookupTable4(30));
    Inv_InitNewGameInventory();
    D_800C0E20.specialStorage[0] = 515;
    D_800C0E00.blend_color = 0x404040;
    Draw_BlendColor(0x404040);
    D_800A76A4[0] = 0;
    D_800A76B0[0] = 0;
    D_800A76BC[0] = 0;
    D_800A76C8[0] = 0;
    Draw_SetBaseOffsetPosition(0, 8 - Draw_GetBaseY());
    MenuWidget_SetColumnLayoutMode(0);
    Menu_SetActionSubmenuSelection(1);
    g_CursorRenderDataBlock[0] = 255;
    D_800C20B4[0] = 255;
}
