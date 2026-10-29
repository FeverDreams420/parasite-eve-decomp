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

void Menu_CreateLevelUpBanner(void);



void Menu_DrawExpReviewPanel(void) {
    s32 can_step;
    s32 level;
    s32 exp_to_next;

    can_step = g_MenuPendingTotalExp < g_MenuExpAllocTarget;
    if (!can_step) {
        Akao_FlushBgmVolumeFade();
    }

    level = Aya_StatBinarySearch(g_MenuPendingTotalExp, Aya_GetLevelExpTable());
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
    Draw_PrintRawText(Text_LookupTable4(9));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(
        0xF423F < g_MenuPendingTotalExp ? 0xF423F : g_MenuPendingTotalExp);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0xE);
    Draw_StatePush();
    Draw_PrintRawText(Text_LookupTable4(0xA));
    Draw_OffsetCursor(0x50, 0);
    Draw_PrintNumberWidth6(exp_to_next);
    Draw_OffsetCursor(2, 2);
    Draw_AllocSprite(0x8A);
    Draw_StatePop();

    Draw_OffsetCursor(0, 0x14);
    Draw_StatePush();
    Draw_PrintRawText(Text_LookupTable4(0xC));
    Draw_OffsetCursor(0x74, 0);
    Draw_PrintNumberWidth2(Menu_GetBattleCount());
    Draw_OffsetCursor(2, 0);
    Draw_PrintRawText(Text_LookupTable4(0xD));
    Draw_StatePop();
}