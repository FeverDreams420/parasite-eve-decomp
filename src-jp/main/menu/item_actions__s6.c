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

void Menu_AlignEquipPanels(void) {
    MenuWidgetNode *panel;
    MenuWidgetNode *child;
    int shift;

    panel = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
    child = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 0xD), 0);
    shift = (g_MenuItemUseMode != 0 ? 0xB0 : child->popup_node != 0 ? 0xA2 : 0x9C) - panel->x;
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
    panel = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
    panel = MenuWidget_FindByModeAndSelectedBase(1, 0x2F);
    MenuWidget_OffsetPosition(panel, shift, 0);
    MenuWidget_ClearCursorY(panel);
}

void Menu_SetEquipPanelsCursorY(void) {
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}

int g_MenuItemUseMode;
int g_BonusPointDisplayValue;

void Menu_DrawBonusPointSlotValue(void) {
    int value;

    Draw_OffsetCursor(6, 4);
    Draw_AllocSprite(0x93);
    value = g_MenuItemUseMode;
    Draw_OffsetCursor((value * 5 * 4) + 0x48, 0);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
}

void Menu_ExitBonusPointAllocation(void) {
    s32 index;
    u16 *dst;
    s32 *src;
    u16 value;

    dst = g_AyaStatAgility;
    index = 0;
    src = g_BonusPointStatDeltas;
    do {
        value = *src;
        src++;
        index += 1;
        *dst = value;
        dst++;
    } while (index < 7);
    g_AyaBonusPoints[0] = g_BonusPointDisplayValue;
    Inv_RecalcSlotStats();
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (g_MenuEquipMode != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != 0) {
            MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
        }
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    g_MenuItemRenameMode = 0;
    g_MenuItemUseMode = 0;
    if (g_MenuEquipMode == 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0));
        Menu_CreateBonusPointAllocationView();
        g_MenuBattleStatusOverlayActive = 1;
    }
}

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