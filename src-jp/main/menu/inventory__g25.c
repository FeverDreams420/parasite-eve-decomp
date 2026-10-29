/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_record.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_MenuInventoryViewMode;
int g_MenuEquipSwapItem;
int g_InvSelectedItemIndex;
int g_InvSwapTargetIndex;
ItemDataRecord *g_MenuActionItemData;
int g_MenuEquipSwapSkipRestore;
int g_MenuItemContextFlag;

/* Weapon and armor list draws, the inventory category step and the battle
 * command list draw. */

void Menu_DrawBattleCommandItem(int index);

#define NULL ((void *)0)
void Menu_DrawSkillSelectionList(int arg0);

void Menu_DrawSkillItem(int arg0);
#undef NULL

#define NULL ((void *)0)

/* Equipment swap: picking the Aya inventory item to exchange records the
 * swap source; returning from the equip screen reopens the inventory. */

int g_MenuEquipSwapSource;
int g_MenuEquipSwapSkipRestore;
#undef NULL

/* Item use/discard panel: creation, its input handler and the item name
 * header. Defines the selected item record and the discard flag. */

#define NULL ((void *)0)
ItemDataRecord *g_MenuSelectedItemData;
void Menu_DrawEquipStatsPanel(void);
int Menu_HandleItemInput(int arg0, int arg1);
void Menu_DrawEquipItemName(void);
void Menu_DrawEquipItemIcon(void);
M2C_UNK Menu_DrawSoundEffectList();
M2C_UNK Menu_DrawEquipItemDetailPanel();
int g_MenuItemDiscardMode;
void Draw_PrintRawText(u8 *arg0);

int g_MenuSelectionLocked;
int g_MenuActionSubmenuOpen;
int g_MenuEquipMode;
int g_MenuLayoutLocked;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_MenuItemSelectFlag;
int g_MenuInputPausePending;
int g_MenuMemCardConfirmPending;

/* List renderers for the item, selectable equip-slot and context-action
 * lists, the action-option and equip-slot adapters, and the usable-item and
 * skill action-code lists. */

MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuActiveItemList;
int g_InvItemUsableFlag;

void Menu_DrawItemSlot(int index);
void Menu_DrawSelectableEquipSlotRow(int index);
void Menu_ContextPick(int index);
void Menu_DrawActionOptionRow(int index);
void Menu_SoundTestSelect(int index);
void Menu_DrawActionCodeItem(int index);
void Draw_SetTextDimmed(int value);
void Battle_DrawActiveStatus(void);

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
void Menu_HandleInput(int index);

/* Item-slot availability, modifier, armor and memory-card port list draw
 * callbacks, and the port and save-slot selectability filters. */

MenuWidgetNode *g_MenuActiveWidget;
int g_McDialogMode;

void Draw_AllocColorQuad(int arg0, int arg1);
void Menu_DrawModNameItem(int index);
void Menu_DrawArmorListItem(int arg0);
void Menu_DrawArmorEquipItem(int arg0);
void Menu_DrawMemCardPortItem(int arg0);



int Menu_IsMemCardSlotCompatible(int arg0) {
    MemCardSaveSlot *ptr;
    int result;

    if (g_McDialogMode == 0) {
        ptr = MemCard_GetSlot(g_MenuActiveWidget->selected_base - 0x25, arg0);
        result = ptr->state == MEMCARD_SLOT_OCCUPIED && ptr->titleStyleFlag == Save_GetTitleStyleFlag();
    } else {
        result = 1;
    }

    return result;
}