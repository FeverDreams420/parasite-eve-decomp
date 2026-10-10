/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/text.h"

/* Contiguous list-draw adapters: each binds a widget list to its row drawer,
 * plus the icon, save-slot, sound-effect and equip-detail lists and the
 * equip sort-toggle and info-option rows. */

MenuWidgetNode *g_MenuActiveWidget;
ItemDataRecord *g_MenuActionItemData;
int g_InvItemUsableFlag;
extern unsigned char *g_MenuItemIconGlyphTable;
extern short D_800922D0[];

void Menu_DrawItemIcon(int index);
void Menu_DrawSaveSlot1Item(int index);
void Menu_DrawSaveSlot2Item(int index);
void Menu_DrawSaveSlot3Item(int index);
void Menu_DrawSoundEffectItem(int index);
void Menu_DrawActionCodeItem(int index);
void Draw_SetTextDimmed(int dimmed);

void Menu_DrawMainMenuItem(int arg0);

void Menu_DrawMainMenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawMainMenuItem);
}

void Menu_DrawActionSubmenuItem(int arg0);

void Menu_DrawActionSubmenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawActionSubmenuItem);
}

void Menu_DrawBlendOptionItem(int arg0);

void Menu_DrawBlendColorOptionListUnk(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawBlendOptionItem);
}

void Menu_DrawNotificationItem(int index);

void Menu_DrawNotificationDialogContent(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawNotificationItem);
}

void Menu_DrawInvPanelItemStub(int index);

void Menu_DrawItemListInvPanel(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawInvPanelItemStub);
}

void Menu_DrawStatAllocationItem(int arg0);
extern int g_StatBaseTable[];

void Menu_DrawStatAllocationList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawStatAllocationItem);
}

int Menu_GetStatBaseValue(int index) {
    return g_StatBaseTable[index];
}

void Menu_DrawScreenModeItem(int arg0);

void Menu_DrawScreenModeList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawScreenModeItem);
}

void Menu_DrawParasiteAbilityItem(int arg0);

void Menu_DrawParasiteAbilityList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawParasiteAbilityItem);
}

void Menu_DrawEmptyList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, 0);
}

void Menu_DrawItemIconList(MenuWidgetNode *node) {
    MenuWidgetNode *entry;
    int index;

    entry = MenuWidget_FindByModeAndSelectedBase(2, 0x17);
    if (entry != 0) {
        index = entry->cursor_y + 0x73;
    } else {
        index = 0x75;
    }

    g_MenuItemIconGlyphTable = Str_LookupTable4(index);
    MenuWidget_DrawList(node, Menu_DrawItemIcon);
}

void Menu_DrawMemCardSlot1List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot1Item);
    MenuWidget_GetCurrentNode();
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}

void Menu_DrawMemCardSlot2List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot2Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
    Draw_OffsetCursor(0, 0x10);
    Draw_AllocSprite(0x68);
}

void Menu_DrawMemCardSlot3List(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    MenuWidget_DrawList(node, Menu_DrawSaveSlot3Item);
    Draw_SetTextDimmed(1);
    Draw_AllocSprite(0x68);
}

void Menu_DrawSoundEffectList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawSoundEffectItem);
}

void Menu_DrawEquipItemDetailPanel(MenuWidgetNode *node) {
    g_MenuActiveWidget = node;
    g_MenuActionItemData = g_MenuSelectedItemData;
    MenuWidget_DrawList(node, Menu_DrawActionCodeItem);
}

void Menu_RebuildSelectableMask(void) {
    Inv_RebuildSelectableMask();
}

void Menu_DrawEquipSortToggleRow(int arg0) {
    short *p;

    Draw_OffsetCursor(0, 2);
    Draw_AllocSprite(D_800922D0[arg0]);
    Draw_OffsetCursor(0x12, 4);
    Draw_AllocSprite(0x22);
    Draw_OffsetCursor(0x0C, -4);

    p = D_800922D0;
    if (arg0 == 0) {
        p++;
    }
    Draw_AllocSprite(*p);
}

void Menu_DrawEquipInfoOptionRow(int arg0) {
    if (g_InvItemUsableFlag != 0) {
        Draw_AllocSprite(arg0 + 0x7C);
    } else {
        Draw_AllocSprite(arg0 + 0x7F);
    }
}
