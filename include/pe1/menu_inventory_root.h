#ifndef PE1_MENU_INVENTORY_ROOT_H
#define PE1_MENU_INVENTORY_ROOT_H

/* Callbacks and state wired up by Menu_StepInventoryRoot. */

#include "pe1/aya.h"
#include "pe1/inventory.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_item_list_input.h"

int Menu_InventoryPageNavHandler(MenuWidgetNode *root, unsigned int flags);
void Menu_DrawBattleCommandList();
void Menu_DrawEquipSelectionList(MenuWidgetNode *node);
void Inv_BuildFilteredPackedListExcluding(int mask, int excluded);
void Menu_DrawBonusPointSlotValue(void);
void Menu_DrawParasiteAbilityList();
void Menu_InitStateTables(void);
void Menu_DrawItemDetailPanel(MenuWidgetNode *panel);
int Menu_BonusPointCancelHandler(void *node, int flags);
void Menu_DrawEmptyList();
void Menu_StepEquipSlotSelect2(void);
void Menu_SetEquipPanelsCursorY(void);
void Menu_AlignEquipPanels(void);

/* Whether the active list override was selected when the root opened. */
extern int g_MenuSourceItemList;
/* Seven per-stat words the bonus-point screen clears next to
 * g_BonusPointStatDeltas and g_BonusPointStatMultipliers; nothing reads them. */
extern int g_BonusPointStatClearedWordsWriteOnly[7];
extern int g_AyaBonusPoints[];

#endif
