#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/room_fx.h"
#include "pe1/field_texture_tables.h"

#ifndef ROOMLIB_POLL_RESET_FUNC
#error "ROOMLIB_POLL_RESET_FUNC must name the room entry point"
#endif
#ifndef ROOMLIB_POLL_RESET_CALLBACK
#error "ROOMLIB_POLL_RESET_CALLBACK must name the spawned callback"
#endif
#ifndef ROOMLIB_POLL_RESET_FLAG
#error "ROOMLIB_POLL_RESET_FLAG must name the completion flag"
#endif
#ifndef ROOMLIB_POLL_RESET_COUNTER
#error "ROOMLIB_POLL_RESET_COUNTER must name the pending counter"
#endif
#ifndef ROOMLIB_POLL_RESET_SEED
#error "ROOMLIB_POLL_RESET_SEED must name the copied seed"
#endif

#ifndef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL extern char **ANIM_POOL_AS(RoomM273PoolContext)
#endif
#ifndef ROOMLIB_POLL_RESET_POOL_EXPR
#define ROOMLIB_POLL_RESET_POOL_EXPR ANIM_POOL_AS(RoomM273PoolContext)[2]
#endif
ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL;
#include "RoomLib_PollAndResetActor_externs.h"

int ROOMLIB_POLL_RESET_CALLBACK();

s32 ROOMLIB_POLL_RESET_FUNC(s32 state) {
    RoomFxSeed8 *obj;
    int index;
    u16 size;
    u16 tableValue;
    u16 value2;
    u16 value3;
    u16 value1;

    switch (state) {
    case 0:
        return func_800CE560(ROOMLIB_POLL_RESET_POOL_EXPR, 8, 4,
                             (FieldAnimCallbackListCallback)ROOMLIB_POLL_RESET_CALLBACK);
    case 1:
        if (ROOMLIB_POLL_RESET_FLAG != 0) {
            return 2;
        }
        if (ROOMLIB_POLL_RESET_COUNTER == 0) {
            return 0;
        }
        obj = (RoomFxSeed8 *)func_800CE610(ROOMLIB_POLL_RESET_POOL_EXPR);
        if (obj != 0) {
            ROOMLIB_POLL_RESET_COUNTER = 0;
            *obj = ROOMLIB_POLL_RESET_SEED;
        }
        return 0;
    case 2:
        index = D_800E11FA;
        size = 0x20;
        D_800F3368.parameter00 = size;
        value2 = 2;
        D_800F3368.parameter02 = value2;
        D_800F3368.extent_x = size;
        D_800F3368.extent_y = size;
        D_800F3368.extent_x = size;
        D_800F3368.extent_y = size;
        tableValue = D_800E2850[index];
        value3 = 3;
        D_800F3368.palette = value3;
        value1 = 1;
        D_800F3368.parameter06 = value1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = tableValue;
        break;
    }
    return 0;
}

#undef ROOMLIB_POLL_RESET_FUNC
#undef ROOMLIB_POLL_RESET_CALLBACK
#undef ROOMLIB_POLL_RESET_FLAG
#undef ROOMLIB_POLL_RESET_COUNTER
#undef ROOMLIB_POLL_RESET_SEED
#undef ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#undef ROOMLIB_POLL_RESET_POOL_EXPR
