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

void Menu_CreateItemDetailView(MenuWidgetNode *arg0) {
    MenuWidgetNode *node;
    MenuWidgetNode *parent;
    MenuWidgetNode *link;

    parent = MenuWidget_CreateSimpleNode(0xB, arg0, 0, 0);
    node = MenuWidget_CreateNode(0xB, parent, parent);
    parent->update = Menu_DetailViewInput;
    node->draw = Menu_DrawUsableItemActionList2;

    Inv_SelectActiveList(0);
    Draw_SetPrimCallback(node, ((char *)Inv_LookupActiveListData(Inv_RestoreSelection(1)))[0x14]);
    MenuWidget_ClearColumnLayout(node);

    link = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (link != 0) {
        node->linkedPrevious = link;
        link->linkedNext = node;
    }

    Inv_InitWayneStorage();
}

int Menu_DetailViewInput(MenuWidgetNode *arg0, unsigned int arg1) {
    MenuWidgetNode *node;

    node = MenuWidget_GetChild(arg0, 0);
    Menu_InventoryNavigate(node, MenuWidget_FindByModeAndSelectedBase(2, 6), arg1);

    if (arg1 & 0x1000) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x1C);
        node->cursor_x = 0;
        node->cursor_y = node->y_limit - 1;
        MenuWidget_SetCurrentNode(node);
        Menu_PlayMoveSound();
    }

    return 1;
}
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