/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/battle_cmd.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/memcard.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"
#include "pe1/text.h"

/* Item-slot availability, modifier, armor and memory-card port list draw
 * callbacks, and the port and save-slot selectability filters. */

MenuWidgetNode *g_MenuActiveWidget;
int g_McDialogMode;

void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(int index);
void Menu_DrawArmorListItem(int arg0);
void Menu_DrawArmorEquipItem(int arg0);
void Menu_DrawMemCardPortItem(int arg0);

int Menu_GetItemSlotAvailability(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        return Menu_CheckItemAffordable(Inv_GetPackedListItem(arg0));
    }

    return 0;
}

void Menu_DrawEquipModList(MenuWidgetNode *arg0) {
    int ammo;
    MenuWidgetNode *saved_arg;
    int remaining;
    int item;

    saved_arg = arg0;
    item = Inv_GetPackedListItem(MenuWidget_GridCellIndex(arg0));
    remaining = BattleCmd_GetRemainingAmmo(&ammo);
    {
        int highlighted = Inv_GetSlotHighlightState(item, remaining);
        remaining -= highlighted;
    }
    Draw_AllocColorQuad(remaining, ammo);
    MenuWidget_DrawList(saved_arg, Menu_DrawModNameItem);
}

void Menu_DrawArmorListContainer(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawArmorListItem);
    Draw_SetTextDimmed(1);
    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

void Menu_DrawArmorSelectionList(MenuWidgetNode *node) {
    Inv_SelectActiveList(1);
    MenuWidget_DrawList(node, Menu_DrawArmorEquipItem);
}

int Menu_IsMemCardSlotSelectable(int arg0) {
    int ret;

    ret = 0;
    if (arg0 == 2) {
        ret = 1;
    } else if (MemCard_IsPortPresent(arg0) != 0) {
        ret = 1;
    }
    return ret;
}

void Menu_DrawMemCardPortList(MenuWidgetNode *node) {
    int count;

    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawMemCardPortItem);
    Draw_SetTextDimmed(1);
    count = node->visible_rows;
    while (count != 0) {
        Draw_AllocSprite(0x68);
        Draw_OffsetCursor(0, 0x10);
        count--;
    }
}

int Menu_IsMemCardSlotCompatible(int arg0) {
    MemCardSaveSlot *ptr;
    int result;

    if (g_McDialogMode != 0) {
        ptr = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, arg0);
        result = ptr->state != MEMCARD_SLOT_UNAVAILABLE;
    } else {
        ptr = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, arg0);
        if (ptr->state != MEMCARD_SLOT_OCCUPIED) {
            result = 0;
        } else {
            result = ptr->titleStyleFlag;
            result ^= Save_GetTitleStyleFlag();
            result = (unsigned int)result < 1;
        }
    }

done:
    return result;
}
