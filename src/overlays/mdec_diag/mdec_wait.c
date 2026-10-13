/* ASSEMBLER: GNU */

#include "common.h"
#include "pe1/mdec_diag.h"

#include "mdec_wait.inc"

MDECDIAG_WAIT_MDEC(func_8010C308, D_8010DB50, 0x20000000, D_8010BD18)

#undef MDECDIAG_WAIT_MDEC

#include "mdec_wait.inc"

MDECDIAG_WAIT_MDEC(func_8010C39C, D_8010DB30, 0x1000000, D_8010BD28)

#undef MDECDIAG_WAIT_MDEC
