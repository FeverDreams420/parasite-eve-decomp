#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_sound.h"
#include "pe1/aya.h"
#include "pe1/menu_widget.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_inventory_root.h"
#include "common.h"
#include "pe1/menu_inventory.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#include "pe1/draw_level_bar.h"
#include "pe1/inventory_slots.h"
#include "pe1/battle_modifiers.h"

/* The inventory root step, equip panel alignment, the bonus-point slot
 * handlers and the bonus-point allocation flow with its item list input
 * (main_tu_evidence G0426). */

/* Defined here so the assembler sees a small-data symbol: retail waits a
 * load-delay slot before the gp-relative store of the reserve ammo. */
int D_8009CFCC;

/* Builds the inventory screen widget tree: the optional battle command list,
 * the item list for `list` (preselecting `item` when it is present), the bonus
 * point and parasite ability panels, and the item action list. */
void Menu_StepInventoryRoot(int mode, int list, int item)
{
    ItemDataRecord *data;
    MenuWidgetNode *panel;
    MenuWidgetNode *node;
    int i;
    int slot;

    data = Inv_LookupActiveListData(list);
    if (g_MenuEquipMode) {
        panel = MenuWidget_CreateSimpleNode(0x36, MenuWidget_GetCurrentNode(), 0, 0);
        node = MenuWidget_CreateNode(0x36, panel, panel);
        panel->update = Menu_InventoryPageNavHandler;
        if (g_MenuItemUseMode)
            MenuWidget_OffsetPosition(panel, 0, 0x14);
        node->draw = Menu_DrawBattleCommandList;
        MenuWidget_SetCurrentNode(node);
    }
    panel = MenuWidget_CreateSimpleNode(0xD, MenuWidget_GetCurrentNode(), 0, 0);
    node = MenuWidget_CreateNode(g_MenuItemUseMode ? 0x10 : 0xD, panel, panel);
    panel->update = Menu_ItemListInputHandler;
    panel->disabled = 1;
    node->draw = Menu_DrawEquipSelectionList;
    if (g_MenuLayoutLocked) {
        MenuWidget_SetColumnLayout(node, 0x15);
        if (node->cursor_x < 0)
            node->cursor_x = 0;
        MenuWidget_ClampScroll(node);
    }
    if (g_MenuEquipMode) {
        MenuWidget_OffsetPosition(panel, 0, g_MenuItemUseMode ? -0x10 : 0x14);
        node->cursor_x = -1;
    }
    D_8009CFD4 = Inv_IsActiveListOverrideSelected();
    Inv_SelectActiveList(0);
    D_8009CFC4 = list;
    g_MenuItemRenameMode = list == -2;
    g_MenuItemDiscardMode = list == -3;
    if (g_MenuItemRenameMode) {
        MenuWidget_ClearColumnLayout(node);
        node->cursor_x = 0;
    }
    if (g_MenuLayoutLocked)
        Inv_BuildFilteredPackedListExcluding(mode, -1);
    else if (!g_MenuItemRenameMode && !g_MenuItemDiscardMode)
        Inv_BuildFilteredPackedList(mode);
    Draw_SetPrimCallback(node, Inv_GetPackedListCount());
    if (item >= 0) {
        for (i = 0; i < Inv_GetPackedListCount(); i++) {
            if (Inv_GetPackedListItem(i) == item)
                break;
        }
        if (i < Inv_GetPackedListCount()) {
            node->cursor_y = i;
            MenuWidget_ClampScroll(node);
        }
    }
    if (node->cursor_x >= 0 && !g_MenuEquipMode)
        MenuWidget_SetCurrentNode(node);
    if (g_MenuItemUseMode) {
        MenuWidget_OffsetPosition(panel, 0, 0x38);
        panel->visible_rows -= 0x30;
        panel = MenuWidget_CreateSimpleNode(0x18, 0, 0, 0);
        panel->draw = Menu_DrawBonusPointSlotValue;
        panel->grid_width += 0x14;
        MenuWidget_OffsetPosition(panel, -0x88, 0);
        if (!g_MenuEquipMode) {
            int unfocused = node->cursor_x < 0;

            panel = MenuWidget_CreateSimpleNode(0x30,
                MenuWidget_FindByModeAndSelectedBase(2, 0), 0, 0);
            node = MenuWidget_CreateNode(0x30, panel, panel);
            panel->update = Menu_StatSlotInputHandler;
            node->draw = Menu_DrawParasiteAbilityList;
            if (unfocused) {
                node->cursor_y = 0;
                node->cursor_x = 0;
            }
            if (node->cursor_x >= 0)
                MenuWidget_SetCurrentNode(node);
        }
        {
            u16 *source = D_800C0E00.stats.levels;
            int stat;

            for (stat = 0; stat < 7; stat++) {
                g_BonusPointStatDeltas[stat] = *source++;
                g_BonusPointStatMultipliers[stat] = 0;
                Stat_QueryLevelAndSubLevel(stat, g_BonusPointStatDeltas[stat], &D_800A18B4.queryResults[stat], 0);
            }
        }
        g_MenuBonusPointBarAnimActive = 0;
        g_BonusPointBarAnimProgress = 0;
        g_BonusPointDisplayValue = D_800C0E00.bonus_points;
        for (slot = 6; slot >= 0; slot--)
            D_800A1898[slot] = 0;
        Menu_InitStateTables();
    }
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 1));
    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
    if (g_MenuLayoutLocked && !MenuWidget_FindByModeAndSelectedBase(1, 0x2F))
        MenuWidget_CreateSimpleNode(0x2F, 0, 0, 0)->draw = Menu_DrawItemDetailPanel;
    panel = MenuWidget_CreateSimpleNode(0xB, node, 0, 0);
    node = MenuWidget_CreateNode(0xB, panel, panel);
    panel->disabled = 1;
    node->draw = Menu_DrawUsableItemActionList;
    MenuWidget_ClearColumnLayout(node);
    MenuWidget_OffsetPosition(panel, 6, 0);
    panel = MenuWidget_CreateSimpleNode(0xF, node, 0, 0);
    if (g_MenuItemUseMode) {
        node = MenuWidget_CreateNode(0x1D, panel, panel);
        panel->update = Menu_BonusPointCancelHandler;
        node->draw = Menu_DrawEmptyList;
        node->cursor_x = -1;
        D_800A1960.iconId = 0;
    }
    panel->draw = Menu_StepEquipSlotSelect2;
    if (data)
        D_8009CFC8 = (u8)data->bonusStats[0] & 3;
    else
        D_8009CFC8 = 0;
    if (data)
        D_8009CFCC = data->reserveAmmo;
    else
        D_8009CFCC = 0;
    g_InvItemUsableFlag = mode != 0x200;
    node = MenuWidget_GetCurrentNode();
    if (node->selected_base == 0x30 || node->selected_base == 0x36)
        Menu_SetEquipPanelsCursorY();
    else
        Menu_AlignEquipPanels();
}


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
    u8 *dst;
    u8 *src;
    u16 value;

    dst = (u8 *)g_AyaStatAgility;
    index = 0;
    src = (u8 *)&g_BonusPointStatDeltas;
    do {
        value = *(u16 *)src;
        src += 4;
        index += 1;
        *(u16 *)dst = value;
        dst += 2;
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

int Menu_StatSlotInputHandler(MenuWidgetNode *node, unsigned int flags) {
    MenuWidgetNode *current = MenuWidget_GetChild(node, 0);

    if (flags & 0x4000) {
        MenuWidgetNode *other;
        int x, offset;

        current->cursor_x = -1;
        current = MenuWidget_FindByModeAndSelectedBase(2, 16);
        current->cursor_x = 0;
        current->cursor_y = 0;
        MenuWidget_ClampScroll(current);
        MenuWidget_SetCurrentNode(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 15);
        other = MenuWidget_GetChild(MenuWidget_FindByModeAndSelectedBase(1, 13), 0);
        x = current->x;
        offset = (g_MenuItemUseMode ? 176 : other->popup_node ? 162 : 156) - x;
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 11);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        current = MenuWidget_FindByModeAndSelectedBase(1, 47);
        MenuWidget_OffsetPosition(current, offset, 0);
        MenuWidget_ClearCursorY(current);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        Menu_OpenBonusPointSpendDialog(current, MenuWidget_GridCellIndex(current) + 5);
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        Menu_ExitBonusPointAllocation();
        Menu_PlayCancelSound();
    }
    return 1;
}

