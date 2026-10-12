/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_item_rows.h"

void Menu_DrawSelectableEquipSlotRow(int slot) {
    int dimmed = 0;

    if (g_MenuSelectionLocked != 0 || Inv_TestSelectionBit(slot) == 0) {
        dimmed = 1;
    }

    Draw_SetTextDimmed(dimmed);
    Sfx_DrawActiveListSlot(slot);
    if (Inv_IsAyaInventorySlotReserved(slot) != 0) {
        Draw_DropShadow();
    }
}
