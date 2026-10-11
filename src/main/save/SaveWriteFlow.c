/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "pe1/menu_confirm_callback.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_widget.h"

/* Save-slot write and read starts and the two confirmation handlers that
 * launch the card write behind the progress widget or report the error. */

int g_SaveSelectedSlot;
int g_SaveSlotCellIndex;
#include "SaveWriteFlow_externs.h"

void Save_StartWriteSlot(int arg0, int arg1);
int Save_StartReadSlot(int port, int slot);
void MemCard_SetDelayedCallback(void (*callback)(void));
void MemCard_DrawProgress(void);
int Menu_MemCardProgressInputHandler(void);
void MemCard_MarkActivePortState13(void);
void MemCard_ClearActivePrompt(void);
int MemCard_GetActivePort(void);

void Save_StartSelectedWriteSlot(void) {
    Save_StartWriteSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}

void Save_StartSelectedReadSlot(void) {
    Save_StartReadSlot(g_SaveSelectedSlot, g_SaveSlotCellIndex);
}

void Menu_TriggerSaveWrite(MenuWidgetNode *arg0, int arg1) {
    if (arg1 != 0) {
        MenuWidget_NavScrollTo(0x1F);
        Menu_CreateMemCardProgressWidget(0x45);
        MemCard_SetDelayedCallback(Save_StartSelectedWriteSlot);
    }
}

void Menu_HandleMemCardWriteOrError(MenuWidgetNode *arg0, int arg1) {
    MenuWidgetNode *entry;

    if (arg1 != 0) {
        entry = MenuWidget_CreateSimpleNode(0x27, MenuWidget_GetCurrentNode(), 0, 1);
        entry->draw = MemCard_DrawProgress;
        entry->update = (void (*)())Menu_MemCardProgressInputHandler;
        MenuWidget_SetCurrentNode(entry);
        g_MemCardProgressPrimList = 0x40;
        MemCard_SetDelayedCallback(MemCard_MarkActivePortState13);
    } else {
        Menu_CreateTwoLineDialog(MemCard_GetActivePort() + 0x47, 0x4B);
        g_MenuDeferredCallback = MemCard_ClearActivePrompt;
    }
}
