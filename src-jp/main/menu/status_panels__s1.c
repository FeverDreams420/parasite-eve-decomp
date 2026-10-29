/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/stat_modifiers.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointDisplayValue;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_InvItemUsableFlag;
MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_InvSelectedItemIndex;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSaveBgFadeIndex;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeTint;

/* Save-screen background fade: gamma curve, per-frame darkening of the
 * captured frame and the fade state machine setters. Contiguous at
 * 0x80042C78 and joined by the fade state words. MemCard_InitSlotState
 * (historical name) arms the fade-in. */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeHeight;

int g_MenuSaveBgFadeState;

int g_MenuItemSelectFlag;

#define NULL ((void *)0)


int g_MenuItemContextFlag;

void Menu_UpdateStatBarAnimation(s32 arg0) {
    s32 sp10;
    s32 sp14;
    s32 current;
    s32 timer;

    Aya_StatQueryDistanceAndSubLevel(
        arg0,
        g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7),
        &sp10,
        &sp14);

    current = g_MenuStatBarWidths[arg0];
    if (sp14 >= current) {
        g_MenuStatBarWidths[arg0] = (current + sp14) >> 1;
    } else if (current < 0x30) {
        g_MenuStatBarWidths[arg0] = 0x30;
        g_MenuStatBarSettleTimers[arg0] = 4;
    } else {
        timer = g_MenuStatBarSettleTimers[arg0];
        if (timer > 0) {
            timer -= 2;
        }
        g_MenuStatBarSettleTimers[arg0] = timer;
        if (timer == 0) {
            g_MenuStatBarWidths[arg0] = sp14;
        }
    }

    Draw_AllocTexturedRectAlt(sp10, g_MenuStatBarWidths[arg0]);
}

int Menu_ClampRange(int arg0) {
    arg0 &= 0x1FF;
    g_MenuItemContextFlag = arg0;
    if (arg0 == 0) {
        g_MenuItemContextFlag = 0x1FF;
    }
    return 0;
}

int Menu_GetItemContextFlag(void) {
    return g_MenuItemContextFlag;
}

/* Inventory stat-panel setup and its adjacent draw callbacks. */

void Menu_CreateInventoryTabView(void) {
    s32 var_a0;
    s32 var_a1;
    s32 var_v1;
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_s1;
    MenuWidgetNode *temp_v0;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0) == 0) {
        temp_v0 = MenuWidget_CreateSimpleNode(0, 0, 0, 0);
        temp_s1 = MenuWidget_CreateNode(0, temp_v0, temp_v0);
        temp_a0 = temp_s1;
        temp_v0->update = Menu_StepItemSelectScreen;
        temp_s1->draw = Menu_DrawItemList;
        MenuWidget_SetCurrentNode(temp_a0);
        if (Menu_GetBattleEquipMode() != 0) {
            var_v1 = g_MenuItemContextFlag & 0x1F;
        } else {
            var_v1 = g_MenuItemContextFlag & 0x1EF;
        }
        var_a1 = 0;
        var_a0 = 8;
        do {
            var_a1 += var_v1 & 1;
            var_a0 -= 1;
            var_v1 = var_v1 >> 1;
        } while (var_a0 >= 0);
        Draw_SetPrimCallback(temp_s1, var_a1);
        g_MenuSelectionLocked = 0;
        g_MenuBattleStatusOverlayActive = 1;
        Menu_CreateBonusPointAllocationView();
        Menu_CreateContextHelpPanel();
    }
}

void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);

