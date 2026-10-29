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

int Save_GetCurrentMapNumber(void) {
    char roomName[8];

    Text_EncodeBase32(roomName, g_SceneDispatchToken);
    return Text_ParseMapNumber(roomName);
}

void Menu_InitScrollDirection(void) {
    g_MenuScrollStep = g_MenuScrollDirectionValueUnwritten < 0x12C ? 4 : -4;
}

void Battle_SyncEquipSlots(void);

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
    expTable = Aya_GetLevelExpTable();
    g_AyaSaveTotalExp[0] = expTable[g_AyaSaveState.level];
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