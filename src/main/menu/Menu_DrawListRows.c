/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/text.h"

/* Row drawers for the equip-slot, inventory, armor, battle-command and
 * skill lists. */

MenuWidgetNode *g_MenuActiveWidget;
int g_MenuEquipMode;
extern unsigned char D_800923B8[][8];

void Menu_DrawEquipSlotListItem(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }

    Draw_OffsetCursor(-2, -2);

    if (g_MenuEquipMode != 0) {
        Draw_AllocSprite(D_800923B8[g_MenuEquipMode][arg0]);
    }
}

void Menu_DrawInventoryItemRow(int index) {
    int slot = Inv_GetWayneListItemByIndex(index);

    Draw_SetTextDimmed(Inv_TestSelectionBit(slot) == 0);
    Sfx_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}

void Menu_DrawArmorItemRow(int index) {
    Menu_DrawArmorItemDetail(index);
}

void Menu_DrawBattleCommandItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x1F);
}

void Menu_DrawSkillItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x5D);
}
