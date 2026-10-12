/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/akao_counters.h"

void Menu_CreateExpReviewView(void);
void Menu_DrawExpReviewPanel(void);
s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags);
void Menu_CreateLevelUpBanner(void);
void Menu_DrawLevelUpBanner(void);
void Menu_CreateLevelUpResultPanel(void);
void Menu_DrawBonusPointAnimFrame(void);
s32 Menu_StepParasiteScreen(MenuWidgetNode *node, s32 flags);

/* COMMON supplies small-data metadata to MASPSX; other words stay absolute. */
int g_MenuPendingTotalExp;
int g_MenuSelectionLocked;
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

    save = &D_800C0E00;
    /* Match debt: retail reads the total and clears the bonus pool through
     * the save base held in s2; a known constant folds to absolute loads. */
    PE1_COMPILER_LAUNDER(save);
    previous = save->total_exp;
    g_MenuPendingTotalExp = previous;
    g_MenuExpAllocTarget = previous + exp_delta;
    g_MenuExpReviewLevel = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    g_MenuSelectionLocked = 1;

    g_AyaBonusPoolRecord.pool = g_AyaBonusPoolRecord.pool + pe_bonus_delta;
    g_MenuLevelDisplayValue = D_800C0E00.level;
    g_MenuHpMaxDisplayValue = D_800C0E00.max_hp;
    g_MenuBonusPointDisplayTarget = D_800C0E00.bonus_points;
    g_BonusPointDisplayValue = D_800C0E00.bonus_points;
    g_MenuLevelDisplayTarget = Stat_BinarySearch(g_MenuExpAllocTarget, Aya_GetLevelExpTable());

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

    stat_src = (s16 *)D_800C0E00.stats.levels;
    i = 0;
    do {
        g_BonusPointStatDeltas[i] = *stat_src++;
        Stat_QueryLevelAndSubLevel(i, g_BonusPointStatDeltas[i], &D_800A18B4.queryResults[i], 0);
        g_BonusPointStatMultipliers[i] = (g_MenuLevelDisplayTarget - g_MenuLevelDisplayValue) * 10;
        i++;
    } while (i < 7);

    Stat_QueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0] + g_BonusPointStatMultipliers[0], &stat_level, 0);
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

void Menu_DrawExpReviewPanel(void) {
    s32 can_step;
    s32 level;
    s32 exp_to_next;

    can_step = g_MenuPendingTotalExp < g_MenuExpAllocTarget;
    if (!can_step) {
        Akao_FlushBgmVolumeFade();
    }

    level = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
    if (level < 0x62) {
        exp_to_next = ((s32 *)Aya_GetLevelExpTable())[level + 1] - g_MenuPendingTotalExp;
    } else {
        exp_to_next = 0;
    }

    g_MenuPendingTotalExp += can_step;
    if (g_MenuExpReviewLevel < level) {
        g_MenuExpReviewLevel = level;
        g_MenuLevelUpAnimTimer = 0x3C;
        if (MenuWidget_FindByModeAndSelectedBase(1, 0x16) == 0) {
            Menu_CreateLevelUpBanner();
        }
    }

    if (g_MenuLevelUpAnimTimer != 0) {
        g_MenuLevelUpAnimTimer--;
        if (g_MenuLevelUpAnimTimer == 0) {
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
        }
    }

    Draw_OffsetCursor(4, 4);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(9));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(
        0xF423F < g_MenuPendingTotalExp ? 0xF423F : g_MenuPendingTotalExp);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0xE);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(0xA));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(exp_to_next);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0x14);
    Draw_StatePush();
    Draw_PrintRawText(Str_LookupTable4(0xC));
    Draw_OffsetCursor(0x74, 0);
    Draw_PrintNumberWidth2(Menu_GetBattleCount());
    Draw_StatePop();
}


s32 Menu_ConfirmExpAllocation(MenuWidgetNode *node, s32 flags) {
    s32 confirm;
    s32 new_level;

    confirm = flags & 0x10000;
    if (confirm != 0) {
        if (g_MenuPendingTotalExp < g_MenuExpAllocTarget) {
            g_MenuPendingTotalExp = g_MenuExpAllocTarget;
        } else {
            MenuWidget_DestroyNode(node);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x16));
            Akao_FlushBgmVolumeFade();
            new_level = Stat_BinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
            if (new_level != Stat_BinarySearch(D_800C0E00.total_exp, Aya_GetLevelExpTable())) {
                Menu_CreateLevelUpResultPanel();
                Akao_SetBgmVolumeFade();
            } else if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
            D_800C0E00.total_exp = g_MenuPendingTotalExp;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_CreateLevelUpBanner(void) {
    MenuWidget_CreateSimpleNode(0x16, 0, 0, 0)->draw = Menu_DrawLevelUpBanner;
}

void Menu_DrawLevelUpBanner(void) {
    Draw_OffsetCursor(0, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x19));
}

