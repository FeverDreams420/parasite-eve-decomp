/* ASSEMBLER: GNU */

#include "common.h"
#include "pe1/mdec_diag.h"
#include "pe1/psyq_callbacks.h"

#include "decdct_externs.h"

/*
 * The LIBPRESS DCT object as linked into the overlay: the DecDCT entry
 * points (DecDCTReset .. DecDCToutCallback), then the MDEC register layer
 * they sit on (reset, input and output DMA). Every function compiles with
 * GCC 2.7.2 and plain GNU as. The rest of the object follows in its own
 * units: the two busy waits (mdec_wait.c, whose global register variable
 * must precede every function of its unit) and the status read with the
 * timeout handler (mdec_status.c, which needs $a0 back).
 */

typedef unsigned int size_t;

typedef struct DecDCTEnv {
    unsigned char iqY[64];
    unsigned char iqC[64];
    short dct[64];
} DecDCTEnv;

void DecDCTReset(int mode) {
    if (mode == 0)
        ResetCallback();
    MdecDiag_Reset(mode);
}

static inline void CopyWords(long *dst, long *src, size_t count) {
    while (count--) {
        *dst++ = *src++;
    }
}

s32 *DecDCTGetEnv(DecDCTEnv *env) {
    CopyWords((long *)&env->iqY, (long *)&D_8010DA0C[1], 16);
    CopyWords((long *)&env->iqC, (long *)&D_8010DA0C[17], 16);
    CopyWords((long *)&env->dct, (long *)&D_8010DA90[1], 32);
    return (s32 *)env;
}

s32 *DecDCTPutEnv(s32 *source)
{
    register s32 *read = source;
    register s32 *write = D_8010DA10;
    int index;

    for (index = 15; index != -1; index--)
        *write++ = *read++;

    write = D_8010DA50;
    read = source + 16;
    for (index = 15; index != -1; index--)
        *write++ = *read++;

    MdecDiag_SendInput(D_8010DA0C, 0x20);
    MdecDiag_SendInput(D_8010DA90, 0x20);
    return source;
}

unsigned short func_8010BF94(unsigned short *value) {
    return *value;
}

void func_8010BFA0(s32 *arg0, s32 arg1) {
    if (arg1 & 1) {
        *arg0 &= ~0x08000000;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= ~0x02000000;
    }

    MdecDiag_SendInput(arg0, *(u16 *)arg0);
}

void func_8010C01C(s32 *buffer, u32 size) {
    MdecDiag_ReceiveOutput(buffer, size);
}

s32 func_8010C03C(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C308();
    } else {
        result = ((u32)func_8010C430() >> 29) & 1;
    }
    return result;
}

s32 func_8010C078(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C39C();
    } else {
        result = ((u32)func_8010C430() >> 24) & 1;
    }
    return result;
}

void func_80073CF4(int mode, void *value);

void func_8010C0B4(void *value) {
    func_80073CF4(0, value);
}

void func_80073CF4(int mode, void *value);

void func_8010C0D8(void *value) {
    func_80073CF4(1, value);
}

void MdecDiag_Reset(int mode)
{
    switch (mode) {
    case 0:
        *D_8010DB50 = 0x80000000;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        *D_8010DB50 = 0x60000000;
        MdecDiag_SendInput(D_8010DA0C, 0x20);
        MdecDiag_SendInput(D_8010DA90, 0x20);
        break;
    case 1:
        *D_8010DB50 = 0x80000000;
        *D_8010DB24 = 0;
        *D_8010DB30 = 0;
        (void)*D_8010DB30;
        *D_8010DB50 = 0x60000000;
        break;
    default:
        func_80071A74(D_8010BCFC, mode);
    }
}

void MdecDiag_SendInput(s32 *data, u32 size)
{
    func_8010C308();
    *D_8010DB54 |= 0x88;
    *D_8010DB1C = data + 1;
    *D_8010DB20 = ((size >> 5) << 16) | 0x20;
    *D_8010DB4C = *data;
    *D_8010DB24 = 0x1000201;
}

void MdecDiag_ReceiveOutput(s32 *data, u32 size)
{
    func_8010C39C();
    *D_8010DB54 |= 0x88;
    *D_8010DB30 = 0;
    *D_8010DB28 = data;
    *D_8010DB2C = ((size >> 5) << 16) | 0x20;
    *D_8010DB30 = 0x1000200;
}

