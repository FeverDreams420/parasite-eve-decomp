/* GCC_VERSION: 2.8.1 */

#include "pe1/mdec_diag.h"
#include "pe1/psyq_callbacks.h"
#include "DecDCTReset_externs.h"
void DecDCTReset(int mode) {
    if (mode == 0)
        ResetCallback();
    MdecDiag_Reset(mode);
}
