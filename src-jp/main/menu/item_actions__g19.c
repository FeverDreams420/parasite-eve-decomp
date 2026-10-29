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
#include "pe1/menu_dialog.h"
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
/* Inlined into Menu_OpenItemList, like Menu_CreateItemActionSubmenu; no
 * out-of-line copy exists in retail. */
static inline void Menu_CreateScreenModeSubmenu(void) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x2c, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x2c, parent, parent);
    parent->draw = Menu_DrawEquipScreenHeader;
    parent->update = Menu_InventoryItemHandler;
    child->draw = Menu_DrawScreenModeList;
    child->selectionAvailable = Menu_GetStatBaseValue;
    if (g_MenuEquipMode != 0) {
        parent->visible_rows += 0x10;
        child->visible_rows = 2;
        child->y_limit = 2;
        child->x += 0x1c;
    }
    MenuWidget_SetCurrentNode(child);
}

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
/* Also inlined into Menu_OpenItemList, which follows in this unit. */
static inline void Menu_CreateItemActionSubmenu(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *child;

    node = MenuWidget_CreateSimpleNode(4, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(4, node, node);
    node->draw = Menu_DrawItemActionSubmenu;
    node->update = Menu_StepSkillSelect;
    child->draw = Menu_DrawActionOptionList;
    g_MenuActionTextBase = 5;
    MenuWidget_SetCurrentNode(child);

    if (g_MenuSelectedItemIndex & 1) {
        node->visible_rows -= 0x1C;
        child->y -= 0x1C;
    }
}
void Menu_CreateItemList(void);



void Menu_OpenItemList(void)
{
    int list = 0;
    int slot = -1;
    int status;
    int kind;
    int i;
    MenuWidgetNode *node;
    ItemDataRecord *item;

    node = MenuWidget_GetCurrentNode();
    switch (node->selected_base) {
    case 6:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
    case 27:
        list = 0;
        break;
    case 11:
        slot = MenuWidget_GridCellIndex(MenuWidget_GetCurrentNode());
        list = 1;
        break;
    case 28:
        list = 1;
        break;
    }

    item = Inv_LookupActiveListData(Inv_RestoreSelection(list));
    /* slot == -1 reads tailCount, the byte just before tailData. */
    if (item->tailData[slot]) {

    g_MenuSelectedItemList = list;
    g_MenuSelectedItemSlot = slot;
    status = Inv_CheckItemEquippable(list, slot);
    if (status == 1 || status == 2) {
        kind = Inv_GetActiveListItemType(Inv_RestoreSelection(list));
        g_InvItemUsableFlag = kind != 9;
        kind = (g_StatBaseTable[0] > 0) + (g_StatBaseTable[1] > 0) + (g_StatBaseTable[2] > 0);
        if (g_StatBaseTable[3] > 0 ? kind + 1 >= 2 : kind >= 2) {
            Menu_CreateScreenModeSubmenu();
        } else {
            for (i = 0; i < 4 && g_StatBaseTable[i] == 0; ++i) {}
            g_MenuSelectedItemIndex = i < 4 ? i : 0;
            if (status == 1 || (!(g_MenuSelectedItemIndex & 1) && status == 2)) {
                Menu_CreateItemActionSubmenu();
            } else {
                Menu_CreateItemList();
            }
        }
        Menu_PlayConfirmSound();
        return;
    }
    switch (status) {
    case 0: Menu_CreateItemList(); return;
    case 4: kind = 0x63; break;
    default: kind = 7; break;
    }
    Menu_CreateNotificationDialog(kind, 0);
    return;
    }
    Menu_PlayErrorSound();
}