void Menu_CreateBonusPointAllocationView(void) {
    int *var_s3;
    int *var_s0;
    int *var_s2;
    s32 var_s1;
    u16 *var_s4;
    MenuWidgetNode *temp_v0;
    MenuWidgetNode *temp_v0_2;
    MenuWidgetNode *temp_v1_reg;

    temp_v0 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x12, 0, 0, 0);
    temp_v1_reg->draw = Menu_DrawStatusPanel;
    temp_v1_reg->appearance.gradientPoints = g_MenuStatusPanelPoints;
    (MenuWidget_CreateSimpleNode(0x18, 0, 0, 0))->draw =
        Menu_DrawBonusPointSlotValue;
    temp_v0_2 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
    var_s4 = &g_AyaSaveState.stats.levels[0];
    var_s1 = 0;
    var_s0 = g_BonusPointStatDeltas;
    var_s3 = g_BonusPointStatQueryResults;
    var_s2 = g_BonusPointStatMultipliers;
    temp_v1_reg->draw = Menu_DrawStatsList;
    temp_v1_reg->appearance.gradientPoints = g_MenuStatsListPanelPoints;
    for (; var_s1 < 7; ++var_s1) {
        *var_s0 = *var_s4++;
        *var_s2 = 0;
        Aya_StatQueryLevelAndSubLevel(var_s1, *var_s0, var_s3, 0);
        var_s3++;
        var_s0++;
        var_s2++;
    }
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;
}

void Menu_DrawStatusPanel(void) {
    s32 var_a0;

    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x47);
    Draw_OffsetCursor(0x28, 2);
    Draw_PrintRawText(Save_GetActiveMetadataBuffer());
    Draw_OffsetCursor(0, 0x15);
    Draw_AllocSprite(0x97);
    Draw_OffsetCursor(0x3C, 0);
    Draw_PrintNumberWidth3(g_AyaSaveState.level + 1);
    Draw_OffsetCursor(-0x78, 0x1A);
    Draw_AllocSprite(0x98);
    Draw_OffsetCursor(0x42, 0);
    if (g_AyaSaveState.level < 0x62U) {
        var_a0 = ((s32 *)Aya_GetLevelExpTable())[g_AyaSaveState.level + 1] - g_AyaSaveState.total_exp;
    } else {
        var_a0 = 0;
    }
    Draw_PrintNumberWidth6(var_a0);
    Draw_OffsetCursor(-0x7C, 0x15);
    Draw_AllocSprite(0x94);
    Draw_OffsetCursor(0, 0x10);
    Menu_DrawActiveListSlot(g_AyaSaveState.equipped_weapon_slot);
    Draw_OffsetCursor(0, 0x10);
    if (g_AyaSaveState.equipped_armor_slot >= 0) {
        Menu_DrawActiveListSlot(g_AyaSaveState.equipped_armor_slot);
        return;
    }
    Draw_PrintTextById(0x39);
}

void Menu_DrawStatsList(void) {
    s32 sp10;
    int var_a0;
    int var_a1;
    s32 *var_s3;
    s32 *var_s4;
    s32 temp_s0;
    s32 temp_s1;
    s32 var_a0_2;
    s32 var_s2;

    var_s4 = Battle_GetModifierTable() + 1;
    Draw_OffsetCursor(4, 5);
    var_s2 = 1;
    var_s3 = &g_BonusPointStatDeltas[1];
    do {
        temp_s1 = *var_s4;
        temp_s0 = *var_s3 + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[var_s2]) >> 7);
        Draw_OffsetCursor(2, 0);
        Draw_AllocSprite(var_s2 + 0x8C);
        Draw_OffsetCursor(0x4A, 0);
        Aya_StatQueryLevelAndSubLevel(var_s2, temp_s0, &sp10, 0);
        var_s4 += 1;
        Draw_PrintNumberWidth2(sp10 + 1);
        if (temp_s1 != 0) {
            Draw_OffsetCursor(6, 0);
            var_a0 = 0x70;
            if (temp_s1 > 0) {
                var_a0_2 = 0x6F;
                var_a0 = var_a0_2;
            }
            Draw_PrintTextById(var_a0);
            /* The printed marker ID is dead; reuse its scratch for the sign. */
            var_a0 = temp_s1 < 0;
            var_a0_2 = temp_s1;
            if (var_a0) {
                var_a0_2 = -var_a0_2;
            }
            Draw_PrintNumberWidth2(var_a0_2);
            Draw_OffsetCursor(-0x18, 0);
        }
        var_a1 = 0xE;
        if (var_s2 == 4) {
            var_a1 = 0x16;
        }
        Draw_OffsetCursor(-0x5E, var_a1);
        var_s2 += 1;
        var_s3 += 1;
    } while (var_s2 < 7);
}