#include "common.h"

#define NULL ((void *)0)
#include "pe1/menu_inventory.h"

s32 Menu_BonusPointCancelHandler(void *arg0, s32 arg1) {
    s32 temp_s0;
    s32 var_a0;
    u16 *var_a1;
    u16 *var_v1;
    u16 temp_v0;

    temp_s0 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_OpenBonusPointSpendDialog(temp_s0, MenuWidget_GridCellIndex(temp_s0));
        Menu_PlayConfirmSound();
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0xF);
        MenuWidget_NavScrollTo(0xB);
        MenuWidget_NavScrollTo(0xD);
        MenuWidget_NavScrollTo(0x18);
        MenuWidget_NavScrollTo(0x30);
        if (g_MenuEquipMode != 0) {
            MenuWidget_SetCurrentNode(MenuWidget_FindByModeAndSelectedBase(2, 0x32));
        } else {
            if (MenuWidget_GetCurrentNode() != NULL) {
                MenuWidget_ClearCursorY(MenuWidget_GetCurrentNode()->parent);
            }
            MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 0x1B));
        }
        g_MenuItemRenameMode = 0;
        var_a1 = g_AyaStatAgility;
        var_a0 = 0;
        var_v1 = (u16 *)&g_BonusPointStatDeltas;
        do {
            temp_v0 = *var_v1;
            var_v1 += 2;
            var_a0 += 1;
            *var_a1 = temp_v0;
            var_a1 += 1;
        } while (var_a0 < 7);
        g_AyaBonusPoints[0] = g_BonusPointDisplayValue;
        Menu_StepInventoryRoot(0x37E, -1, -1);
        Menu_PlayCancelSound();
    }
    return 1;
}
#include "pe1/menu_item_list_input.h"
#include "pe1/text.h"

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
    root->update = (void (*)())Menu_InventoryPageInputHandler;
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

