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


/* Leaves the item list: scrolls the inventory panels back and returns the
 * focus to the equipment slots or the category tabs. */
static inline void Menu_CloseItemList(void)
{
    MenuWidget_NavScrollTo(0xF);
    MenuWidget_NavScrollTo(0xB);
    MenuWidget_NavScrollTo(0xD);
    MenuWidget_NavScrollTo(0x18);
    MenuWidget_NavScrollTo(0x30);
    if (g_MenuEquipMode != 0) {
        MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
    } else {
        if (MenuWidget_GetCurrentNode() != 0)
            MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    }
    g_MenuItemRenameMode = 0;
}

static inline void Menu_ResetSlotCursors(void)
{
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xF));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0xB));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x2F));
}

/* Item action list opened next to the inventory panel. */
static inline void Menu_CreateUsableItemActionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(6, parent, 0, 0);
    node = MenuWidget_CreateNode(6, root, root);
    root->update = Menu_InventoryPageInputHandler;
    root->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    node->layout_flags |= 0x80;
    if (g_MenuLayoutLocked != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (g_InvItemUsableFlag == 0)
        MenuWidget_SetColumnLayout(node, 0x14);
    if (node->cursor_x >= 0)
        MenuWidget_SetCurrentNode(node);
}

/* Whether the selected item can go into an equipment slot that has room. */
static inline int Menu_CanEquipSelection(int selection)
{
    int usable;

    usable = 0;
    if (g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3] != 0
        && !(Inv_LookupActiveListData(selection)->flags & 0x40)) {
        if (Inv_SetupSlotDisplay(Inv_GetActiveListItemType(selection) >= 6
                                     ? 1 << Inv_GetActiveListItemType(selection)
                                     : 0x3E) != 0)
            usable = 1;
    }
    return usable;
}inline void Menu_CreateEquipItemSelectionView(MenuWidgetNode *parent);


/* Input handler of the inventory item list: up returns to the equipment
 * slots, confirm applies a pending stat bonus, opens the equip, examine,
 * rename or use flows, and cancel backs out of the active mode. */
