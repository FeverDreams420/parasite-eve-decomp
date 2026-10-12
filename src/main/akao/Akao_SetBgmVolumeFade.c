/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */

#include "pe1/akao_counters.h"
#include "pe1/akao.h"
#include "pe1/akao_send.h"

void Akao_SetBgmVolumeFade(void) {
    int value;

    if (g_AkaoBgmHandle[0] != 0) {
        value = Akao_SendTableCommand(g_AkaoBgmHandle[0], 0x450, 0x100, 0x80, 0x7F);
    } else {
        value = 0;
    }
    g_AkaoPendingBgmVolumeFade = value;
}
