/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/stat_modifiers.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointDisplayValue;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_InvItemUsableFlag;
MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_InvSelectedItemIndex;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSaveBgFadeIndex;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeTint;

/* Save-screen background fade: gamma curve, per-frame darkening of the
 * captured frame and the fade state machine setters. Contiguous at
 * 0x80042C78 and joined by the fade state words. MemCard_InitSlotState
 * (historical name) arms the fade-in. */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeHeight;

int g_MenuSaveBgFadeState;

int g_MenuItemSelectFlag;

#define NULL ((void *)0)


int g_MenuItemContextFlag;

void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);

#define CLOSE_SELECT_VIEWS() { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
}



void Menu_DrawSaveSlotEntry(s32 arg0) {
    register MemCardSaveSlot *slot;
    s32 index;
    int dx;
    int text_id;
    int dy;
    u8 state;

    index = arg0;
    slot = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, index);
    if (slot != NULL) {
        state = slot->state;
        switch (state) {
        case MEMCARD_SLOT_OCCUPIED:
            if (index == MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
                Menu_SetSaveSlotBlendBase(slot->blendColor);
            } else {
                Draw_OffsetCursor(-2, -2);
                Draw_BlendColor(slot->blendColor);
                Draw_EmitWipeBarRect(g_MenuActiveWidget->draw_state, g_MenuActiveWidget->disabled, 1);
                Draw_BlendColor(g_SavedDrawBlendColor[0]);
                Draw_OffsetCursor(2, 2);
            }
            Draw_OffsetCursor(2, 2);
            if (Save_GetMetadataWindowIndex() == 0) {
                Draw_PrintRawText(slot->primaryTitle.text);
            } else {
                Draw_PrintRawText(slot->alternateTitle.text);
            }
            Draw_OffsetCursor(0x54, 0);
            if (slot->exGameIndex != 0) {
                Draw_AllocSprite(0x95);
                Draw_OffsetCursor(0x1A, 2);
                Draw_AllocSprite(0x96);
                Draw_OffsetCursor(0x22, 1);
                Draw_PrintNumberWidth2Unk(slot->exGameIndex + 1);
                dx = 4;
                dy = -3;
            } else {
                dx = 0x4A;
                dy = 0;
            }
            Draw_OffsetCursor(dx, dy);
            Draw_PrintTextById(0x5A);
            Draw_OffsetCursor(0x19, 0);
            Draw_PrintNumberWidth2(index + 1);
            Draw_OffsetCursor(-0xC0, 0xF);
            Draw_AllocSprite(0x53);
            Draw_OffsetCursor(0x1E, 1);
            Draw_PrintNumberWidth3Unk(slot->levelIndex + 1);
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x54);
            Draw_OffsetCursor(0x10, 1);
            Draw_PrintNumberWidth4Unk(slot->currentHp);
            Draw_AllocSprite(0x4C);
            Draw_OffsetCursor(5, 0);
            Draw_PrintNumberWidth4Unk(slot->maxHp);
            Draw_OffsetCursor(8, -1);
            Draw_AllocSprite(0x55);
            Draw_OffsetCursor(0x1A, 1);
            Draw_PrintTimeValue(slot->playTimeMinutes, 0);
            Draw_OffsetCursor(-0xC0, 0xC);
            if (slot->titleStyleFlag != 0) {
                Draw_PrintTextById(0x60);
                return;
            }
            Draw_PrintTextById(0xB);
            Draw_OffsetCursor(0x1E, 0);
            Draw_PrintNumberWidth2(slot->progressStage);
            Draw_OffsetCursor(0x14, 0);
            Draw_PrintRawText(Text_LookupMapLocationText(slot->mapNumber));
            return;
        case MEMCARD_SLOT_EMPTY:
            Draw_OffsetCursor(0, 0x12);
            text_id = 0x41;
            MenuWidget_DrawCenteredTableText(text_id);
            Draw_OffsetCursor(0x5C, -0x10);
            break;
        }
    }
}