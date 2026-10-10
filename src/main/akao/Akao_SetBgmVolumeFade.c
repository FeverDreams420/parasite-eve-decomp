/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/akao_counters.h"

extern void * g_AkaoBgmHandle[];
int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4);

void Akao_SetBgmVolumeFade(void) {
    void * volatile *slot;
    int value;

    slot = g_AkaoBgmHandle;
    if (*slot != 0) {
        value = 0x7F;
        value = Akao_SendTableCommand(*slot, 0x450, 0x100, 0x80, value);
    } else {
        value = 0;
    }
    g_AkaoPendingBgmVolumeFade = value;
}
