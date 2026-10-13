#ifndef PE1_PSYQ_PAD_MAIN_H
#define PE1_PSYQ_PAD_MAIN_H

#include "pe1/card_obj.h"
#include "pe1/memcard_state.h"

/* LIBPAD/PADMAIN.OBJ. Legacy MemCard/Spu names are preserved for callers. */
extern int g_MemCardServiceReady, D_8009B768;
extern char D_800A5AB0[];
extern MemCardInterruptRegisters *D_8009B784;
extern MemCardSioRegisters *D_8009B788;
/* Per-port idle counters that PadEnableCom and the VSync timer age. */
extern int D_800A5AC0[2];
extern int (*D_8009B7A8[])(void);
extern int (*D_8009B724)(int);
extern u32 g_TimerTimeoutLimit, g_TimerTimeoutStart;
void setRC2wait(int);
int MemCard_WaitReadyForTransfer(void);
int chkRC2wait(void);
void _padStartCom(void);
void MemCard_RunCommandStep(CardObj *port);
int _padSioRW(CardObj *, int);

/* Pad interrupt queue entry removed by StopPAD. */
extern int D_800A34B0;

#endif
