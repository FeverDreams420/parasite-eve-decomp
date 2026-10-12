/* ASSEMBLER: GNU */
/* PSY-Q LIBAPI PAD, part 1 of 3: SetInitPadFlag, ReadInitPadFlag, PAD_init,
 * InitPAD, StartPAD, StopPAD. */
/* LIBAPI PAD stays in three units: Sys_SetInterruptHandler (pad_5.c) only
 * matches with GCC 2.8.1 and -mno-split-addresses, the rest of the object
 * (pad_6.c) only with GCC 2.7.2. PAD_init and InitPAD instantiate
 * Pad_InitCommon.inc. */
#include "pe1/pad_internal.h"
#include "pe1/psyq_bios.h"

#include "pad_externs.h"

long StartPAD2(void);
void EnablePAD(void);
int RemovePatchPad(void);

void SetInitPadFlag(int value) {
    g_InitPadFlag = value;
}

int ReadInitPadFlag(void) {
    return g_InitPadFlag;
}

#define PAD_INIT_COMMON_NAME PAD_init
#define PAD_INIT_COMMON_CALL PAD_init2
#include "Pad_InitCommon.inc"

#define PAD_INIT_COMMON_NAME InitPAD
#define PAD_INIT_COMMON_CALL InitPAD2
#include "Pad_InitCommon.inc"

long StartPAD(void) {
    StartPAD2();
    ChangeClearPAD(0);
    EnablePAD();
    return 1;
}

void StopPAD(void) {
    DisablePAD();
    StopPAD2();
    RemovePatchPad();
    g_InitPadFlag = 0;
}
