#include "pe1/room_m404.h"

int func_80193368(int mode, void *unused, char *state) {
    RoomTemplate8 template = D_8018F208;
    s16 position[4];
    s16 target[4];
    char *pool;
    RoomM404Particle *child;
    switch (mode) {
    case 0:
        {
            int handle = func_800D3FD8();
            func_800D3F64(0x5b9, handle);
            if (D_800E2368->active) {
                RoomM404Node **slot = (RoomM404Node **)D_800F32D0->pool;
                if (slot && *slot) {
                    u8 *flag = (*slot)->state;
                    if (*flag == 1) *flag = 2;
                }
            }
            pool = D_800F33E0->pool;
            return func_800CE560(pool, 20, 55,
                                 (FieldAnimCallbackListCallback)func_80192540);
        }
    case 1:
    {
        pool = D_800F32D0->pool;
        func_800CE8F0(pool, 3, &template, position);
        pool = D_800F32D0->pool;
        func_800CE9D4((struct RoomFxTransformOwner *)pool, 0,
                      (GteShortVector *)target);
        if (D_800E27EC == 1) {
            pool = D_800F33E0->pool;
            child = func_800CE610(pool);
            if (child) {
                child->x = position[0];
                child->y = position[1];
                child->z = position[2];
                func_800CFB7C((GteShortVector *)target, *(s16 *)state, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
        }
        if (D_800E27EC < 2) break;
        return 2;
    }
    case 2:
    {
        int idx = D_800E11EA;
        int palette;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E2850[idx];
        PE1_COMPILER_MEMORY_BARRIER();
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}
