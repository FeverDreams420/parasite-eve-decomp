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

void Menu_CreateMainMenuView(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;

    parent = MenuWidget_CreateSimpleNode(0x20, arg0, 0, 0);
    child = MenuWidget_CreateNode(0x20, parent, parent);
    parent->update = Menu_MainMenuInputHandler;
    child->draw = Menu_DrawMainMenuList;
    MenuWidget_SetCurrentNode(child);
    Draw_SetPrimCallback(child, 4);
}

int Menu_MainMenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *child = MenuWidget_GetChild(arg0, 0);

    if (arg1 & 0x10000) {
        switch (MenuWidget_GridCellIndex(child)) {
        case 0:
            Menu_CreateActionSubmenu(child);
            break;
        case 1:
            Menu_OpenCursorModeMenu(child);
            break;
        case 2:
            Menu_OpenBlendColorScreen(child);
            break;
        case 3:
            Menu_OpenScreenAdjustView(child);
            break;
        case 4:
        case 5:
            Inv_InitMaxLevelInventory(MenuWidget_GridCellIndex(child) - 4);
            MenuWidget_DestroyNode(arg0);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayConfirmSound();
            break;
        }
        Menu_PlayConfirmSound();
        return 1;
    }

    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_CreateBonusPointAllocationView();
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_CreateActionSubmenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x21, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x21, created, created);
    created->update = Menu_ActionSubmenuInputHandler;
    node->draw = Menu_DrawActionSubmenuList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_ActionSubmenuInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        Menu_SetActionSubmenuSelection(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_OpenCursorModeMenu(MenuWidgetNode *arg0) {
    MenuWidgetNode *created;
    MenuWidgetNode *node;

    created = MenuWidget_CreateSimpleNode(0x23, arg0, 0, 0);
    node = MenuWidget_CreateNode(0x23, created, created);
    created->update = Menu_CursorModeInputHandler;
    node->draw = Menu_DrawCursorModeList;
    MenuWidget_SetCurrentNode(node);
}

int Menu_CursorModeInputHandler(MenuWidgetNode *arg0, u32 arg1) {
    MenuWidgetNode *base;

    base = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x10000) {
        Menu_PlayConfirmSound();
        MenuWidget_SetColumnLayoutMode(MenuWidget_GridCellIndex(base));
        MenuWidget_DestroyNode(arg0);
        return 1;
    }
    if (arg1 & 0x40) {
        MenuWidget_DestroyNode(arg0);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorArrows(MenuWidgetNode *panel);
s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags);
void Menu_DrawBlendColorChannelList(int node);
void Menu_DrawBlendColorOptionList(int node);
void Menu_DrawScreenAdjustPanel(void);
int Menu_ScreenAdjustInputHandler(void *node, int input);

void Menu_OpenBlendColorScreen(MenuWidgetNode *arg0) {
    MenuWidgetNode *parent;
    MenuWidgetNode *panel;
    MenuWidgetNode *options;

    parent = MenuWidget_CreateSimpleNode(0x2E, arg0, 0, 0);
    panel = MenuWidget_CreateNode(0x2E, parent, parent);
    parent->draw = Menu_DrawBlendColorArrows;
    parent->update = Menu_StepColorSelect;
    parent->appearance.gradientPoints = g_MenuBlendColorPanelPoints;
    parent->disabled = 1;
    panel->draw = Menu_DrawBlendColorChannelList;
    panel->flags = 1;
    options = MenuWidget_CreateNode(0x31, parent, parent);
    options->draw = Menu_DrawBlendColorOptionList;
    panel->linkedNext = options;
    options->linkedPrevious = panel;
    MenuWidget_SetCurrentNode(panel->cursor_x < 0 ? options : panel);
    g_MenuBlendColorBackup = Draw_GetBlendColor();
}

/* Up/down arrows over the three colour channels; the pair over the selected
 * channel lights up while that direction is held. */
void Menu_DrawBlendColorArrows(MenuWidgetNode *panel) {
    int dimmed0;
    int dimmed1;
    int dimmed2;
    int dimmed3;
    int dimmed4;
    int dimmed5;
    u16 directionHeld;
    s32 selectedChannel;

    selectedChannel = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0));
    directionHeld = MenuInput_GetStatusFlags() & 0x1000;
    Draw_OffsetCursor(0x12, 5);
    dimmed0 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed0 = 1;
    }
    Draw_SetTextDimmed(dimmed0);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed1 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed1 = 1;
    }
    Draw_SetTextDimmed(dimmed1);
    Draw_AllocSprite(0x4A);
    Draw_OffsetCursor(0x20, 0);
    dimmed2 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed2 = 1;
    }
    Draw_SetTextDimmed(dimmed2);
    Draw_AllocSprite(0x4A);
    directionHeld = MenuInput_GetStatusFlags() & 0x4000;
    Draw_OffsetCursor(-0x40, 0x19);
    dimmed3 = 0;
    if ((selectedChannel != 0) || (directionHeld == 0)) {
        dimmed3 = 1;
    }
    Draw_SetTextDimmed(dimmed3);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed4 = 0;
    if ((selectedChannel != 1) || (directionHeld == 0)) {
        dimmed4 = 1;
    }
    Draw_SetTextDimmed(dimmed4);
    Draw_AllocSprite(0x4B);
    Draw_OffsetCursor(0x20, 0);
    dimmed5 = 0;
    if ((selectedChannel != 2) || (directionHeld == 0)) {
        dimmed5 = 1;
    }
    Draw_SetTextDimmed(dimmed5);
    Draw_AllocSprite(0x4B);
}

