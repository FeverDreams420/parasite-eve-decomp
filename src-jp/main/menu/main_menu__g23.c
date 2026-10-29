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

#define NULL ((void *)0)




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
    if (g_AyaSaveState.level < g_MenuLevelDisplayValue) {
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
    if (g_AyaSaveState.max_hp < g_MenuHpMaxDisplayValue) {
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
    if (g_AyaSaveState.bonus_points < temp_v1) {
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
            statDestination = g_AyaSaveState.stats.transfer;
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
            g_AyaSaveState.level = temp_v0;
            g_AyaSaveState.max_hp = temp_v1;
            g_AyaSaveState.bonus_points = value;
            Inv_RecalcSlotStats();
            MenuWidget_DestroyNode(arg0);
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x1E));
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