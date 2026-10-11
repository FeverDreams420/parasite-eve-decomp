#include "pe1/menu_state.h"
#include "pe1/menu_bonus_stats.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/menu_inventory.h"
#include "pe1/aya.h"
#include "pe1/save.h"
#include "pe1/draw_state.h"
#include "pe1/text.h"
#include "pe1/stat_modifiers.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_rows.h"

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
        temp_v0->update = (void (*)())Menu_StepItemSelectScreen;
        temp_s1->draw = (void (*)())Menu_DrawItemList;
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


/* COMMON tells MASPSX that the shown bonus points are a $gp word, so it
 * keeps the load delay slot before storing them. */
int g_BonusPointDisplayValue;
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
    ((MenuWidgetNode *)MenuWidget_CreateSimpleNode(0x18, 0, 0, 0))->draw =
        Menu_DrawBonusPointSlotValue;
    temp_v0_2 = temp_v1_reg = MenuWidget_CreateSimpleNode(0x2D, 0, 0, 0);
    var_s4 = &D_800C0E00.stats.levels[0];
    var_s1 = 0;
    var_s0 = g_BonusPointStatDeltas;
    var_s3 = g_BonusPointStatQueryResults;
    var_s2 = g_BonusPointStatMultipliers;
    temp_v1_reg->draw = Menu_DrawStatsList;
    temp_v1_reg->appearance.gradientPoints = g_MenuStatsListPanelPoints;
    for (; var_s1 < 7; ++var_s1) {
        *var_s0 = *var_s4++;
        *var_s2 = 0;
        Stat_QueryLevelAndSubLevel(var_s1, *var_s0, var_s3, 0);
        var_s3++;
        var_s0++;
        var_s2++;
    }
    g_BonusPointDisplayValue = D_800C0E00.bonus_points;
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
    Draw_PrintNumberWidth3(D_800C0E00.level + 1);
    Draw_OffsetCursor(-0x78, 0x1A);
    Draw_AllocSprite(0x98);
    Draw_OffsetCursor(0x42, 0);
    if ((u8) D_800C0E00.level < 0x62U) {
        var_a0 = ((s32 *)Aya_GetLevelExpTable())[D_800C0E00.level + 1] - D_800C0E00.total_exp;
    } else {
        var_a0 = 0;
    }
    Draw_PrintNumberWidth6(var_a0);
    Draw_OffsetCursor(-0x7C, 0x15);
    Draw_AllocSprite(0x94);
    Draw_OffsetCursor(0, 0x10);
    Sfx_DrawActiveListSlot(D_800C0E00.equipped_weapon_slot);
    Draw_OffsetCursor(0, 0x10);
    if (D_800C0E00.equipped_armor_slot >= 0) {
        Sfx_DrawActiveListSlot(D_800C0E00.equipped_armor_slot);
        return;
    }
    Draw_PrintTextById(0x39);
}



void Menu_DrawStatsList(void) {
    s32 sp10;
    int var_a0;
    int var_a1;
    s32 *var_s3;
    /* Match debt: with either pin gone loop.c strength-reduces the
     * multiplier index into a sixth saved pointer; retail indexes it
     * (lev 20-25). */
    register s32 *var_s4 asm("$20");
    s32 temp_s0;
    s32 temp_s1;
    register s32 var_a0_2 asm("$4");
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
        Stat_QueryLevelAndSubLevel(var_s2, temp_s0, &sp10, 0);
        var_s4 += 1;
        Draw_PrintNumberWidth2(sp10 + 1);
        if (temp_s1 != 0) {
            Draw_OffsetCursor(6, 0);
            var_a0 = 0x70;
            if (temp_s1 > 0) {
                var_a0 = 0x6F;
            }
            Draw_PrintTextById(var_a0);
            var_a0_2 = temp_s1;
            if (temp_s1 < 0) {
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
