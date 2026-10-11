#ifndef SCENE_E08_H
#define SCENE_E08_H

#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "pe1/scene_archive.h"
#include "pe1/akao_send.h"

typedef struct {
    RoomFxEmitterParams emitters[2];
    char sourceData[2][0x100];
    s16 field_230[2];
    char pad_234[0x20];
    s16 field_254[4];
    s16 field_25C[4];
    s16 field_264;
} Ovl178FadeState;

typedef struct {
    char pad_00[0x18];
    s32 x;
    s32 y;
    s32 z;
} Ovl178Position;



typedef struct {
    u8 pad_00[8];
    s16 field_08;
    s16 field_0A;
    s16 offsets[12];
    union {
        s16 variation[3];
        u8 flags[10];
    } tail;
} Ovl178RandomizedEffect;

s32 *func_800C2B10(int index);
s32 *func_800C2B28(int index);
extern s16 D_80199590[];
extern u16 D_80199658[];
extern s32 D_8019956C;
extern s32 D_8019957C;
void func_800C4E50(void *item);

#endif
