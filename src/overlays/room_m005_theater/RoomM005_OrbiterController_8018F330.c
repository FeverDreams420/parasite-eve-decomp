#include "common.h"
#include "pe1/room_m005.h"
#include "pe1/room_sound_slot.h"

int func_8018F330(int mode, RoomM005OrbiterState *state) {
    RoomM005Seed8 seed = D_8018EFF4;
    s16 target[4];
    char *pool;
    RoomM005OrbiterChild *child;
    RoomSoundSlot *soundSlot;
    int handle;
    int spread;
    int volume;

    switch (mode) {
    case 0:
        state->armed = 0;
        state->frame = 0;
        pool = ANIM_POOL_AS(RoomM005Channel)->pool;
        return func_800CE560(pool, 16, 24,
                             (FieldAnimCallbackListCallback)RoomM005_FxOrbiter_8018F018);
    case 1:
        if (D_800E27EC == 7) {
            soundSlot = (RoomSoundSlot *)D_800B0E64;
            if (soundSlot->channel != 0) {
                volume = 0x7F;
                handle = func_800D3FD8();
                Akao_SendTableCommand(soundSlot->channel, 0x5AB, handle, 0x80, volume);
                if (soundSlot->channel != 0) {
                    Akao_SendTableCommand(soundSlot->channel, 0x5AC, 0x80, 0x80, volume);
                }
            }
        }
        if (D_800E27EC < 25) {
            pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
            func_800CE870(pool, 0, target);
            pool = ANIM_POOL_AS(RoomM005Channel)->pool;
            child = func_800CE610(pool);
            if (child) {
                child->seed = func_80071A54();
                spread = func_80071A54() & 0x1FF;
                child->y = target[1] + spread - 0x100;
                child->flag = 0;
                child->radius = (func_80071A54() & 0xFF) + 0x100;
                child->state = 0;
                child->timer = 0;
            }
        }
        if (state->armed == 1) {
            state->frame++;
            if ((s16)state->frame >= 32) return 1;
        } else if (ANIM_TASK_AS(RoomM005EventState)->triggered != 0) {
            state->armed = 1;
        }
        break;
    case 2:
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        func_800CE870(pool, 0, (s16 *)D_80190B84);
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        FieldEng_TransformMatrixPoint(pool, 20, &seed, D_80190B8C);
        {
            int idx = D_800E11E8;
            int palette;
            D_800F3368.parameter00 = 16;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 16;
            D_800F3368.extent_y = 16;
            palette = D_800E2850[idx];
            PE1_COMPILER_MEMORY_BARRIER();
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = palette;
        }
        break;
    }
    return 0;
}
