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

/* Equip item selection view, its stat delta preview, grid scroll and slot
 * step, and the skill screen input handler it installs. */

#define NULL ((void *)0)




s32 Menu_StepSkillScreen(MenuWidgetNode *panel, s32 flags) {
    MenuWidgetNode *node;
    MenuWidgetNode *root;

    node = MenuWidget_GetChild(panel, 0);
    if (flags & 0x10000) {
        if (Inv_GetPackedListCount() != 0) {
            if (g_MenuLayoutLocked != 0) {
                Menu_OpenItemList();
                return 1;
            }
            if (Battle_IsInputAllowedWrapped() != 0) {
                root = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                MenuWidget_CreateNode(0x35, root, root)->draw = Menu_SetupSkillSubmenu;
                node = MenuWidget_FindByModeAndSelectedBase(2, 7);
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                Menu_PlayConfirmSound();
                return 1;
            }
        }
        Menu_PlayErrorSound();
        return 1;
    }
    if (flags & 0x40) {
        if (g_MenuLayoutLocked != 0) {
            Menu_InitCategoryTabs();
            return 1;
        }
        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(panel);
        panel = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (panel != NULL) {
            MenuWidget_DestroyNode(panel);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1C));
            Menu_CreateBonusPointAllocationView();
            Menu_PlayCancelSound();
        }
        return 1;
    }
    if ((flags & 0x4000) && MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 6)) != 0) {
        node->cursor_x = -1;
        node = MenuWidget_GetChild(panel, 1);
        if (node != NULL) {
            node->cursor_x = -1;
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node != NULL) {
            node->cursor_y = 0;
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
        }
        Menu_PlayMoveSound();
    }
    return 1;
}