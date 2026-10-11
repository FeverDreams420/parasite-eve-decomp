/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "pe1/inventory.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#include "pe1/menu_dialog.h"

int g_MenuItemActionContext;
int g_MenuActiveItemSlot;
int g_MenuItemActionDisabled;
int g_MenuEquipMode;

void Sfx_ContextPick(int arg0)
{
    int state;
    int flag;
    int raw;
    ItemDataRecord *entry;

    state = g_MenuItemActionStateTable[g_MenuItemActionContext][arg0];

    switch (state) {
    case 0:
        flag = 0;
        raw = g_MenuActiveItemSlot;
        entry = Inv_LookupActiveListData(raw);
        if (Inv_TestSelectionBit(raw) != 0) {
            if (g_MenuEquipMode != 1 || entry->kind != 0xA || (u8)entry->bonusStats[0] < 4) {
                flag = 1;
            }
        }
        Draw_SetTextDimmed(flag ^ 1);
        break;
    case 1:
        break;
    case 2:
        Draw_SetTextDimmed(Inv_IsSlotSelectable(g_MenuActiveItemSlot) == 0);
        break;
    case 3:
        Draw_SetTextDimmed(g_MenuItemActionDisabled);
        break;
    }
    MenuWidget_DrawCenteredText(Str_LookupTable4(state));
}
