#include "common.h"
#include "pe1/mdec_diag.h"

#include "MdecDiag_WaitMdec.inc"

MDECDIAG_WAIT_MDEC(func_8010C308, D_8010DB50, 0x20000000, 0x42E8)

#undef MDECDIAG_WAIT_MDEC

#include "MdecDiag_WaitMdec.inc"

MDECDIAG_WAIT_MDEC(func_8010C39C, D_8010DB30, 0x1000000, 0x42D8)

#undef MDECDIAG_WAIT_MDEC