int Menu_ItemListInputHandler(MenuWidgetNode *node, unsigned int flags)
{
    ItemDataRecord *data;
    MenuWidgetNode *child;
    MenuWidgetNode *found;
    MenuWidgetNode *target;
    MenuWidgetNode *panel;
    MenuWidgetNode *list;
    int index;
    int item;
    int selection;
    int usable;
    int mask;
    int kind;
    int value;

    child = MenuWidget_GetChild(node, 0);
    if (flags & 0x1000) {

        found = MenuWidget_FindByModeAndSelectedBase(2, 0x30);
        if (found != 0) {
            child->cursor_x = -1;
            found->cursor_x = 0;
            found->cursor_y = 1;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
            Menu_PlayMoveSound();
        }
        found = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
        if (found != 0) {
            child->cursor_x = -1;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
            Menu_PlayMoveSound();
            return 1;
        }
    } else if (flags & 0x10000) {
        index = MenuWidget_GridCellIndex(MenuWidget_FindByModeAndSelectedBase(2, 0xD));
        item = Inv_GetPackedListItem(index);
        data = Inv_LookupActiveListData(item);
        if (g_MenuSourceItemSlot >= 0) {
            switch (g_MenuStatItemStatIndex) {
            case 0:
                data->bonusStats[0] = data->bonusStats[0] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[0] + g_MenuStatItemAmount;
                break;
            case 1:
                data->bonusStats[1] = data->bonusStats[1] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[1] + g_MenuStatItemAmount;
                break;
            case 2:
                data->bonusStats[2] = data->bonusStats[2] + g_MenuStatItemAmount >= 1000
                                          ? 999 : data->bonusStats[2] + g_MenuStatItemAmount;
                break;
            }
            Inv_SelectActiveList(g_MenuSourceItemList);
            Inv_RemoveActiveListItem(g_MenuSourceItemSlot);
            Menu_CloseItemList();
            if (g_MenuEquipMode == 1) {
                MenuWidget_NavScrollTo(0x36);
                Menu_ReopenEquipScreen();
            }
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuLayoutLocked != 0) {
            if (Menu_CanEquipSelection(Inv_RestoreSelection(0)) != 0) {
                Menu_CloseItemList();
                MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x36), 0x98, 0);
                g_InvItemUsableFlag = data->kind != 9;
                panel = MenuWidget_FindByModeAndSelectedBase(2, 0);
                if (panel == 0)
                    panel = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
                Menu_CreateUsableItemActionView(panel);
                Menu_CreateEquipItemSelectionView(panel);
                Menu_StepSkillList(panel, 1);
                if (g_MenuEquipMode == 0) {
                    MenuWidgetNode *submenu;

                    submenu = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                    MenuWidget_CreateNode(0x35, submenu, submenu)->draw = Menu_SetupSkillSubmenu;
                }
                /* The list child is no longer needed: reuse it for the equipment node. */
                child = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
                if (child != 0)
                    MenuWidget_SetCurrentNode(child);
                return 1;
            }
        } else if (g_MenuItemUseMode != 0) {
            MenuWidget_NavScrollTo(0xD);
            MenuWidget_NavScrollTo(0x30);
            MenuWidget_NavScrollTo(0x36);
            target = MenuWidget_FindByModeAndSelectedBase(1, 0xF);
            target->parent = MenuWidget_FindByModeAndSelectedBase(2, 0);
            if (target->parent == 0)
                target->parent = MenuWidget_FindByModeAndSelectedBase(2, 0x32);
            MenuWidget_OffsetPosition(target, 0x1C - target->x, 0x14);
            target = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
            MenuWidget_OffsetPosition(target, 0x1C - target->x, 0x14);
            target = MenuWidget_FindByModeAndSelectedBase(2, 0x1D);
            target->cursor_x = 0;
            target->cursor_y = 0;
            kind = Inv_GetActiveListItemType(Inv_RestoreSelection(0));
            target->y_limit = kind == 8 ? 1 : kind == 6 ? 2 : 3;
            MenuWidget_SetCurrentNode(target);
            g_MenuItemUseSelectedItemSnapshot = *Inv_LookupActiveListData(Inv_RestoreSelection(0));
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuItemRenameMode != 0 && Inv_TestSelectionBit(index) != 0) {
            Menu_CloseItemList();
            Menu_OpenRenameScreen(item);
            Menu_PlayConfirmSound();
            return 1;
        } else if (g_MenuItemDiscardMode != 0) {
            if (Inv_TestSelectionBit(index) != 0) {
                MenuWidgetNode *dialog;
                MenuWidgetNode *options;
                MenuConfirmCallback action;
                int width;

                dialog = MenuWidget_CreateSimpleNode(0x29, child, 0, 1);
                options = MenuWidget_CreateNode(0x29, dialog, dialog);
                dialog->draw = Menu_DrawItemLabel;
                dialog->update = Menu_ConfirmDialogHandler;
                options->draw = Menu_DrawActionOptionList;
                g_MenuActionTextBase = 5;
                MenuWidget_SetCurrentNode(options);
                Inv_SelectActiveList(g_MenuActiveItemList);
                g_MenuItemLabelBuffer[0] = 0xFF;
                Text_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Text_LookupTable4(0x61));
                g_MenuItemPrimListHandles[0] = 0;
                action = Menu_ItemUseAction;
                if (Draw_MeasureTextWidth(g_MenuItemLabelBuffer) < 0x78)
                    width = 0x78;
                else
                    width = Draw_MeasureTextWidth(g_MenuItemLabelBuffer);
                dialog->grid_width = width + 0x14;
                dialog->visible_rows = 0x32;
                dialog->x = (300 - width) >> 1;
                options->x = (dialog->grid_width - 0x80) >> 1;
                g_MenuConfirmCallback = action;
                options->y = dialog->visible_rows - 0x14;
                Menu_PlayConfirmSound();
                return 1;
            }
            Menu_CreateNotificationDialog(0x62, 0);
        }
        Menu_PlayErrorSound();
        return 1;
    } else if (flags & 0x40) {
        found = MenuWidget_FindByModeAndSelectedBase(2, 0x36);
        if (found != 0) {
            child->cursor_x = -1;
            child->scroll_y = 0;
            MenuWidget_SetCurrentNode(found);
            Menu_ResetSlotCursors();
        } else if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
            g_MenuItemRenameMode = 0;
            g_MenuItemDiscardMode = 0;
            Inv_SetActiveList(9, 0);
        } else {
            MenuWidget_NavScrollTo(0x36);
            if (g_MenuLayoutLocked != 0) {
                g_MenuLayoutLocked = 0;
                Menu_CloseItemList();
                MenuWidget_NavScrollTo(0x2F);
                if (g_MenuEquipMode == 0) {
                    Menu_CreateBonusPointAllocationView();
                    g_MenuBattleStatusOverlayActive = 1;
                }
            } else if (g_MenuItemUseMode != 0) {
                Menu_ExitBonusPointAllocation();
            } else {
                Menu_CloseItemList();
            }
        }
        Menu_PlayCancelSound();
    }
    return 1;
}
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

