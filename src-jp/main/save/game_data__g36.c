/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_bios.h"
#include "pe1/save.h"
#include "pe1/scene_dispatch.h"
#include "pe1/scene_room.h"
#include "pe1/text.h"
#include "include_asm.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvCategoryBaseItemId;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvRebuildFilterMode;
s32 g_InvRebuildMinFirstStat;
int g_MenuBattleEquipMode;
u8 *g_SaveMetadataCursor;
ItemDataRecord *g_SaveMetadataSourceRecord;
int g_SaveMetadataWindowIndex;
int g_SaveMetadataWindowLength;
int g_MenuScrollDirectionValueUnwritten;
int g_MemCardDialogState;
int g_MenuScrollStep;

int g_SaveMetadataWindowIndex;

SavedMenuMode g_SavedMenuMode;

/* Open the player inventory using the same saved menu mode. */

void BattleCmd_SyncActiveAmmo(void);
void Menu_CreateInventoryTabView(void);

int Menu_GetItemContextFlag(void);
int Save_ClassifyPlaytime(int arg0);

void Menu_CopyPromptCodes(u8 *src);

u32 g_MenuErrorSoundPending;

static inline void SelectAya(void) {
    g_InvItemPtr = g_AyaSaveState.inventory_items;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
}

/* Carry a tagged record forward, removing its first storage-list reference. */
static inline int TakeTagged(int armor) {
    ItemDataRecord *p;
    int id = 0;
    for (p = g_InventoryRuntime.equipment; p < g_InventoryRuntime.equipment + 128; p++) {
        if ((armor ? p->kind == 9 : p->kind != 9) && (p->flags & 16)) break;
    }
    if (p < g_InventoryRuntime.equipment + 128) {
        s16 *slot;
        id = (p - g_InventoryRuntime.equipment) + 256;
        for (slot = g_InventoryRuntime.specialStorage;
             slot < g_InventoryRuntime.specialStorage + 82; slot++)
            if (*slot == id) break;
        if (slot < g_InventoryRuntime.specialStorage + 82) *slot = 0;
    }
    return id;
}

/* Remove the selected record, preserving armor-capacity bookkeeping. JP
 * releases the reserved armor slots before clearing the record. */
static inline void RemoveItem(int index) {
    int required, id;
    if (g_InvItemPtr == g_AyaInventoryItems && g_InventoryRuntime.tracked[2] == index) {
        Inv_GetActiveSlotCount(&required);
        Inv_CheckFreeSlotCapacity(required);
        g_InventoryRuntime.tracked[2] = -1;
        Inv_SetActiveList(3, 0);
    }
    id = g_InvItemPtr[index];
    g_InvItemPtr[index] = 0;
    if (id >= 256) g_InventoryRuntime.equipment[id - 256].iconId = 0;
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
    g_AyaSaveState.inventory_slot_count = value;
    if (g_InvItemPtr == g_AyaInventoryItems) g_InvSlotLimit = Inv_GetAyaSlotLimit();
}
static inline void CopyName(u8 *dest, u8 *source) {
    while ((*dest++ = *source++) != 255) {}
}

static inline void InitName(u8 *buffer, int length, ItemDataRecord *item) {
    u8 *p, *source, *dest;
    g_SaveMetadataWindowIndex = 0;
    g_SaveMetadataSourceRecord = item;
    g_SaveMetadataCursor = buffer;
    g_SaveMetadataWindowLength = length;
    for (p = g_SaveMetadataCursor;
         p < g_SaveMetadataCursor + g_SaveMetadataWindowLength; p++)
        *p = 255;
    dest = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord)
        source = Text_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
    else
        source = Text_LookupTable4(30);
    CopyName(dest, source);
}

void Battle_SyncEquipSlots(void);
int Inv_RebuildWithBonusSlots(int minSecond, int minThird);
int Inv_TransferToStorage(void);



