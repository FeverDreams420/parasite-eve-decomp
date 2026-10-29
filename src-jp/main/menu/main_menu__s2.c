/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao_counters.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuPendingTotalExp;
int g_MenuSelectionLocked;
int g_AyaFullInvHold;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuBlendColorBackup;
int g_MenuEquipSwapSource;

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
void Menu_OpenCursorModeMenu(MenuWidgetNode *parent);
int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1);
int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1);

void Menu_DrawBlendColorArrows(MenuWidgetNode *panel);
s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags);
void Menu_DrawBlendColorChannelList(int node);
void Menu_DrawBlendColorOptionList(int node);
void Menu_DrawScreenAdjustPanel(void);
int Menu_ScreenAdjustInputHandler(void *node, int input);

int g_MenuScreenBaseYBackup;

void Menu_CreateExpReviewView(void);
void Menu_DrawExpReviewPanel(void);
s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags);
void Menu_CreateLevelUpBanner(void);
void Menu_DrawLevelUpBanner(void);
void Menu_CreateLevelUpResultPanel(void);
void Menu_DrawBonusPointAnimFrame(void);
s32 Menu_StepParasiteScreen(MenuWidgetNode *node, s32 flags);

int g_BonusPointBarAnimProgress, g_BonusPointBarAnimStep;
int g_BonusPointDisplayValue, g_MenuBonusPointDisplayTarget;
int g_MenuBonusPointBarAnimActive;
int g_MenuExpAllocTarget;
int g_MenuExpReviewLevel;
int g_MenuHpMaxAnimTimer;
int g_MenuHpMaxDisplayTarget, g_MenuHpMaxDisplayValue;
int g_MenuLevelDisplayTarget, g_MenuLevelDisplayValue;
int g_MenuLevelUpAnimTimer;

#define NULL ((void *)0)

s32 Menu_OpenEquipSlotSwap(s32 arg0) {
    s32 temp_s2;
    s32 temp_v1;
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;

    temp_s2 = MenuWidget_FindByModeAndSelectedBase(1, 1) != NULL;
    if (temp_s2 == 0) {
        MenuInput_InitQueue();
        MenuWidget_CreateSimpleNode(0x1B, 0, 0, 0)->draw = Menu_DrawAmmoTypeHeader;
        temp_v0 = MenuWidget_CreateSimpleNode(1, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(1, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        temp_v0->update = Menu_InventoryInputHandler;
        temp_s1->draw = Menu_DrawSelectableEquipSlotList;
        MenuWidget_SetCurrentNode(temp_a0);
        temp_s1->itemAction = (MenuWidgetItemAction)Inv_SwapSlots;
        temp_s1->refreshItems = Menu_RebuildSelectableMask;
        Inv_RebuildSelectableMask();
        g_InvSwapTargetIndex = -1;
        g_InvSelectedItemIndex = -1;
        Draw_SetPrimCallback(temp_s1, Inv_GetAyaSlotLimit());
        g_MenuActionSubmenuOpen = 0;
        if (g_MenuEquipSwapSource != 0) {
            temp_v1 = g_MenuEquipSwapSource - 1;
            temp_s1->cursor_x = (temp_v1 & 1);
            temp_s1->cursor_y = ((temp_v1 >> 1) & 0x7F);
            temp_s1->scroll_y = (temp_v1 >> 8);
        }
        Menu_CreateContextHelpPanel();
        MenuWidget_FindByModeAndSelectedBase(2, 1)->itemAction = NULL;
        Inv_SortSlotsByPriority();
        g_MenuActionSubmenuOpen = 1;
        g_AyaFullInvHold = arg0;
        g_MenuSelectionLocked = 0;
        Menu_SetBattleEquipMode(0);
    }
    return temp_s2;
}