#include "common.h"
#include "pe1/scene_e08_ring_effect.h"
#include "pe1/room_fx.h"
#include "pe1/field_flash_burst.h"
#include "pe1/gte_types.h"
#include "pe1/field_ring_geometry.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef struct {
    s16 m[3][3];
    s16 pad;
    s32 t[3];
} Matrix;

typedef struct {
    s32 x;
    s32 y;
    s32 z;
    s32 w;
} Vec4;



extern char D_801995F8[];
extern s32 D_8019956C, D_8019957C;

void func_80192544(void *unused, RoomFxControl *time, SceneE08RingEffectState *record) {
    Matrix matrix;
    Vec4 scratchScale, firstScale, secondScale;

    func_800C3238(3);
    record->rotation[0] = 0;
    record->rotation[1] = 0;
    record->rotation[2] = 0;
    RotMatrixYXZ(record->rotation, &matrix);
    memset(&firstScale, 0, 0x10);
    firstScale.x = record->firstScale;
    firstScale.y = record->firstScale;
    firstScale.z = record->firstScale;
    scratchScale = firstScale;
    ScaleMatrix(&matrix, &scratchScale);
    matrix.t[0] = D_8019956C;
    matrix.t[1] = 0;
    matrix.t[2] = D_8019957C;
    func_800C4FC4(D_801995F8, &matrix, 1);
    if (time->frame >= 0x3D) {
        func_800C3238(2);
        record->rotation[0] = 0x400;
        record->rotation[1] = 0x400;
        record->rotation[2] = 0;
        RotMatrixYXZ(record->rotation, &matrix);
        memset(&secondScale, 0, 0x10);
        secondScale.x = record->secondScale;
        secondScale.y = record->secondScale;
        secondScale.z = record->secondScale;
        firstScale = secondScale;
        ScaleMatrix(&matrix, &firstScale);
        matrix.t[0] = D_8019956C;
        matrix.t[1] = 0;
        matrix.t[2] = D_8019957C;
        func_800C4FC4(D_801995F8 + 0x18, &matrix, 0);
    }
}
