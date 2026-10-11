/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/text.h"
#include "pe1/inventory.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_sound.h"

/* Per-row item drawers the widget list renderer calls with the row index:
 * action options, equipped weapon/armor, action codes, modifier names, armor
 * cells, table text rows, stat and ability bars, item icons, save slots and
 * the selected item's sound-effect cursor. */

int g_MenuActionTextBase;
int g_InvItemUsableFlag;
int g_MenuLayoutLocked;
MenuWidgetNode *g_MenuActiveWidget;
unsigned char *g_MenuItemIconGlyphTable;
void Draw_PrintNumberWidth2(int arg0);
void Draw_StatePush(void);
void Draw_StatePop(void);
void Draw_PrintNumberWidth2Unk(int);
void Draw_PrintNumberWidth3Unk(int value);
void Menu_UpdateStatBarAnimation(int);
void Draw_AllocTexturedQuad(int);
void Inv_ShowItemDescriptionPanel(void);

void Menu_DrawActionOptionRow(int arg0) {
    Draw_SetTextDimmed(0);
    MenuWidget_DrawCenteredTableText(g_MenuActionTextBase + arg0);
}

void Menu_SoundTestSelect(int index)
{
    int selected;
    int value;
    MenuWidgetNode *node;

    if (g_MenuLayoutLocked != 0) {
        selected = Inv_RestoreSelection(0);
    } else {
        Draw_SetTextDimmed(Battle_IsInputAllowedWrapped() == 0);
        if (g_InvItemUsableFlag != 0) {
            value = g_AyaEquippedWeaponSlot[0];
        } else {
            value = g_AyaEquippedArmorSlot[0];
        }
        selected = value;
    }

    if (selected >= 0) {
        MenuWidget_ClearCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
        Sfx_DrawActiveListSlot(selected);
        return;
    }

    MenuWidget_SetCursorY(MenuWidget_FindByModeAndSelectedBase(1, 6));
    node = MenuWidget_FindByModeAndSelectedBase(2, 6);
    if (node->cursor_x >= 0) {
        node->cursor_x = -1;
        node = MenuWidget_FindByModeAndSelectedBase(2, 5);
        node->cursor_x = 0;
        MenuWidget_SetCurrentNode(node);
    }
    Draw_PrintTextById(0x39);
}

void Menu_DrawActionCodeItem(int arg0) {
    int code;

    if (arg0 < g_MenuActionItemData->tailCount) {
        code = g_MenuActionItemData->tailData[arg0] & 0x1F;
        if (code == 0) {
            return;
        }

        if (g_InvItemUsableFlag != 0) {
            Draw_AllocSprite(code + 0x22);
        } else {
            Draw_AllocSprite(code + 0x36);
        }
    } else {
        Draw_AllocSprite(0x46);
    }
}

void Menu_DrawModNameItem(int arg0) {
    if (arg0 < Inv_GetPackedListCount()) {
        Draw_PrintRawText(Str_LookupTable8(Inv_GetPackedListItem(arg0) + 0xEB));
    }
}

void Menu_DrawArmorListItem(int arg0) {
    if (arg0 != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(arg0 + 0x60);
}

void Menu_DrawArmorEquipItem(int index) {
    Inv_ShowItemDescriptionPanel();
}

void Menu_DrawMemCardPortItem(int arg0) {
    Draw_OffsetCursor(-2, -2);
    if (arg0 < 2) {
        Draw_AllocSprite(arg0 + 0x84);
    } else {
        Draw_AllocSprite(0x62);
    }
}

void Menu_DrawMainMenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x28);
}

void Menu_DrawActionSubmenuItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x2E);

    if (Menu_GetActionSubmenuSelection() == arg0) {
        Draw_DropShadow();
    }
}

void Menu_DrawCursorModeItem(int arg0) {
    MenuWidget_DrawCenteredTableText(arg0 + 0x31);

    if (MenuWidget_GetColumnLayoutMode() == arg0) {
        Draw_DropShadow();
    }
}

void Menu_DrawNotificationItem(int index) {
    MenuWidget_DrawCenteredTableText(8);
}

void Menu_DrawInvPanelItemStub(int index) {
}

void Menu_DrawStatAllocationItem(int arg0) {
    int value;
    int local;

    arg0 += 1;
    value = g_BonusPointStatDeltas[arg0] + ((g_BonusPointBarAnimProgress * g_BonusPointStatMultipliers[arg0]) >> 7);

    Draw_OffsetCursor(6, 0);
    Draw_AllocSprite(arg0 + 0x8C);
    Draw_OffsetCursor(0x4A, 0);
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
}

void Menu_DrawScreenModeItem(int arg0) {
    Draw_StatePush();
    Draw_PrintTextById((arg0 & 1) | 0x6A);

    if ((arg0 >= 2) || (g_StatBaseTable[arg0] >= 100)) {
        Draw_OffsetCursor(0x28, 3);
        Draw_PrintNumberWidth3Unk(g_StatBaseTable[arg0]);
    } else {
        Draw_OffsetCursor(0x2D, 3);
        Draw_PrintNumberWidth2Unk(g_StatBaseTable[arg0]);
    }

    Draw_StatePop();
}

void Menu_DrawParasiteAbilityItem(int arg0) {
    int value;
    int local;

    value = g_BonusPointStatDeltas[arg0 + 5];

    Draw_OffsetCursor(2, 1);
    Draw_AllocSprite(arg0 + 0x91);
    Draw_OffsetCursor(0x42, -1);

    arg0 += 5;
    Stat_QueryLevelAndSubLevel(arg0, value, &local, 0);
    Draw_PrintNumberWidth2(local + 1);
    Draw_OffsetCursor(2, 0);
    Menu_UpdateStatBarAnimation(arg0);
    Draw_OffsetCursor(-0x44, 0xE);
}

void Menu_DrawItemIcon(int arg0) {
    Draw_StatePush();
    Draw_OffsetCursor(2, 0);
    Draw_AllocTexturedQuad(g_MenuItemIconGlyphTable[arg0]);
    Draw_StatePop();
}

void Menu_DrawSaveSlot1Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x63);
}

void Menu_DrawSaveSlot2Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x66);
}

void Menu_DrawSaveSlot3Item(int index) {
    if (index != MenuWidget_GridCellIndex(g_MenuActiveWidget)) {
        Draw_SetTextDimmed(1);
    }
    Draw_OffsetCursor(-2, -2);
    Draw_AllocSprite(index + 0x62);
}

void Menu_DrawSoundEffectItem(int index) {
    Sfx_CursorRenderData(g_MenuSelectedItemData);
}