void Menu_StepEquipSlotSelect2(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *cursor_node;
    ItemDataRecord *item;
    int slot;
    int value;

    node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
    if (node != 0) {
        if (node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (g_MenuItemUseMode != 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        if (node != 0 && node->cursor_x >= 0) {
            slot = Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
            Inv_RememberSelection(0, slot);
        }
    } else if (g_MenuLayoutLocked == 0) {
        if (g_InvItemUsableFlag != 0) {
            Inv_RememberSelection(0, g_InventoryRuntime.tracked[0]);
        } else {
            Inv_RememberSelection(0, g_InventoryRuntime.tracked[2]);
        }
    }

    slot = Inv_RestoreSelection(0);
    Draw_OffsetCursor(4, 4);
    Menu_DrawActiveListSlot(slot);
    Draw_OffsetCursor(-4, -4);

    item = Inv_LookupActiveListData(slot);
    g_InvItemUsableFlag = (item->kind != 9);
    Menu_DrawEquipStatsDelta(item);

    if ((g_MenuLayoutLocked != 0 || g_MenuItemUseMode != 0 || g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) &&
        item != 0) {
        Draw_OffsetCursor(0x2A, -0xC);
        Draw_PrintNumberWidth4Unk(item->baseStats[2]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[1]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x2D, -0xE);

        Draw_PrintNumberWidth4Unk(item->baseStats[0]);
        Draw_OffsetCursor(5, 0);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    }

    if (g_MenuSourceItemSlot >= 0) {
        Draw_OffsetCursor(0x28, -0x32);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);
        Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
        Draw_OffsetCursor(-0x14, 0xE);
        Draw_PrintSignedNumberWidth4(item->bonusStats[2]);

        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -0xE);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0xA, -0xA);
        Draw_AllocSprite(0x88);
        Draw_OffsetCursor(0, 0xA);

        CLAMP_EQUIP_PREVIEW(value, item, 0, 0);
        Draw_SetStatCompareColor(item->bonusStats[0], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 1, 1);
        Draw_SetStatCompareColor(item->bonusStats[1], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_OffsetCursor(-0x14, 0xE);

        CLAMP_EQUIP_PREVIEW(value, item, 2, 2);
        Draw_SetStatCompareColor(item->bonusStats[2], value);
        Draw_PrintSignedNumberWidth4(value);
        Draw_SetColor(0x808080);
    }

    cursor_node = MenuWidget_FindByModeAndSelectedBase(1, 0xB);
    {
        void (*set_cursor_y)(MenuWidgetNode *node);
        set_cursor_y = item->tailCount != 0 ? MenuWidget_ClearCursorY : MenuWidget_SetCursorY;
        set_cursor_y(cursor_node);
    }
}

void Menu_DrawBonusPointSpendPanel(void);
s32 Menu_SpendBonusPoints(void *node, u32 buttons);

void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *node;
    InvItemSlot *record;

    node = MenuWidget_CreateSimpleNode(9, arg0, 0, 1);
    node->draw = Menu_DrawBonusPointSpendPanel;
    node->update = Menu_SpendBonusPoints;
    node->flags = 1;
    MenuWidget_SetCurrentNode(node);

    g_BonusPointSpendStat = arg1;
    g_BonusPointSpendWorkingValue = g_BonusPointDisplayValue;
    if (arg1 < 3) {
        record = Inv_LookupActiveListData(Inv_RestoreSelection(0));
        g_BonusPointSpendItemCopy = *record;
        node->grid_width -= 0x14;
        MenuWidget_OffsetPosition(node, 0xA, 0);
    } else {
        g_BonusPointSpendCurrentValue = g_BonusPointStatDeltas[arg1];
    }
    g_MenuSpendArrowDirection = 0;
}

