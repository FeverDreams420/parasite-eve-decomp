/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_memcard_display.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_MemCardProgressPrimList;
void (*g_MenuDeferredCallback)(void);
ItemDataRecord *g_MenuSelectedItemData;
int g_MenuSelectionLocked;

MenuWidgetNode *g_MenuActiveWidget;

void Menu_DrawSaveSlotList(int arg0) {
    g_MenuActiveWidget = (MenuWidgetNode *)arg0;
    MenuWidget_DrawList((MenuWidgetNode *)arg0, Menu_DrawSaveSlotEntry);
    Draw_BlendColor(g_SavedDrawBlendColor[0]);
    Menu_SetSaveSlotBlendBase(0);
}

/* Contiguous list-draw adapters: each binds a widget list to its row drawer,
 * plus the icon, save-slot, sound-effect and equip-detail lists and the
 * equip sort-toggle and info-option rows. */

MenuWidgetNode *g_MenuActiveWidget;
ItemDataRecord *g_MenuActionItemData;
int g_InvItemUsableFlag;

void Menu_DrawItemIcon(int index);
void Menu_DrawSaveSlot1Item(int index);
void Menu_DrawSaveSlot2Item(int index);
void Menu_DrawSaveSlot3Item(int index);
void Menu_DrawSoundEffectItem(int index);
void Menu_DrawActionCodeItem(int index);

void Menu_DrawMainMenuItem(int arg0);

void Menu_DrawMainMenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawMainMenuItem);
}

void Menu_DrawActionSubmenuItem(int arg0);

void Menu_DrawActionSubmenuList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawActionSubmenuItem);
}

void Menu_DrawCursorModeItem(int arg0);

void Menu_DrawCursorModeList(MenuWidgetNode *node) {
    MenuWidget_DrawList(node, Menu_DrawCursorModeItem);
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

    g_MenuItemIconGlyphTable = Text_LookupTable4(index);
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

int g_MenuEquipMode;
int g_MenuItemRenameMode;

int Inv_GetPackedListItem(int arg0);
void Menu_CloseContextHelpPanel(void);
int Inv_GetActiveListItem(int arg0);

void Draw_SetColor(int arg0);

void MenuWidget_DrawCenteredTableText(int arg0);

/* Save-slot write and read starts and the two confirmation handlers that
 * launch the card write behind the progress widget or report the error. */

int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;

void Save_StartWriteSlot(int arg0, int arg1);
int Save_StartReadSlot(int port, int slot);
void MemCard_SetDelayedCallback(void (*callback)(void));
void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void MemCard_MarkActivePortState13(void);
void MemCard_ClearActivePrompt(void);
int MemCard_GetActivePort(void);

/* Row drawers for the equip-slot, inventory, armor, battle-command and
 * skill lists. */

MenuWidgetNode *g_MenuActiveWidget;
int g_MenuEquipMode;

int g_MenuItemContextFlag;
MenuWidgetNode *g_MenuActiveWidget;

void Draw_SetTextDimmed(int value);

int g_MenuItemActionContext;
int g_MenuActiveItemSlot;
int g_MenuItemActionDisabled;
int g_MenuEquipMode;

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
void Draw_PrintNumberWidth2Unk(int);
void Menu_UpdateStatBarAnimation(int);
void Draw_AllocTexturedQuad(int);
void Inv_ShowItemDescriptionPanel(void);