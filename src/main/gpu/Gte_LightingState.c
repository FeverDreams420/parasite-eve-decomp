#include "common.h"
void Gte_SetBackColor(u16 *dst, int r, int g, int b) {
    register int sr asm("t4");
    register int sg asm("t5");
    register int sb asm("t6");

    sr = r << 4;
    sg = g << 4;
    sb = b << 4;
    asm volatile("ctc2 %0,$13" : : "r"(sr));
    asm volatile("ctc2 %0,$14" : : "r"(sg));
    asm volatile("ctc2 %0,$15" : : "r"(sb));

    dst[0] = 0;
    dst[1] = 0;
    dst[2] = 0;
    dst[3] = 0;
    dst[4] = 0;
    dst[5] = 0;
    dst[6] = 0;
    dst[7] = 0;
    dst[8] = 0;
}
#include "pe1/render_lighting.h"
void Gte_SetLightColor(RenderLightingState *lighting, int index, int r, int g, u16 b) {
    u8 *state = (u8 *)lighting;
    u8 *byte_slot;
    register u32 m0 asm("t4");
    register u32 m1 asm("t5");
    register u32 m2 asm("t6");

    if ((unsigned int)index < 3) {
        lighting->color.matrix.m[0][index] = r;
        lighting->color.matrix.m[1][index] = g;
        lighting->color.matrix.m[2][index] = b;
    }

    byte_slot = state + index * 4;
    byte_slot[0x40] = r;
    byte_slot[0x41] = g;
    byte_slot[0x42] = b;

    asm volatile("" : : : "memory");
    {
        u32 *matrix = lighting->color.words;

        asm volatile("" : "=r"(matrix) : "0"(matrix));
        m0 = matrix[0];
        m1 = matrix[1];
        asm volatile("ctc2 %0,$16" : : "r"(m0));
        asm volatile("ctc2 %0,$17" : : "r"(m1));
        m0 = matrix[2];
        m1 = matrix[3];
        m2 = matrix[4];
        asm volatile("ctc2 %0,$18" : : "r"(m0));
        asm volatile("ctc2 %0,$19" : : "r"(m1));
        asm volatile("ctc2 %0,$20" : : "r"(m2));
    }
}
