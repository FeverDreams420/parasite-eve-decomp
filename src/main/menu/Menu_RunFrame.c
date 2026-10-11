/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/menu_state.h"
#include "pe1/menu_background.h"
#include "pe1/menu_inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_widget.h"
u32 g_MenuErrorSoundPending;

/* Queued by frontend commands, consumed by either frame entry point. */
void Menu_RequestErrorSound(void) {
    g_MenuErrorSoundPending = 1;
}

s32 Menu_RunFrameWithArg(s32 arg0) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget[0] = arg0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Evt_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Window_SetBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

s32 Menu_RunFrame(void) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget[0] = 0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Evt_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Window_SetBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}