s32 Menu_StepColorSelect(MenuWidgetNode *panel, s32 flags) {
    s32 shift;
    s32 choice;
    s32 color;
    s32 level;
    s32 value;
    s32 others;
    MenuWidgetNode *options;

    shift = MenuWidget_GridCellIndex(MenuWidget_GetChild(panel, 0)) << 3;
    color = Draw_GetBlendColor();
    if (flags & 0x1000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) + 2;
        if (level < 0xE9) {
            value = level << shift;
        } else {
            value = 0xE8 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x4000) {
        others = color & ~(0xFF << shift);
        level = ((color >> shift) & 0xFF) - 2;
        if (level >= 0x20) {
            value = level << shift;
        } else {
            value = 0x20 << shift;
        }
        Draw_BlendColor(others | value);
        Menu_PlayMoveSound();
    } else if (flags & 0x10000) {
        options = MenuWidget_GetChild(panel, 1);
        choice = MenuWidget_GridCellIndex(options);
        if (choice == 0) {
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else if (choice == 1) {
            Draw_BlendColor(0x404040);
            g_SavedDrawBlendColor[0] = Draw_GetBlendColor();
            MenuWidget_DestroyNode(panel);
        } else {
            options->cursor_x = 0;
            options->cursor_y = 0;
            MenuWidget_SetCurrentNode(options);
            MenuWidget_GetChild(panel, 0)->cursor_x = -1;
        }
        Menu_PlayConfirmSound();
    } else if (flags & 0x40) {
        MenuWidget_DestroyNode(panel);
        Draw_BlendColor(g_MenuBlendColorBackup);
        Menu_PlayCancelSound();
    }
    return 1;
}

void Menu_DrawBlendColorChannelList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorChannelRow);
}

int g_MenuScreenBaseYBackup;

void Menu_DrawBlendColorOptionList(int node) {
    MenuWidget_DrawList((MenuWidgetNode *)node, Menu_DrawBlendColorOptionRow);
}

void Menu_OpenScreenAdjustView(MenuWidgetNode *parent) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x38, parent, 0, 1);
    node->draw = Menu_DrawScreenAdjustPanel;
    node->update = Menu_ScreenAdjustInputHandler;
    MenuWidget_SetCurrentNode(node);
    g_MenuScreenBaseYBackup = Draw_GetBaseY();
}

void Menu_DrawScreenAdjustPanel(void) {
    Draw_OffsetCursor(0x10, 0xA);
    Draw_PrintSignedNumberWidth3(8 - Draw_GetBaseY());
    Draw_SetTextDimmed(VSync(-1) & 0x10);
    Draw_OffsetCursor(-0x10, -0x69);
    Draw_AllocSprite(0x7B);
    Draw_OffsetCursor(0, 0xBE);
    Draw_EmitGlyph(0x7B, 2);
}

