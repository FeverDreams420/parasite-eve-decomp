/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* PSY-Q LIBAPI PAD, part 2 of 3: Sys_SetInterruptHandler. */
#include "common.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_pad_main.h"

#include "pad_2_externs.h"

void SysDeqIntRP(int index, void *queue);
void SysEnqIntRP(int index, void *queue);
void func_8007E0F8(void);
void func_8007E160(void);

int Sys_SetInterruptHandler(void) {
    void *queue;
    void (**handlers)(void);
    int result;
    EnterCriticalSection();
    handlers = D_800A34B4;
    queue = (char *)handlers - 4;
    handlers[0] = func_8007E0F8;
    handlers[1] = func_8007E160;
    D_800A34B0 = 0;
    D_800A34BC = 0;
    SysDeqIntRP(1, queue);
    SysEnqIntRP(1, queue);
    ExitCriticalSection();
    result = 1;
    PE1_COMPILER_LAUNDER(result);
    return result;
}
