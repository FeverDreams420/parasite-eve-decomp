/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "pe1/menu_flags.h"

void Menu_SetEquipSlotIndex(int index) {
    D_8009CE94 = index;
}
