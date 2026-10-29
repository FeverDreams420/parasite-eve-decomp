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

void Menu_DeferredExec(void) {
    if (g_MenuInputPausePending != 0) {
        if ((g_DiscChangeFlags & 3) == 0) {
            MenuInput_SetPollingPaused(0);
            g_MenuInputPausePending = 0;
        }
    }
}

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