void Inv_InitNewGameInventory(void) {
    int i, capacity;
    AyaLevelStats *stats;
    u16 *growth = g_AyaStatAgility;
    for (i = 0; i < 7; i++) *growth++ = *(u16 *)Aya_GetStatGrowthTable(i);
    stats = Aya_LookupLevelStats(0);
    g_AyaSaveState.max_hp = g_AyaSaveState.current_hp = stats->hp;
    g_AyaSaveState.inventory_slot_count = stats->inventoryCapacity;
    g_AyaSaveState.parasite_spell_flags = 1;
    g_InvSpecialItemRecords[0].ammo = g_InvSpecialItemRecords[1].ammo = g_InvSpecialItemRecords[2].ammo = 0;
    SelectAya();
    capacity = stats->inventoryCapacity;
    if (capacity > 50) capacity = 50;
    g_AyaSaveState.inventory_slot_count = capacity;
    if (g_InvItemPtr == g_AyaSaveState.inventory_items)
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
    g_InventoryRuntime.tracked[2] = 1;
    g_InventoryRuntime.tracked[0] = 0;
    g_AyaSaveState.menu_clamp_value = 61;
    Menu_ClampRange(61);
    g_AyaSaveState.total_exp = 0;
    g_AyaSaveState.level = 0;
    Menu_SaveBgInitFade();
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
            g_InventoryRuntime.equipment[(short)id - 256].itemId == 0x61) break;
    }
    if (i < g_InvSlotLimit) RemoveItem(i);
    Inv_AddItem(0x93);
    g_InventoryRuntime.tracked[0] = FindKind(7, -1);
    g_MenuBattleEquipMode = 0;
    Inv_SetActiveList(2, 0);
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
            count = g_InvSpecialItemRecords[value - g_InvCategoryBaseItemId].ammo;
        else for (p = g_InvItemPtr; p < g_InvItemPtr + g_InvSlotLimit; p++) {
            unsigned short id = *p;
            if (id - 256U < 128U)
                count += (g_InventoryRuntime.equipment[(short)id - 256].itemId == value);
            else
                count += ((short)id == value);
        }
        return count;
    }
    case INV_CMD_SET_REBUILD_FILTER:
        g_InvRebuildFilterMode = value;
        g_InvRebuildMinFirstStat = other;
        return 0;
    case INV_CMD_REBUILD_SLOTS:
        return Inv_RebuildWithBonusSlots(value, other);
    case INV_CMD_SET_CAPACITY:
        SetCapacity(value);
        return 0;
    case INV_CMD_GET_CURRENT_HP:
        return g_AyaSaveState.current_hp;
    case INV_CMD_GET_MAX_HP:
        return g_AyaSaveState.max_hp;
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
        MenuInput_InitQueue();
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
        Menu_SetMemCardConfirmPending();
        if (g_AyaSaveState.new_game_count + 1 < 100) g_AyaSaveState.new_game_count++;
        else g_AyaSaveState.new_game_count = 99;
        Inv_InitNewGameInventory();
        return 0;
    case INV_CMD_INIT_BONUS_POINTS:
        Menu_InitBonusPointAllocState(value);
        return 0;
    case INV_CMD_CLEAR_STORAGE_ITEM: {
        s16 *p;
        int missing;
        for (p = g_InventoryRuntime.storage; p < g_InventoryRuntime.storage + 100; p++)
            if (*p == value) break;
        missing = !(p < g_InventoryRuntime.storage + 100);
        if (!missing) *p = 0;
        return missing;
    }
    case INV_CMD_MERGE_STORAGE:
        Inv_MergeStorageToSlot();
        /* Fall through: this command returns 0. */
    }
    return 0;
}

void Save_InitMetadataState(void) {
    bzero(&g_SaveRuntimeState, sizeof(g_SaveRuntimeState));
    /* Retail initializes the same window twice, including both lookups. JP
     * fills the six-character save name and leaves the prompt codes alone. */
    InitName(g_SaveRuntimeState.windows.header.saveName, 6, 0);
    InitName(g_SaveRuntimeState.windows.header.saveName, 6, 0);
    Inv_InitNewGameInventory();
    g_InventoryRuntime.specialStorage[0] = 515;
    g_AyaSaveState.blend_color = 0x404040;
    Draw_BlendColor(0x404040);
    g_GameTimers[0].current = 0;
    g_GameTimers[1].current = 0;
    g_GameTimers[2].current = 0;
    g_GameTimers[3].current = 0;
    Draw_SetBaseOffsetPosition(0, 8 - Draw_GetBaseY());
    MenuWidget_SetColumnLayoutMode(0);
    Menu_SetActionSubmenuSelection(1);
    g_CursorRenderDataBlock[0] = 255;
    g_CursorRenderDataBlock[0x10] = 255;
}