int Menu_ScreenAdjustInputHandler(void *node, int input) {
    int zero_arg;

    {
        if ((input & 0x1000) != 0) {
            zero_arg = 0;
            input = -1;
        } else if ((input & 0x4000) != 0) {
            zero_arg = 0;
            input = 1;
        } else {
            goto screen_adjust_other;
        }
        Draw_SetBaseOffsetPosition(zero_arg, input);
        Menu_PlayMoveSound();
        return 1;
    }
screen_adjust_other:

    if ((input & 0x10000) != 0) {
        MenuWidget_DestroyNode(node);
        Menu_PlayConfirmSound();
        return 1;
    }

    if ((input & 0x40) != 0) {
        Draw_SetBaseOffsetPosition(0, g_MenuScreenBaseYBackup - Draw_GetBaseY());
        MenuWidget_DestroyNode(node);
        Menu_PlayCancelSound();
    }

    return 1;
}

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
/* The stat loop sign-extends the allocation levels (lh in retail). */

void Aya_SetTotalExp(s32 exp_delta, s32 pe_bonus_delta, void *wayne_items) {
    AyaSaveState *save;
    s32 previous;
    s16 *stat_src;
    s32 i;
    s32 stat_level;

    Menu_SetBattleEquipMode(0);
    BattleCmd_SyncActiveAmmo();

    save = &g_AyaSaveState;
    /* Match debt: retail reads the total and clears the bonus pool through
     * the save base held in s2; a known constant folds to absolute loads. */
    PE1_COMPILER_LAUNDER(save);
    previous = save->total_exp;
    g_MenuPendingTotalExp = previous;
    g_MenuExpAllocTarget = previous + exp_delta;
    g_MenuExpReviewLevel = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    g_MenuSelectionLocked = 1;

    g_AyaBonusPoolRecord.pool = g_AyaBonusPoolRecord.pool + pe_bonus_delta;
    g_MenuLevelDisplayValue = g_AyaSaveState.level;
    g_MenuHpMaxDisplayValue = g_AyaSaveState.max_hp;
    g_MenuBonusPointDisplayTarget = g_AyaSaveState.bonus_points;
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;
    g_MenuLevelDisplayTarget = Aya_StatBinarySearch(g_MenuExpAllocTarget, Aya_GetLevelExpTable());

    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        s32 total;
        s32 bonus = g_MenuBonusPointDisplayTarget + g_AyaBonusPoolRecord.pool;
        if (bonus <= 0x1869F) {
            total = bonus;
        } else {
            total = 0x1869F;
        }
        g_MenuBonusPointDisplayTarget = total;
        save->pe_bonus_pool = 0;
    }

    stat_src = g_AyaSaveState.stats.levels;
    i = 0;
    do {
        g_BonusPointStatDeltas[i] = *stat_src++;
        Aya_StatQueryLevelAndSubLevel(i, g_BonusPointStatDeltas[i], &g_BonusPointStatQueryResults[i], 0);
        g_BonusPointStatMultipliers[i] = (g_MenuLevelDisplayTarget - g_MenuLevelDisplayValue) * 10;
        i++;
    } while (i < 7);

    Aya_StatQueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0] + g_BonusPointStatMultipliers[0], &stat_level, 0);
    g_MenuHpMaxDisplayTarget = Battle_GetScaledMaxHP(stat_level);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(wayne_items);
    Menu_CreateExpReviewView();
    Akao_SetBgmVolumeFade();
    Menu_ResetInputState();
}

void Menu_CreateExpReviewView(void) {
    MenuWidgetNode *node;

    node = MenuWidget_CreateSimpleNode(0x14, 0, 0, 0);
    node->draw = Menu_DrawExpReviewPanel;
    node->update = Menu_ConfirmExpAllocation;
    MenuWidget_SetCurrentNode(node);
    node->appearance.gradientPoints = g_MenuExpReviewPanelPoints;
}

#define NULL ((void *)0)
