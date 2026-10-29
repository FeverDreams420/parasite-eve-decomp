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

int Aya_StatBinarySearch(int value, int *table) {
    int step;
    int index;
    int result;

    index = 0x40;
    step = 0x20;

    while (step != 0) {
        if (value >= table[index]) {
            index += step;
        } else if (value < table[index - 1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    }

    index--;
    result = 0x62;
    if (index < 0x63) {
        result = index;
    }

    return result;
}

static inline int InterpolateSubLevel(int value, int *cursor) {
    int current_reg;
    int next_reg;
    int delta_reg;
    int numerator_reg;
    next_reg = cursor[1];
    current_reg = cursor[0];
    if (next_reg != current_reg) {
        delta_reg = value - current_reg;
        numerator_reg = ((delta_reg << 1) + delta_reg) * 16 + delta_reg;
        return numerator_reg / (next_reg - current_reg);
    }
    return 0;
}

void Aya_StatQueryLevelAndSubLevel(int kind, int value, int *level_out, int *sublevel_out) {
    int saved_value;
    int *level_ptr;
    int *sublevel_ptr;
    int *table;
    int index;
    int step;
    int offset;
    int *loop_cursor;
    int *cursor;
    int sublevel;

    saved_value = value;
    level_ptr = level_out;
    sublevel_ptr = sublevel_out;
    table = Aya_GetStatGrowthTable(kind);

    index = 0x40;
    step = 0x20;
    do {
        offset = index << 2;
        loop_cursor = (int *)(offset + (int)table);
        if (saved_value >= loop_cursor[0]) {
            index += step;
        } else if (saved_value < loop_cursor[-1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    } while (step != 0);

    index--;
    step = 0x62;
    if (index < 0x63) {
        step = index;
    }
    index = step;

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        sublevel = InterpolateSubLevel(saved_value, cursor);
    } else {
        sublevel = 0;
    }

    if (sublevel >= 0x31) {
        sublevel = 0x30;
    }

    if (level_ptr != 0) {
        *level_ptr = index;
    }
    if (sublevel_ptr != 0) {
        if (index < 0x62) {
            *sublevel_ptr = sublevel;
        } else {
            *sublevel_ptr = 0x30;
        }
    }
}

void Aya_StatQueryDistanceAndSubLevel(int kind, int value, int *distance_out, int *sublevel_out) {
    int saved_value;
    int *distance_ptr;
    int *sublevel_ptr;
    int *table;
    int index;
    int step;
    int offset;
    int *loop_cursor;
    int *cursor;
    int sublevel;
    int distance;

    saved_value = value;
    distance_ptr = distance_out;
    sublevel_ptr = sublevel_out;
    table = Aya_GetStatGrowthTable(kind);

    index = 0x40;
    step = 0x20;
    do {
        offset = index << 2;
        loop_cursor = (int *)(offset + (int)table);
        if (saved_value >= loop_cursor[0]) {
            index += step;
        } else if (saved_value < loop_cursor[-1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    } while (step != 0);

    index--;
    step = 0x62;
    if (index < 0x63) {
        step = index;
    }
    index = step;

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        distance = cursor[1] - saved_value;
    } else {
        distance = 0;
    }

    if (index < 0x62) {
        offset = index << 2;
        cursor = (int *)(offset + (int)table);
        sublevel = InterpolateSubLevel(saved_value, cursor);
    } else {
        sublevel = 0;
    }

    if (distance_ptr != 0) {
        *distance_ptr = distance;
    }
    if (sublevel_ptr != 0) {
        if (index < 0x62) {
            *sublevel_ptr = sublevel;
        } else {
            *sublevel_ptr = 0x30;
        }
    }
}

int Aya_StatOverflowQuery(int arg0, int value) {
    int *table;
    int step;
    int index;
    int result;

    table = Aya_GetStatGrowthTable(arg0);
    index = 0x40;
    step = 0x20;

    while (step != 0) {
        if (value >= table[index]) {
            index += step;
        } else if (value < table[index - 1]) {
            index -= step;
        } else {
            step = 0;
        }
        step >>= 1;
    }

    index--;
    result = 0x62;
    if (index < 0x63) {
        result = index;
    }

    if (result <= 0) {
        return 0;
    }
    return value - table[result - 1];
}

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