#define CLOSE_SELECT_VIEWS() { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
}

/* Matching debt: the three inventory-root arguments retain retail registers.
 * Empty barriers preserve the return paths and call delay slots. */
int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int buttons)
{
    int index = 0;
    int mask;
    int action;
    MenuWidgetNode *child;
    int confirm = buttons & 0x10000;
    if (confirm) {
        child = MenuWidget_GetChild(node, 0);
        index = MenuWidget_GridCellIndex(child);
        if (Menu_GetBattleEquipMode()) {
            mask = g_MenuItemContextFlag & 0x1f;
        } else {
            mask = g_MenuItemContextFlag & 0x1ef;
        }
        action = -1;
        goto check;
        while (1) {
            if (index < 0) break;
            index -= mask & 1;
            mask >>= 1;
            ++action;
        check:
            if (action >= 9) break;
        }
        switch (action) {
        case 0:
            CLOSE_SELECT_VIEWS();
            Menu_CreateEquipScreen(child);
            break;
        case 1:
            CLOSE_SELECT_VIEWS();
            Menu_CreateSkillActionScreen(child);
            break;
        case 2:
            CLOSE_SELECT_VIEWS();
            g_InvItemUsableFlag = 1;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            Menu_StepSkillList(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 3:
            CLOSE_SELECT_VIEWS();
            g_InvItemUsableFlag = 0;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            /* An int-returning call keeps cross-jumping from merging this
             * case into case 2, which retail keeps separate. */
            ((int (*)(MenuWidgetNode *, int))Menu_StepSkillList)(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 4:
            if (Battle_IsActiveWrapped()) {
                CLOSE_SELECT_VIEWS();
                Inv_SetActiveList(8, 0);
            } else {
                Menu_PlayErrorSound();
                index = 1;
                asm volatile("" : "=r"(index) : "0"(index));
                return index;
            }
            break;
        case 5:
            CLOSE_SELECT_VIEWS();
            Menu_CreateMainMenuView(child);
            break;
        case 6:
            CLOSE_SELECT_VIEWS();
            Menu_CreateInvSwapView(child, 0);
            break;
        case 7:
            CLOSE_SELECT_VIEWS();
            g_MenuLayoutLocked = 1;
            Inv_SelectActiveList(0);
            g_StatBaseTable[0] = Inv_CountByCategory(0xe) ? 999 : Inv_CountByCategory(0xc);
            g_StatBaseTable[1] = Inv_CountByCategory(0xf) ? 999 : Inv_CountByCategory(0xd);
            if (g_MenuEquipMode) {
                g_StatBaseTable[2] = Inv_CountWayneStorageItemType(0xe) ? 999 : Inv_CountWayneStorageItemType(0xc);
                g_StatBaseTable[3] = Inv_CountWayneStorageItemType(0xf) ? 999 : Inv_CountWayneStorageItemType(0xd);
            } else {
                g_StatBaseTable[3] = 0;
                g_StatBaseTable[2] = 0;
            }
            Menu_StepInventoryRoot(0x33e, -1, -1);
            g_MenuBattleStatusOverlayActive = 0;
            break;
        case 8:
            CLOSE_SELECT_VIEWS();
            g_MenuItemUseMode = 1;
            Menu_StepInventoryRoot(0x37e, -1, -1);
            g_MenuBattleStatusOverlayActive = 0;
            break;
        default:
            goto return_one;
        }
        Menu_PlayConfirmSound();
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    if (buttons & 0x40) {
        MenuWidget_InitPool();
        Inv_SetActiveList(9, 0);
        if (!Menu_GetBattleEquipMode()) BattleCmd_ResetTableCursor();
        Menu_PlayCancelSound();
    return_one:
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    {
        int down = buttons & 2;
        asm volatile("" : "=r"(index) : "0"(index));
        if (down) Menu_InitScrollDirection();
    }
    return index;
}