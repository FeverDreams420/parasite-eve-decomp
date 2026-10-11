#include "pe1/inventory.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "Menu_HandleInput_externs.h"

int Inv_GetPackedListCount(void);
int Inv_TestSelectionBit(int arg0);
void Draw_SetTextDimmed(int arg0);
int Inv_GetPackedListItem(int arg0);
int Inv_IsAyaInventorySlotReserved(int arg0);
int Inv_GetActiveListItemType(int arg0);
int Inv_SetupSlotDisplay(int arg0);
void Sfx_DrawActiveListSlot(int arg0);
void Draw_DropShadow(void);

void Menu_HandleInput(int arg0) {
    register int value;
    int flag;
    int blocked;

    if (arg0 >= Inv_GetPackedListCount()) {
        return;
    }

    if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
        if (Inv_TestSelectionBit(arg0) == 0) {
            Draw_SetTextDimmed(1);
        }
    }

    value = Inv_GetPackedListItem(arg0);
    blocked = Inv_IsAyaInventorySlotReserved(value);

    if (blocked != 0) {
        if (g_MenuActiveWidget->selected_base != 0xD) {
            if (g_MenuLayoutLocked == 0 && g_MenuItemUseMode == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    }

    if (g_MenuLayoutLocked != 0) {
    flag = 0;
    if (g_StatBaseTable[0] + g_ItemCategoryCount1[0] + g_ItemCategoryCount2[0] + g_ItemCategoryCount3[0] != 0) {
        if ((((u8 *)Inv_LookupActiveListData(value))[5] & 0x40) == 0) {
            int mask;

            if (Inv_GetActiveListItemType(value) >= 6) {
                mask = 1 << Inv_GetActiveListItemType(value);
            } else {
                mask = 0x3E;
            }

            if (Inv_SetupSlotDisplay(mask) != 0) {
                flag = 1;
            }
        }
    }

    if (flag == 0) {
        Draw_SetTextDimmed(1);
    }
    }

    Sfx_DrawActiveListSlot(value);
    if (blocked != 0) {
        Draw_DropShadow();
    }
}
