#include "pe1/field_anim_pool.h"
#include "pe1/asset_find.h"
#include "pe1/field_actor.h"

extern unsigned char D_8019AE9A;


int func_80194D54(int mode) {
    FieldActor *obj;
    unsigned char *state;
    unsigned char obj_mode;
    unsigned short frame;
    unsigned short timer;
    RenderMatrix *data;

    if (mode != 1) {
        if (mode >= 2) {
            return 0;
        }
        if (mode != 0) {
            return 0;
        }
        D_8019AE9A = 0;
        goto ret_zero;

    }
mode_one:
    obj = ANIM_OWNER_AS(FieldAnimObjectPrefix)->actor;
    state = obj->state->action_state;
    frame = obj->anim.parts.integer;
    /* High half of anim_prev, the previous whole frame. */
    timer = *(unsigned short *)((char *)obj + 0x1A);

    if (*state == 1) {
        *state = 2;
    }

    obj_mode = obj->mode;
    if (obj_mode != 9) {
        if (obj_mode == 0x10) {
            if ((unsigned int)(frame - 0x23) < 0x19) {
                D_8019AE9A = 1;
            }
        }
    } else if ((short)frame >= 0x21) {
        D_8019AE9A = 1;
    } else if ((short)frame > 0 && ((int)(timer << 16) <= 0)) {
        data = obj->render_object.matrices;
        Asset_Find08w(0x5CF, obj->state->progress,
                      (short)data[44].translation[0], (short)data[44].translation[1],
                      (short)data[44].translation[2]);
    }

    if (D_8019AE9A != 0) {
        return 1;
    }

ret_zero:
    return 0;

ret_one:
    return 1;
}