void Menu_InitBonusPointAllocState(int gained_points) {
    int scratch[2];
    s16 *source;
    int *deltas;
    int *query_results;
    int *multipliers;
    int stat;
    int value;

    BattleCmd_SyncActiveAmmo();

    g_MenuPendingTotalExp = D_800C0E00.total_exp;
    g_MenuExpAllocTarget = D_800C0E00.total_exp;
    g_MenuLevelDisplayValue = D_800C0E00.level;
    g_MenuExpReviewLevel = D_800C0E00.level;
    g_MenuLevelDisplayTarget = D_800C0E00.level;
    g_MenuHpMaxDisplayValue = D_800C0E00.max_hp;
    g_MenuHpMaxDisplayTarget = D_800C0E00.max_hp;
    g_BonusPointDisplayValue = D_800C0E00.bonus_points;

    if (gained_points < 0) {
        gained_points = 0;
    }
    g_MenuBonusPointDisplayTarget = D_800C0E00.bonus_points + gained_points;

    source = (s16 *)D_800C0E00.stats.levels;
    stat = 0;
    multipliers = g_BonusPointStatMultipliers;
    query_results = g_BonusPointStatQueryResults;
    deltas = g_BonusPointStatDeltas;
    do {
        value = *source++;
        *deltas = value;
        Stat_QueryLevelAndSubLevel(stat, value, query_results, 0);
        *multipliers = 0;
        multipliers++;
        query_results++;
        deltas++;
        stat++;
    } while (stat < 7);

    Stat_QueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0], scratch, 0);
    g_BonusPointBarAnimStep = 2;
    Inv_LoadWayneItemsAsOverride(0);
    Menu_CreateLevelUpResultPanel();
    MenuWidget_OffsetPosition(MenuWidget_FindByModeAndSelectedBase(1, 0x15), 0, 0x20);
    if (gained_points > 0) {
        Akao_SetBgmVolumeFade();
    }
    Menu_ResetInputState();
}


void Menu_CreateLevelUpResultPanel(void) {
    MenuWidgetNode *root;
    MenuWidgetNode *child;

    root = MenuWidget_CreateSimpleNode(0x15, 0, 0, 0);
    child = MenuWidget_CreateNode(0x2F, root, root);
    root->draw = Menu_DrawBonusPointAnimFrame;
    root->update = Menu_StepParasiteScreen;
    child->draw = Menu_DrawStatAllocationList;
    child->cursor_x = -1;
    child->x += 0x44;
    child->y += 2;
    child->disabled -= 2;
    MenuWidget_SetCurrentNode(root);
    root->appearance.gradientPoints = g_MenuLevelUpPanelPoints;
    g_MenuBonusPointBarAnimActive = 1;
    Menu_InitStateTables();
}



void Menu_InitStateTables(void) {
    int i;

    for (i = 0; i < 8; i++) {
        g_MenuStatBarSettleTimers[i] = 0;
        g_MenuStatBarWidths[i] = 0;
    }
}


