/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/memcard_save_state.h"
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_flags.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_rows.h"
#include "pe1/Menu_MemCardProgress_externs.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/save.h"
#include "pe1/text.h"
#include "include_asm.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_SaveSelectedSlotState;
int g_InvItemUsableFlag;
int g_InvSelectedItemIndex;
ItemDataRecord *g_MenuActionItemData;
int g_MenuActionTextBase;
int g_MenuActiveItemList;
MenuConfirmCallback g_MenuConfirmCallback;
void (*g_MenuDeferredCallback)(void);
int g_MenuEquipMode;
int g_MenuItemContextFlag;
unsigned char *g_MenuItemIconGlyphTable;
int g_MenuItemPrimListHandles[2];
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
ItemDataRecord *g_MenuRenameTargetRecord;
int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;

/* Parasite-spell info and context help panels, notification and two-line
 * dialogs, the deferred confirm callback they run, and the memory-card port
 * dialog with its error prompt. */

#define NULL ((void *)0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))

s32 Menu_MemCardPortSelectHandler(void *node, s32 flags);

int g_McDialogMode;
int g_MenuMemCardConfirmPending;
/* Parasite spell named by the info panel. */
int g_MenuSpellInfoId;

void Menu_DrawParasiteSpellInfoPanel(void);
#undef NULL

#define NULL ((void *)0)

#undef NULL


#define NULL ((void *)0)

#undef NULL

/* Memory-card progress widget: creation, teardown, block-meter draw and the
 * write-confirmation prompt it raises when the card operation finishes. */

int g_MemCardProgressPrimList;

void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void Draw_FlushPrimList(int arg0);
int MemCard_GetActiveProgressBlocks(void);
void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0);
int MemCard_GetActivePort(void);
void Menu_CreateNotificationDialog(int arg0, int arg1);
void MemCard_ClearActivePrompt(void);

#define NULL ((void *)0)

s32 Inv_GetPackedListCursor(void);
void Menu_StepInventoryRoot(s32 arg0, s32 arg1, s32 arg2);
void Menu_OpenRenameScreen(s32 arg0);
#undef NULL

#define NULL ((void *)0)

void Menu_DrawSaveMetadataPreview(void);

#define MENU_INPUT_BACK 0x800
#define MENU_INPUT_UP 0x1000
#define MENU_INPUT_RIGHT 0x2000
#define MENU_INPUT_DOWN 0x4000
#define MENU_INPUT_CONFIRM 0x10000

#define MODE_NAME_WINDOW_0 0x17
#define MODE_NAME_WINDOW_1 0x18
#define MODE_NAME_WINDOW_2 0x19
#define MODE_NAME_GRID 0x11




/* JP-only panel polygon for the spell info window (D_80091E14). */
extern u8 g_MenuSpellInfoPanelPoints[];

void Menu_CreateParasiteSpellInfoPanel(int arg0) {
    MenuWidgetNode *widget;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x1E) == 0) {
        widget = MenuWidget_CreateSimpleNode(0x1E, 0, 0, 0);
        widget->draw = Menu_DrawParasiteSpellInfoPanel;
        widget->appearance.gradientPoints = g_MenuSpellInfoPanelPoints;
    }

    g_MenuSpellInfoId = arg0;
}

void Menu_DrawParasiteSpellInfoPanel(void) {
    Draw_OffsetCursor(2, 4);
    Draw_PrintCenteredText(Text_LookupTable4(0x26));
    Draw_OffsetCursor(0, 0x10);
    Draw_PrintCenteredText(Text_LookupTable8(g_MenuSpellInfoId + 0xEB));
    Draw_OffsetCursor(2, 0x14);
    Draw_PrintTextWrapped(Text_LookupTableC(g_MenuSpellInfoId + 0xEB), 0xAA);
}