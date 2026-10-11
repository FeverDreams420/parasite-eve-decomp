/* MASPSX_FLAGS: -G8 --use-comm-section */

#include "pe1/save_tail_state.h"

int g_MenuInputPausePending;

void MenuInput_SetPollingPaused(int arg0);

void Evt_DeferredExec(void) {
    if (g_MenuInputPausePending != 0) {
        if ((g_DiscChangeFlags & 3) == 0) {
            MenuInput_SetPollingPaused(0);
            g_MenuInputPausePending = 0;
        }
    }
}
