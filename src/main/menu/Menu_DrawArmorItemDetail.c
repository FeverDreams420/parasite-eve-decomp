/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/draw_state.h"
#include "pe1/menu_item_rows.h"
#include "pe1/save.h"
#include "pe1/text.h"

void Menu_DrawArmorItemDetail(int slot) {
    void *display;
    ItemDataRecord *entry;
    ItemDataRecord *candidate;
    int value;
    unsigned int bit;

    value = D_8009D07C[slot];
    if (value != 0) {
        bit = g_InvSelectionBits[slot >> 5] & (1 << (slot & 0x1F));
        Draw_SetTextDimmed(bit == 0);

        if ((unsigned int)(value - 0x100) < 0x80U) {
            candidate = (ItemDataRecord *)(g_EquipItemDataTable +
                                           (value << 5));
        } else if ((unsigned int)(value - 1) < 0xFFU) {
            candidate = Item_LookupBaseData(value - 1);
        } else if ((unsigned int)(value - 0x200) < 9U) {
            candidate = (ItemDataRecord *)(g_KeyItemDataTable + (value << 5));
        } else {
            candidate = 0;
        }

        entry = candidate;

        if ((entry->flags & ITEM_DATA_FLAG_GENERIC_DESCRIPTION) != 0) {
            display = D_800C20A4;
            if (entry->kind == ITEM_KIND_ARMOR) {
                display = D_800C20A4 + 0x10;
            }
        } else {
            display = Str_LookupTable8(entry->itemId - 1);
        }

        Sfx_DrawSlotRow(entry, display);
    }
}