/* Same panel as Menu_CreateEquipItemSelectionView. */
static inline void Menu_CreateEquipItemSelectionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(5, parent, 0, 0);
    node = MenuWidget_CreateNode(5, root, root);
    root->draw = Menu_StepEquipSlotSelect;
    root->update = (void (*)())Menu_StepSkillScreen;
    node->draw = Menu_DrawSoundTestList;
    if (g_MenuLayoutLocked != 0)
        MenuWidget_ClearColumnLayout(node);
    else if (g_InvItemUsableFlag == 0)
        MenuWidget_SetColumnLayout(node, 0x12);
    if (node->cursor_x >= 0)
        MenuWidget_SetCurrentNode(node);
    root->disabled = 1;
    node = MenuWidget_CreateNode(0x1B, root, root);
    node->draw = (void (*)())Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(node);
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
}

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
        if (D_8009CFC4 >= 0) {
            switch (D_8009CFC8) {
            case 0:
                data->bonusStats[0] = data->bonusStats[0] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[0] + D_8009CFCC;
                break;
            case 1:
                data->bonusStats[1] = data->bonusStats[1] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[1] + D_8009CFCC;
                break;
            case 2:
                data->bonusStats[2] = data->bonusStats[2] + D_8009CFCC >= 1000
                                          ? 999 : data->bonusStats[2] + D_8009CFCC;
                break;
            }
            Inv_SelectActiveList(D_8009CFD4);
            Inv_RemoveActiveListItem(D_8009CFC4);
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
                    MenuWidget_CreateNode(0x35, submenu, submenu)->draw = (void (*)())Menu_SetupSkillSubmenu;
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
            D_800A1960 = *Inv_LookupActiveListData(Inv_RestoreSelection(0));
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
                dialog->draw = (void (*)())Menu_DrawItemLabel;
                dialog->update = (void (*)())Menu_ConfirmDialogHandler;
                options->draw = (void (*)())Menu_DrawActionOptionList;
                g_MenuActionTextBase = 5;
                MenuWidget_SetCurrentNode(options);
                Inv_SelectActiveList(g_MenuActiveItemList);
                g_MenuItemLabelBuffer[0] = 0xFF;
                Util_AppendFFTerminatedBytes(g_MenuItemLabelBuffer, Str_LookupTable4(0x61));
                D_8009CFA0 = 0;
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
#include "common.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#define CLAMP_EQUIP_PREVIEW(out, item, stat_index, active_stat) do { \
    int desired_ = (active_stat); \
    int current_ = D_8009CFC8; \
    int raw_ = (item)->bonusStats[(stat_index)]; \
    if ((current_ == desired_ && raw_ + D_8009CFCC < 1000) || \
        (current_ != desired_ && raw_ < 1000)) { \
        (out) = (item)->bonusStats[(stat_index)]; \
        if (D_8009CFC8 == (active_stat)) { \
            (out) += D_8009CFCC; \
        } \
    } else { \
        (out) = 999; \
    } \
} while (0)

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
            Inv_RememberSelection(0, D_800C0E20.tracked[0]);
        } else {
            Inv_RememberSelection(0, D_800C0E20.tracked[2]);
        }
    }

    slot = Inv_RestoreSelection(0);
    Draw_OffsetCursor(4, 4);
    Sfx_DrawActiveListSlot(slot);
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

    if (D_8009CFC4 >= 0) {
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
#include "common.h"
#include "pe1/psyq_nop.h"

#include "pe1/inventory.h"
#include "pe1/menu_inventory.h"

void Menu_DrawBonusPointSpendPanel(void);
s32 Spend_BonusPoints(void *node, u32 buttons);

/* COMMON tells MASPSX that the spend value is a $gp word, so it keeps the
 * load delay slot before storing it. */
int g_BonusPointSpendCurrentValue;

void Menu_OpenBonusPointSpendDialog(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *node;
    InvItemSlot *record;

    node = MenuWidget_CreateSimpleNode(9, arg0, 0, 1);
    node->draw = Menu_DrawBonusPointSpendPanel;
    node->update = Spend_BonusPoints;
    node->flags = 1;
    MenuWidget_SetCurrentNode(node);

    g_BonusPointSpendStat = arg1;
    g_BonusPointSpendWorkingValue = g_BonusPointDisplayValue;
    if (arg1 < 3) {
        record = Inv_LookupActiveListData(Inv_RestoreSelection(0));
        D_800A1A00 = *record;
        node->grid_width -= 0x14;
        MenuWidget_OffsetPosition(node, 0xA, 0);
    } else {
        g_BonusPointSpendCurrentValue = g_BonusPointStatDeltas[arg1];
    }
    g_MenuSpendArrowDirection = 0;
}
#include "pe1/inventory.h"


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
            value = D_800A1A00.baseStats[0] + D_800A1A00.bonusStats[0];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(D_800A1A00.baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(D_800A1A00.bonusStats[0]);
            break;
        }
        case 1: {
            value = D_800A1A00.baseStats[1] + D_800A1A00.bonusStats[1];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(D_800A1A00.baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(D_800A1A00.bonusStats[1]);
            break;
        }
        case 2: {
            value = D_800A1A00.baseStats[2] + D_800A1A00.bonusStats[2];
            if (value >= 1000) value = 999;
            Draw_PrintNumberWidth4(value);
            Draw_OffsetCursor(4, 2);
            Draw_PrintNumberWidth4Unk(D_800A1A00.baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(D_800A1A00.bonusStats[2]);
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
        Stat_QueryLevelAndSubLevel(g_BonusPointSpendStat,
                                   g_BonusPointSpendCurrentValue, &level, 0);
        Draw_PrintNumberWidth2(level + 1);
        Stat_QueryDistanceAndSubLevel(g_BonusPointSpendStat,
                                      g_BonusPointSpendCurrentValue, &distance, &sublevel);
        Draw_OffsetCursor(2, 0);
        Draw_AllocTexturedRectAlt(distance, sublevel);
    }
}
#include "pe1/inventory.h"



int Spend_BonusPoints(void *node, unsigned int buttons)
{
    ItemDataRecord *item;
    int amount;
    register int refund asm("$2");
    int changed;

    item = Inv_LookupActiveListData(Inv_RestoreSelection(0));
    if (buttons & 0x4000) {
        g_MenuSpendArrowDirection = 0;
        if (g_BonusPointSpendWorkingValue < 100) {
            goto error;
        }
        changed = 0;
        switch (g_BonusPointSpendStat) {
        case 0: {
            short *bonus = &D_800A1A00.bonusStats[0];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 1: {
            short *bonus = &D_800A1A00.bonusStats[1];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 2: {
            short *bonus = &D_800A1A00.bonusStats[2];
            changed = *bonus < 999;
            *bonus += changed;
            break;
        }
        case 5:
        case 6:
            Stat_QueryDistanceAndSubLevel(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue, &amount, 0);
            changed = amount > 0;
            g_BonusPointSpendCurrentValue += amount;
            break;
        }
        g_BonusPointSpendWorkingValue -= changed * 100;
        if (!changed) {
            goto error;
        }
        Menu_PlayMoveSound();
        return 1;
    }

    if (buttons & 0x1000) {
        g_MenuSpendArrowDirection = 1;
        switch (g_BonusPointSpendStat) {
        case 0: {
            short *bonus = &D_800A1A00.bonusStats[0];
            if (*bonus <= item->bonusStats[0]) {
                goto error;
            }
            {
                short next;
                refund = g_BonusPointSpendWorkingValue;
                asm volatile("" : : "r"(refund));
                next = *bonus - 1;
                asm volatile("" : "=m"(*bonus) : "m"(*bonus));
                *bonus = next;
            }
            break;
        }
        case 1: {
            short *bonus = &D_800A1A00.bonusStats[1];
            if (*bonus <= item->bonusStats[1]) {
                goto error;
            }
            {
                short next;
                refund = g_BonusPointSpendWorkingValue;
                asm volatile("" : : "r"(refund));
                next = *bonus - 1;
                asm volatile("" : "=m"(*bonus) : "m"(*bonus));
                *bonus = next;
            }
            break;
        }
        case 2: {
            short *bonus = &D_800A1A00.bonusStats[2];
            if (*bonus <= item->bonusStats[2]) {
                goto error;
            }
            --*bonus;
            refund = g_BonusPointSpendWorkingValue;
            break;
        }
        case 5:
        case 6: {
            int *minimum = &g_BonusPointStatDeltas[g_BonusPointSpendStat];
            int gap;
            int new_current;
            if (*minimum >= g_BonusPointSpendCurrentValue) {
                goto error;
            }
            gap = Stat_OverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
            if (g_BonusPointSpendCurrentValue - gap < *minimum) {
                new_current = g_BonusPointStatDeltas[g_BonusPointSpendStat];
            } else {
                new_current = g_BonusPointSpendCurrentValue - Stat_OverflowQuery(g_BonusPointSpendStat, g_BonusPointSpendCurrentValue);
            }
            g_BonusPointSpendCurrentValue = new_current;
            refund = g_BonusPointSpendWorkingValue;
            break;
        }
        default:
            return 1;
        }
        g_BonusPointSpendWorkingValue = refund + 100;
        Menu_PlayMoveSound();
        return 1;
    }

    goto after_error;
error:
    Menu_PlayErrorSound();
    return 1;
after_error:
    if (buttons & 0x10000) {
        g_BonusPointDisplayValue = g_BonusPointSpendWorkingValue;
        if (g_BonusPointSpendStat < 3) {
            *item = D_800A1A00;
            if (!Inv_IsActiveListOverrideSelected()) {
                if (item == Inv_LookupActiveListData(D_800C0E20.tracked[0])) {
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
        return 1;
    }

    if (buttons & 0x40) {
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }
    return 1;
}
