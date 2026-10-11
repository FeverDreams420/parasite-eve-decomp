#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/fixed_point.h"

typedef struct RoomTransformScaleOwner {
    char pad0[8];
    FieldActor *actor;
    void (*callback)(struct RoomTransformScaleOwner *);
    char pad10[0x22];
    s16 timer;
    s16 duration;
} RoomTransformScaleOwner;

extern s32 Scratch_1F800008 asm("0x1F800008");
extern s32 Scratch_1F80000C asm("0x1F80000C");
extern s32 Scratch_1F800010 asm("0x1F800010");


int ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME(RoomTransformScaleOwner *owner) {
    FieldActor *actor;
    RenderMatrix *matrix;
    s32 *scale;
    s32 scaleValue;
    s32 curve;

    actor = owner->actor;
    scale = (s32 *)0x1F800008;
    owner->callback(owner);

    matrix = &actor->render_object.model_matrix;
    matrix->translation[0] = Pe1Fixed_Integer(&actor->pos_x);
    matrix->translation[1] = Pe1Fixed_Integer(&actor->pos_y);
    matrix->translation[2] = Pe1Fixed_Integer(&actor->pos_z);
    RotMatrixYXZ((void *)&actor->rot_x, (void *)matrix);

    scaleValue = actor->move_speed;
    Scratch_1F800010 = scaleValue;
    Scratch_1F80000C = scaleValue;
    Scratch_1F800008 = scaleValue;
    ScaleMatrix((void *)matrix, (void *)0x1F800008);

    if (owner->timer != 0) {
        curve = (*(s32 *)D_800966EC[(((owner->duration - owner->timer) << 11) /
                                     owner->duration) & 0xFFF].half << 16) >> 17;
        Scratch_1F800010 = 0x1000 + curve;
        scale[0] = 0x1000 + curve;
        Scratch_1F80000C = 0x1000 - curve;
        ScaleMatrix((void *)matrix, (void *)scale);
        owner->timer--;
    }

    return 0;
}

#undef ROOMLIB_UPDATE_TRANSFORM_SCALE_NAME
