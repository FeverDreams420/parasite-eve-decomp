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

void Battle_SyncEquipSlots(void);



void Save_SelectMetadataWindow(ItemDataRecord *record) {
    SaveMetadataWindow *window;

    g_SaveMetadataSourceRecord = record;
    if (record != 0) {
        window = g_CursorRenderMetadataWindows;
        if (record->kind == ITEM_KIND_ARMOR) {
            window++;
        }
    } else {
        window = g_SaveRuntimeState.windows.metadata;
        if (g_SaveMetadataWindowIndex != 0) {
            window++;
        }
    }

    g_SaveMetadataCursor = window->text;
    if (record == 0) {
        g_SaveMetadataWindowLength = g_SaveMetadataWindowIndex != 0 ? 8 : 6;
    } else {
        g_SaveMetadataWindowLength = 6;
    }
}