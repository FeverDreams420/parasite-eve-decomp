/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvAmmoSpendActiveList;
int g_InvItemUsableFlag;
int g_InvSelectedItemIndex;
int g_InvSwapSourceList;
int g_InvSwapTargetIndex;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;

#define NULL ((void *)0)

int Menu_AmmoSpendInputHandler(s32 arg0, s32 arg1);
void Menu_DrawAmmoSpendPanel(void);

int g_MenuSpendArrowDirection;
#undef NULL


/* The retail display clamps only the upper bound; negative totals survive. */
static inline int clampedStatTotal(ItemDataRecord *item, int index)
{
    int value = item->baseStats[index] + item->bonusStats[index];
    if (value > 999)
        value = 999;
    return value;
}

void Menu_CreateInventoryListView(MenuWidgetNode *parent) {
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (g_MenuLayoutLocked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x14);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
}

void Menu_DrawItemDetailPanel(MenuWidgetNode *panel) {
    MenuWidgetNode *selection;
    ItemDataRecord *item;
    int index;
    int base;
    int panel_y;
    /* Match debt: retail computes dx right after its two loads, in the
     * argument registers; unpinned it is scheduled after the tail-count
     * chain (lev 5). */
    register int dx asm("$5");
    register int dy asm("$6");
    int sum;
    int displayed;
    MenuWidgetNode *arg_panel;
    int panel_offset;

    if (!MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 7))) {
        selection = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (selection == 0) selection = MenuWidget_FindByModeAndSelectedBase(1, 0xB);

        if (MenuWidget_FindByModeAndSelectedBase(1, 0xB) == 0 &&
            MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 7)) >= 0) {
            index = Inv_GetPackedListItem(MenuWidget_GridCellIndex(
                MenuWidget_FindByModeAndSelectedBase(2, 7)));
        } else {
            index = Inv_RestoreSelection(0);
        }

        item = Inv_LookupActiveListData(index);
        arg_panel = panel;
        dx = selection->x - arg_panel->x;
        dy = ((item->tailCount + 1) >> 1) * 16;
        panel_offset = arg_panel->y - 0x6C;
        dy -= panel_offset;
    } else {
        base = g_MenuEquipMode ? 0x38 : 0x24;
        panel_y = panel->y;
        if (Inv_GetPackedListCount() < 9) {
            panel_offset = base + Inv_GetPackedListCount() * 16;
            dy = panel_offset - panel_y;
        } else {
            panel_offset = panel_y - 0x80;
            dy = base - panel_offset;
        }
        arg_panel = panel;
        dx = 0;
    }

    MenuWidget_OffsetPosition(arg_panel, dx, dy);
    Draw_OffsetCursor(4, 4);
    Draw_PrintTextById(0x6A);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintTextById(0x6B);

    sum = g_StatBaseTable[0] + g_StatBaseTable[2];
    if (sum < 100 && !g_MenuEquipMode) {
        Draw_OffsetCursor(-0xF, 3);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(-0x14, 3);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }

    sum = g_StatBaseTable[1] + g_StatBaseTable[3];
    if (sum < 100 && !g_MenuEquipMode) {
        Draw_OffsetCursor(0x32, 0);
        Draw_PrintNumberWidth2Unk(sum);
    } else {
        Draw_OffsetCursor(0x2D, 0);
        displayed = 999;
        if (sum < 1000) displayed = sum;
        Draw_PrintNumberWidth3Unk(displayed);
    }
}