/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "fx_common.h"

int func_80191E30(int arg0)
{
    int size = 0x300;
    void *resource;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;
    int result;

    resource = Gpu_GetTimTableEntry(&D_801D0260, 3);
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &D_800BCFA4.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = D_800BCFA8.distance;
    D_800BCFA8.distance = &size;
    result = Akao_SendTableCommand(resource, arg0, 0, 0x80, 1);
    *matrixSlot = savedMatrix;
    D_800BCFA8.distance = savedOutput;
    PopMatrix();
    return result;
}

void func_80191EFC(void *object, volatile FxCommonVec3 *input)
{
    struct FxCommonApplyScratch {
        int size;
        int pad14;
        FxCommonShortVec3 vector;
        s16 pad1E;
        int valueA;
        int valueB;
    } scratch;
    GteMatrix **matrixSlot;
    GteMatrix *savedMatrix;
    s32 *savedOutput;

    scratch.size = 0x300;
    PushMatrix();
    SetTransMatrix(&D_8019CC30);
    SetRotMatrix(&D_8019CC30);
    matrixSlot = &D_800BCFA4.value;
    savedMatrix = *matrixSlot;
    *matrixSlot = &D_8019CC30;
    savedOutput = D_800BCFA8.distance;
    D_800BCFA8.distance = &scratch.size;
    scratch.vector.x = input->x;
    scratch.vector.y = input->y;
    scratch.vector.z = input->z;
    func_8006DFA8(&scratch.vector, &scratch.valueA, &scratch.valueB);
    func_800868F0(object, 0, scratch.valueB);
    func_80086A28(object, 0, scratch.valueA);
    *matrixSlot = savedMatrix;
    D_800BCFA8.distance = savedOutput;
    PopMatrix();
}
