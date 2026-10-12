/* Psy-Q LIBGPU SYS.OBJ part, the end of the object: the DMA callback restore
 * and GPU_memset. GCC 2.7.2 without markers; the other SYS.OBJ functions are
 * in neighbouring units (see docs/CODE_ORGANIZATION.md, "PsyQ objects split
 * over several units"). */
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_gpu.h"

void Gpu_RestoreDmaCallback(void) {
    DMACallback(2, (PsyqInterruptHandler)Gpu_SetDisplayBuffer);
}

void GPU_memset(unsigned char *dst, int value, int count)
{
    int remaining;

    remaining = count - 1;
    if (count != 0) {
        do {
            *dst++ = value;
            remaining--;
        } while (remaining != -1);
    }
}

static unsigned int GPU_memset_alignment[] __attribute__((section(".text"))) = { 0x00000000, 0x00000000 };