void Menu_DrawBonusPointSpendPanel(void) {
    int value;
    int mode;
    int level;
    int distance;
    int sublevel;

    Draw_OffsetCursor(0x14, 0x15);
    Draw_AllocSprite(g_MenuSpendArrowDirection + 0x4D);
    Draw_OffsetCursor(-0xE, -0x11);
    Draw_AllocSprite(0x93);

    if (g_BonusPointSpendStat < 3) {
        Draw_OffsetCursor(0x48, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x75, 0x1E);
        Draw_AllocSprite(g_InvItemUsableFlag != 0 ? g_BonusPointSpendStat + 0x7C : g_BonusPointSpendStat + 0x7F);
        Draw_OffsetCursor(0x1E, 0);
        mode = g_BonusPointSpendStat;
        switch (mode) {
        case 0: {
            value = g_BonusPointSpendItemCopy.baseStats[0] + g_BonusPointSpendItemCopy.bonusStats[0];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[0]);
            break;
        }
        case 1: {
            value = g_BonusPointSpendItemCopy.baseStats[1] + g_BonusPointSpendItemCopy.bonusStats[1];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[1]);
            break;
        }
        case 2: {
            value = g_BonusPointSpendItemCopy.baseStats[2] + g_BonusPointSpendItemCopy.bonusStats[2];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(g_BonusPointSpendItemCopy.baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(g_BonusPointSpendItemCopy.bonusStats[2]);
            break;
        }
        }
        Draw_OffsetCursor(-0x2D, -0xA);
        Draw_AllocSprite(0x87);
        Draw_OffsetCursor(0x19, 0);
        Draw_AllocSprite(0x88);
    } else {
        Draw_OffsetCursor(0x5C, 0);
        Draw_PrintNumberWidth5(g_BonusPointSpendWorkingValue);
        Draw_OffsetCursor(-0x8A, 0x1F);
        Draw_AllocSprite(g_BonusPointSpendStat + 0x8C);
        Draw_OffsetCursor(0x42, -1);
        Aya_StatQueryLevelAndSubLevel(g_BonusPointSpendStat,
                                   g_BonusPointSpendCurrentValue, &level, 0);
        Draw_PrintNumberWidth2(level + 1);
        Aya_StatQueryDistanceAndSubLevel(g_BonusPointSpendStat,
                                      g_BonusPointSpendCurrentValue, &distance, &sublevel);
        Draw_OffsetCursor(2, 0);
        Draw_AllocTexturedRectAlt(distance, sublevel);
    }
}

int Menu_SpendBonusPoints(void *node, unsigned int buttons)
{
    ItemDataRecord *item;
    int amount;
    int changed;

    item = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    if (buttons & 0x4000) {
        g_MenuSpendArrowDirection = 0;
        if (g_BonusPointSpendWorkingValue >= 100) {
            changed = 0;
            switch (g_BonusPointSpendStat) {
            case 0: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[0];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 1: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[1];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 2: {
                short *bonus = &g_BonusPointSpendItemCopy.bonusStats[2];
                changed = *bonus < 999;
                *bonus += changed;
                break;
            }
            case 5:
            case 6:
                Aya_StatQueryDistanceAndSubLevel(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue, &amount, 0);
                changed = amount > 0;
                g_BonusPointSpendCurrentValue += amount;
                break;
            }
            g_BonusPointSpendWorkingValue -= changed * 100;
            if (changed) {
                Menu_PlayMoveSound();
                return 1;
            }
        }
        Menu_PlayErrorSound();
    } else if (buttons & 0x1000) {
        g_MenuSpendArrowDirection = 1;
        switch (g_BonusPointSpendStat) {
        case 0: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[0];
            if (*bonus > item->bonusStats[0]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 1: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[1];
            if (*bonus > item->bonusStats[1]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 2: {
            short *bonus = &g_BonusPointSpendItemCopy.bonusStats[2];
            if (*bonus > item->bonusStats[2]) {
                --*bonus;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        case 5:
        case 6: {
            int *minimum = &g_BonusPointStatDeltas[g_BonusPointSpendStat];
            int gap;
            int new_current;
            if (*minimum < g_BonusPointSpendCurrentValue) {
                gap = Aya_StatOverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
                if (g_BonusPointSpendCurrentValue - gap < *minimum) {
                    new_current = g_BonusPointStatDeltas[g_BonusPointSpendStat];
                } else {
                    new_current = g_BonusPointSpendCurrentValue - Aya_StatOverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
                }
                g_BonusPointSpendCurrentValue = new_current;
                g_BonusPointSpendWorkingValue += 100;
                Menu_PlayMoveSound();
            } else {
                Menu_PlayErrorSound();
            }
            break;
        }
        }
    } else if (buttons & 0x10000) {
        g_BonusPointDisplayValue = g_BonusPointSpendWorkingValue;
        if (g_BonusPointSpendStat < 3) {
            *item = g_BonusPointSpendItemCopy;
            if (!Inv_IsActiveListOverrideSelected()) {
                if (item == Inv_LookupActiveListData(g_InventoryRuntime.tracked[0])) {
                    Inv_SetActiveList(2, 0);
                } else if (item == Inv_LookupActiveListData(g_AyaEquippedArmorSlot[0])) {
                    Inv_SetActiveList(3, 0);
                }
            }
        } else {
            g_BonusPointStatDeltas[g_BonusPointSpendStat] = g_BonusPointSpendCurrentValue;
        }
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
    } else if (buttons & 0x40) {
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }
    return 1;
}