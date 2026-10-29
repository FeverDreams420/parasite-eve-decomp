/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_nop.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointSpendCurrentValue;
int g_MenuItemPrimListHandles[2];
int g_MenuSourceItemSlot;
int g_MenuStatItemStatIndex;
int g_MenuSourceItemList;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_BonusPointSpendStat;
int g_BonusPointSpendWorkingValue;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
int g_MenuActionSubmenuOpen;
int g_MenuActiveItemList;
int g_MenuBattleStatusOverlayActive;
int g_MenuBonusPointBarAnimActive;
MenuConfirmCallback g_MenuConfirmCallback;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_MenuEquipMode;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSpendArrowDirection;

void Menu_CreateItemDetailView(MenuWidgetNode *arg0);
void Menu_CreateEquipStatsPanel(int arg0);

/* Category tab reset and equip screen header, item action submenu,
 * skill/stat point use, the equip stats panel and the item detail and item
 * list views of the inventory menu. Contiguous at 0x80047678 and joined by
 * private data and helper calls. */

int Menu_EquipStatsInputHandler(MenuWidgetNode *node, int flags);
int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1);
int g_MenuSelectedItemIndex;
int g_MenuArmorCapacityModBlocked;
void Menu_CreateItemActionSubmenu(void);
void Menu_CreateItemList(void);
int g_MenuActionTextBase;
int g_InvItemUsableFlag;
int g_MenuSelectedItemList;
int g_MenuSelectedItemSlot;
void Menu_DrawActiveListSlot(int arg0);
#define NULL ((void *)0)
void Menu_DrawEquipStats(void);

void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_DrawItemListHeader(void);
int g_MenuItemListHeaderGlyph;

int g_MenuEquipMode;

void Draw_FlushPrimList(int arg0);
void Draw_SetColor(int arg0);
#undef NULL


/* The inventory root step, equip panel alignment, the bonus-point slot
 * handlers and the bonus-point allocation flow with its item list input
 * (main_tu_evidence G0426). */

/* Defined here so the assembler sees a small-data symbol: retail waits a
 * load-delay slot before the gp-relative store of the reserve ammo. */
int g_MenuStatItemAmount;

int g_MenuItemUseMode;
int g_BonusPointDisplayValue;

int g_MenuItemUseMode;

#define NULL ((void *)0)

#undef g_BonusPointStatDeltas
#define CLAMP_EQUIP_PREVIEW(out, item, stat_index, active_stat) { \
    int desired_ = (active_stat); \
    int current_ = g_MenuStatItemStatIndex; \
    int raw_ = (item)->bonusStats[(stat_index)]; \
    if ((current_ == desired_ && raw_ + g_MenuStatItemAmount < 1000) || \
        (current_ != desired_ && raw_ < 1000)) { \
        (out) = (item)->bonusStats[(stat_index)]; \
        if (g_MenuStatItemStatIndex == (active_stat)) { \
            (out) += g_MenuStatItemAmount; \
        } \
    } else { \
        (out) = 999; \
    } \
}


void Menu_DrawBonusPointSpendPanel(void);
s32 Menu_SpendBonusPoints(void *node, u32 buttons);
void Menu_StepInventoryRoot(int mode, int list, int item);



int Menu_StepSkillSelect(MenuWidgetNode *arg0, int arg1) {
    M2C_UNK var_a0;
    s32 *temp_a1;
    s32 pad_[1];
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    s32 var_v0;

    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GridCellIndex(MenuWidget_GetChild(arg0, 0));
        if (temp_v0 != 0) {
            if (temp_v0 == 1) {
                MenuWidget_DestroyNode(arg0);
                Menu_PlayCancelSound();
                return 1;
            }
            return 1;
        }
        {
            MenuWidget_DestroyNode(arg0);
            Inv_TransferItemBetweenLists(g_MenuSelectedItemList, g_MenuSelectedItemSlot, g_MenuSelectedItemIndex, g_StatBaseTable[g_MenuSelectedItemIndex]);
            temp_a1 = &g_StatBaseTable[g_MenuSelectedItemIndex];
            temp_v0_2 = *temp_a1;
            *temp_a1 = temp_v0_2 - (temp_v0_2 < 0x3E7);
            MenuWidget_NavScrollTo(0x2C);
            MenuWidget_NavScrollTo(0xB);
            MenuWidget_NavScrollTo(0xA);
            MenuWidget_NavScrollTo(5);
            MenuWidget_NavScrollTo(6);
            MenuWidget_NavScrollTo(7);
            MenuWidget_NavScrollTo(0x1C);
            var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (var_v0 == 0) {
                var_v0 = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            }
            MenuWidget_SetCurrentNode(var_v0);
            var_s1 = 0;
            Menu_StepInventoryRoot(0x33E, -1, Inv_RestoreSelection(g_MenuSelectedItemList == 0));
            do {
                temp_s0 = Inv_RestoreSelection(var_s1);
                if ((Inv_IsActiveListOverrideSelected() == 0) && ((var_a0 = 2, (temp_s0 == g_InventoryRuntime.tracked[0])) || (var_a0 = 3, (temp_s0 == g_InventoryRuntime.tracked[2])))) {
                    Inv_SetActiveList(var_a0, 0);
                }
                var_s1 += 1;
            } while (var_s1 < 2);
            Menu_PlayConfirmSound();
            return 1;
        }
    } else {
        if (arg1 & 0x40) {
shared_1:
            MenuWidget_DestroyNode(arg0);
            Menu_PlayCancelSound();
        }
        return 1;
    }
}