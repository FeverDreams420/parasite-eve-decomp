/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/save_tail_state.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuItemPrimListHandles[2];
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuActionTextBase;
int g_MenuActiveItemList;
MenuConfirmCallback g_MenuConfirmCallback;
int g_MenuEquipMode;
int g_MenuEquipSwapSource;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSelectedSpell;

int g_MenuInputPausePending;

#define NULL ((void *)0)
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
int Menu_StepEquipConfirm(MenuWidgetNode *node, int input);
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);
#undef NULL


void Menu_StepItemDetailPanel2(void);

int g_MenuSelectedSpell;

#define NULL ((void *)0)

void Menu_DrawEquipSortToggleList(int node);
int Menu_EquipGridHandler(void *node, int flags);
void Menu_CreateEquipInfoPanel(int parent, unsigned int option);
int Menu_EquipSelectInput(void *node, unsigned int flags);
void Menu_DrawEquipInfoOptionList(int node);
void Menu_DrawWeaponModSlots(s32 index);

int g_MenuInventoryViewMode;
int g_InvItemUsableFlag;



void Menu_StepSkillList(MenuWidgetNode *arg0, s32 arg1) {
    M2C_UNK var_a0_2;
    MenuWidgetNode *saved_arg0 = arg0;
    s32 saved_arg1 = arg1;
    s32 temp_v0;
    s32 mask;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    if (g_MenuLayoutLocked != 0) {
        temp_v0 = Inv_RestoreSelection(0);
        if ((Inv_GetActiveListItemType(temp_v0) != 0) && (Inv_GetActiveListItemType(temp_v0) < 6)) {
            mask = 0x3E;
        } else {
            mask = 1 << Inv_GetActiveListItemType(temp_v0);
        }
        Inv_BuildFilteredPackedListExcluding(mask, temp_v0);
        parent = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
        MenuWidget_OffsetPosition(parent, 0xB4 - parent->x, 0xA4 - parent->y);
    } else {
        var_a0_2 = 0x200;
        if (g_InvItemUsableFlag != 0) {
            var_a0_2 = 0x1FE;
        }
        Inv_BuildFilteredPackedList(var_a0_2);
    }
    if ((g_MenuEquipMode != 0) || (Inv_GetPackedListCount() != 0)) {
        parent = MenuWidget_CreateSimpleNode(7, saved_arg0, 0, 0);
        child = MenuWidget_CreateNode(7, parent, parent);
        parent->update = Menu_StepEquipConfirm;
        {
            s32 temp_v1 = g_MenuLayoutLocked;
            child->draw = &Menu_DrawEquipSelectionList;
            if (temp_v1 != 0) {
                MenuWidget_ClearColumnLayout(child);
            }
        }
        if (g_InvItemUsableFlag == 0) {
            MenuWidget_SetColumnLayout(child, 0x13);
        }
        if (saved_arg1 == 0) {
            child->cursor_x = -1;
        } else {
            child->cursor_x = 0;
        }
        if (child->cursor_x >= 0) {
            MenuWidget_SetCurrentNode(child);
        }
        Draw_SetPrimCallback(child, Inv_GetPackedListCount());
        if (g_MenuEquipMode != 0) {
            child->cursor_x = -1;
            MenuWidget_OffsetPosition(parent, 0, 0x14);
        }
        Menu_PlayConfirmSound();
        return;
    }
    Menu_PlayErrorSound();
}