void Menu_DrawBonusPointAnimFrame(void) {
    int var_a0;
    int var_a0_2;
    int var_a0_3;
    s32 temp_a0;
    s32 temp_v0;
    s32 temp_v1;
    s32 var_s0;
    s32 var_s0_2;

    Draw_OffsetCursor(4, 4);
    Draw_AllocSprite(0x97);
    var_s0 = 0;
    if (g_MenuLevelDisplayValue < g_MenuLevelDisplayTarget) {
        temp_v0 = g_MenuLevelUpAnimTimer < 0x1E;
        g_MenuLevelUpAnimTimer += 1;
        var_s0 = 1;
        if (temp_v0 == 0) {
            temp_a0 = g_MenuLevelDisplayValue + 1;
            g_MenuLevelUpAnimTimer = 0;
            g_MenuLevelDisplayValue = temp_a0;
            Aya_UnlockParasiteSpellById(temp_a0);
        }
    }
    var_a0 = 0x808080;
    if (D_800C0E00.level < g_MenuLevelDisplayValue) {
        var_a0 = 0x8080;
    }
    Draw_SetColor(var_a0);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuLevelDisplayValue + 1);
    Draw_SetColor(0x808080);
    Draw_OffsetCursor(-0x3C, 0x14);
    Draw_AllocSprite(0x99);
    if (g_MenuHpMaxDisplayValue < g_MenuHpMaxDisplayTarget) {
        temp_v1 = g_MenuHpMaxAnimTimer;
        g_MenuHpMaxAnimTimer = temp_v1 + 1;
        var_s0 = 1;
        if (temp_v1 >= 0) {
            g_MenuHpMaxAnimTimer = 0;
            g_MenuHpMaxDisplayValue += 1;
        }
    }
    var_a0_2 = 0x808080;
    if (D_800C0E00.max_hp < g_MenuHpMaxDisplayValue) {
        var_a0_2 = 0x8080;
    }
    Draw_SetColor(var_a0_2);
    Draw_OffsetCursor(0xF, 0x10);
    Draw_PrintNumberWidth5(g_MenuHpMaxDisplayValue);
    Draw_OffsetCursor(-0x3C, 0x13);
    Draw_SetColor(0x808080);
    Draw_AllocSprite(0x93);
    if (g_BonusPointDisplayValue < g_MenuBonusPointDisplayTarget) {
        g_BonusPointDisplayValue += 1;
        var_s0 = 1;
    }
    temp_v1 = g_BonusPointDisplayValue;
    var_a0_3 = 0x808080;
    if ((int)D_800C0E00.bonus_points < temp_v1) {
        var_a0_3 = 0x8080;
    }
    Draw_SetColor(var_a0_3);
    Draw_OffsetCursor(0xF, 0x11);
    Draw_PrintNumberWidth5(g_BonusPointDisplayValue);
    Draw_OffsetCursor(0xA, -0x56);
    Draw_SetColor(0x808080);
    if (g_MenuBonusPointBarAnimActive != 0) {
        if (g_BonusPointBarAnimProgress < 0x80) {
            g_BonusPointBarAnimProgress += g_BonusPointBarAnimStep;
        }
        if ((var_s0 == 0) && (g_BonusPointBarAnimProgress >= 0x80)) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuBonusPointBarAnimActive = 0;
        }
    }
    Draw_OffsetCursor(0x78, 2);
    var_s0_2 = 1;
    do {
        Menu_UpdateStatBarAnimation(var_s0_2);
        Draw_OffsetCursor(0, 0x10);
        var_s0_2 += 1;
    } while (var_s0_2 < 7);
    if (g_MenuBonusPointBarAnimActive == 0) {
        Akao_FlushBgmVolumeFade();
    }
}

s32 Menu_StepParasiteScreen(MenuWidgetNode *arg0, s32 arg1) {
    u16 *statDestination;
    s32 *statDeltas;
    s32 *statMultipliers;
    s32 temp_v0;
    s32 temp_v1;
    s32 value;
    s32 var_a0;
    s32 var_a2_old;
    s32 var_a2;

    if (arg1 & 0x10000) {
        if (g_MenuBonusPointBarAnimActive != 0) {
            g_BonusPointBarAnimProgress = 0x80;
            g_MenuHpMaxDisplayValue = g_MenuHpMaxDisplayTarget;
            g_BonusPointDisplayValue = g_MenuBonusPointDisplayTarget;
            var_a2_old = g_MenuLevelDisplayValue;
            while (var_a2_old < g_MenuLevelDisplayTarget) {
                var_a0 = var_a2_old + 1;
                g_MenuLevelDisplayValue = var_a0;
                value = Aya_UnlockParasiteSpellById(var_a0);
                if (value != 0) {
                    break;
                }
                var_a2_old = g_MenuLevelDisplayValue;
            }
        } else {
            statDestination = D_800C0E00.stats.transfer;
            var_a2 = 0;
            statMultipliers = g_BonusPointStatMultipliers;
            statDeltas = g_BonusPointStatDeltas;
            do {
                *statDestination++ = *statDeltas++ + *statMultipliers++;
                var_a2 += 1;
            } while (var_a2 < 9);
            temp_v0 = g_MenuLevelDisplayValue;
            temp_v1 = g_MenuHpMaxDisplayValue;
            value = g_BonusPointDisplayValue;
            D_800C0E00.level = temp_v0;
            D_800C0E00.max_hp = temp_v1;
            D_800C0E00.bonus_points = value;
            Inv_RecalcSlotStats();
            MenuWidget_DestroyNode(arg0);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1E));
            MenuWidget_NavScrollTo(0x1C);
            Akao_FlushBgmVolumeFade();
            var_a0 = 0xA;
            if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}
