/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/text.h"
#include "pe1/memcard.h"
#include "pe1/menu_widget.h"

#define NULL ((void *)0)

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
            Draw_OffsetCursor(0x58, 0);
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
            Draw_OffsetCursor(-0xC4, 0xF);
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
            Draw_PrintRawText(Str_LookupMapLocationText(slot->mapNumber));
            return;
        case MEMCARD_SLOT_EMPTY:
            Draw_OffsetCursor(0, 0x12);
            text_id = 0x41;
            MenuWidget_DrawCenteredTableText(text_id);
            Draw_OffsetCursor(0x5C, -0x10);
            break;
        case MEMCARD_SLOT_UNAVAILABLE:
            Draw_OffsetCursor(0, 0x12);
            text_id = 0x76;
            MenuWidget_DrawCenteredTableText(text_id);
            Draw_OffsetCursor(0x5C, -0x10);
            break;
        }
    }
}
