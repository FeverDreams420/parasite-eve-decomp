/* PSY-Q LIBAPI PAD, part 4 of 6: StartPAD, StopPAD. */
long StartPAD2(void);
void EnablePAD(void);

#include "pe1/pad_internal.h"
#include "pe1/psyq_bios.h"

#include "pad_4_externs.h"
int RemovePatchPad(void);

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
