#include "common.h"
#include "pe1/field_anim_callback_list.h"

#include "room_m273_effects.h"
#include "pe1/field_anim_pool.h"

extern u8 D_8019AE68;

void func_80194B5C(void);

s32 func_8019A068(s32 state) {
    char *obj;
    RoomM273PlayerActorView *root;
    RoomM273ThresholdTransform *source;
    u8 status;

    if (state != 1) {
        if (state >= 2) {
            return 0;
        }
        if (state != 0) {
            return 0;
        }
        D_8019AE68 = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 8, 5,
                             (FieldAnimCallbackListCallback)func_80194B5C);
    }
    if (D_8019AE68 != 0) {
        return 2;
    }

    root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
    status = root->kind;
    if (status != 0x10 && status != 8) {
        D_8019AE68 = 1;
        return 2;
    }
    if ((D_800E27EC & 3) != 0) {
        return 0;
    }

    obj = func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
    if (obj != 0) {
        root = ANIM_OWNER_AS(RoomM273EffectStateContext)->state.player;
        source = (RoomM273ThresholdTransform *)root->tail.object;
        *(u16 *)(obj + 0) = source->x;
        *(u16 *)(obj + 2) = source->y;
        *(u16 *)(obj + 4) = source->z;
    }
    return 0;
}
