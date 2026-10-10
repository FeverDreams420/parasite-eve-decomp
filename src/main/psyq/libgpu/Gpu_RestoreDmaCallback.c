#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_gpu.h"

void Gpu_RestoreDmaCallback(void) {
    DMACallback(2, (PsyqInterruptHandler)Gpu_SetDisplayBuffer);
}
