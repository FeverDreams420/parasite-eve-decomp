#include "fx_common.h"
#include "fx_common_motion.h"

/* Pins and empty scheduling barriers preserve the retail argument setup,
 * interleaved vector reads, and snapshot stores; tracked in crutch debt. */
void func_80195994(s32 index, s32 first, s32 second, s32 source) {
    FxCommonMotionVec point;
    GteShortVector extra;
    s32 secondMask;
    s16 setupIndex;
    s32 firstMask;
    register s32 firstValue asm("$18");
    s32 secondValue;
    u8 *table = &D_801D0260;
    GteShortVector *extraPtr;
    volatile FxCommonShortVec3 *origin0 = &g_FxCommonMotionOrigin0;
    volatile FxCommonShortVec3 *target0 = &g_FxCommonMotionTarget0;
    volatile FxCommonShortVec3 *origin1 = &g_FxCommonMotionOrigin1;
    volatile FxCommonShortVec3 *target1 = &g_FxCommonMotionTarget1;
    register s32 sourceValue asm("$22");
    register void *lookupTable asm("$4");
    register s32 lookupCount asm("$5");
    s32 *base;
    register s32 bx asm("$9");
    s32 by;
    s32 bz;
    s32 ax;
    s32 ay;
    s32 az;
    s32 px;
    s32 py;
    s32 pz;
    s32 qx;
    s32 qy;
    s32 qz;

    lookupTable = table;
    firstValue = first;
    lookupCount = 2;
    sourceValue = source;
    /* The tied operand copies second unchanged after the other call arguments. */
    asm("" : "=r"(secondValue) : "0"(second), "r"(lookupTable), "r"(lookupCount), "r"(sourceValue));
    base = Gpu_GetTimTableEntry(lookupTable, lookupCount);
    setupIndex = (s16)index;
    extraPtr = &extra;
    func_8018F55C(sourceValue, g_FxCommonEffectSetup[setupIndex].motionIndices[0], base, &point, extraPtr);

    lookupTable = table;
    firstValue &= 0xff;
    px = point.x;
    bx = g_FxCommonMotionPosition0.x;
    asm volatile("" : : "r"(bx));
    py = point.y;
    by = g_FxCommonMotionPosition0.y;
    pz = point.z;
    bz = g_FxCommonMotionPosition0.z;
    D_8019C056 = firstValue;
    firstMask = 1 << firstValue;
    D_8019C09C = 0;
    D_8019C0A0 = 0;
    D_8019C0A4 = 0;
    g_FxCommonMotionRemaining0 = firstMask;
    D_8019C08C = px - bx;
    D_8019C090 = py - by;
    D_8019C094 = pz - bz;
    D_8019C0AC = bx;
    origin0->y = by;
    origin0->z = bz;
    D_8019C0B4 = px;
    target0->y = py;
    target0->z = pz;


    base = Gpu_GetTimTableEntry(lookupTable, 2);
    func_8018F55C(sourceValue, g_FxCommonEffectSetup[setupIndex].motionIndices[1], base, &point, extraPtr);

    asm volatile("" : : "r"(secondValue));
    secondValue &= 0xff;
    qx = point.x;
    ax = g_FxCommonMotionPosition1.x;
    qy = point.y;
    asm volatile("" : : "r"(qy));
    ay = g_FxCommonMotionPosition1.y;
    asm volatile("" : : "r"(ay));
    qz = point.z;
    asm volatile("" : : "r"(qz));
    az = g_FxCommonMotionPosition1.z;
    asm volatile("" : : "r"(az));
    secondMask = 1 << secondValue;
    D_8019C06C = 0;
    D_8019C070 = 0;
    D_8019C074 = 0;
    D_8019C052 = secondValue;
    g_FxCommonMotionRemaining1 = secondMask;
    D_8019C05C = qx - ax;
    D_8019C060 = qy - ay;
    D_8019C064 = qz - az;
    D_8019C07C = ax;
    origin1->y = ay;
    origin1->z = az;
    D_8019C084 = qx;
    target1->y = qy;
    target1->z = qz;
}
