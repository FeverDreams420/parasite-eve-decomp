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
            new_level = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
            if (new_level != Aya_StatBinarySearch(g_AyaSaveState.total_exp, Aya_GetLevelExpTable())) {
                Menu_CreateLevelUpResultPanel();
                Akao_SetBgmVolumeFade();
            } else if (Menu_GetBattleCount() != 0) {
                Menu_StepSaveLoadScreen();
            } else {
                Inv_SetActiveList(0xA, 0);
            }
            g_AyaSaveState.total_exp = g_MenuPendingTotalExp;
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
    Draw_PrintCenteredText(Text_LookupTable4(0x19));
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

    g_MenuPendingTotalExp = g_AyaSaveState.total_exp;
    g_MenuExpAllocTarget = g_AyaSaveState.total_exp;
    g_MenuLevelDisplayValue = g_AyaSaveState.level;
    g_MenuExpReviewLevel = g_AyaSaveState.level;
    g_MenuLevelDisplayTarget = g_AyaSaveState.level;
    g_MenuHpMaxDisplayValue = g_AyaSaveState.max_hp;
    g_MenuHpMaxDisplayTarget = g_AyaSaveState.max_hp;
    g_BonusPointDisplayValue = g_AyaSaveState.bonus_points;

    if (gained_points < 0) {
        gained_points = 0;
    }
    g_MenuBonusPointDisplayTarget = g_AyaSaveState.bonus_points + gained_points;

    source = g_AyaSaveState.stats.levels;
    stat = 0;
    multipliers = g_BonusPointStatMultipliers;
    query_results = g_BonusPointStatQueryResults;
    deltas = g_BonusPointStatDeltas;
    do {
        value = *source++;
        *deltas = value;
        Aya_StatQueryLevelAndSubLevel(stat, value, query_results, 0);
        *multipliers = 0;
        multipliers++;
        query_results++;
        deltas++;
        stat++;
    } while (stat < 7);

    Aya_StatQueryLevelAndSubLevel(0, g_BonusPointStatDeltas[0], scratch, 0);
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

#define NULL ((void *)0)
