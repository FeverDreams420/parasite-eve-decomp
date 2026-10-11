/* GCC_VERSION: 2.8.1 */
#include "pe1/sys_reset.h"
#include "pe1/psyq_callbacks.h"
#include "DecDCTReset_externs.h"
void DecDCTReset(int mode) {
    if (mode == 0)
        ResetCallback();
    func_8010C0FC(mode);
}
