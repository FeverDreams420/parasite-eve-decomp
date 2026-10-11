#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
#include "room_m350_data.h"
typedef RoomM350Vector Vector;
typedef struct { Vector *position; short scale, reserved; } Particle;
typedef struct { short target, current; } State;

typedef struct RoomM350Instance { char reserved[22]; unsigned short frame; } Instance;
typedef RoomM350Actor Actor;

/* The spawner owns the scaled particles rendered by this callback. */
#include "pe1/gte_sine_table.h"
extern unsigned short D_800E11E8;

int func_80192ADC(int event, Particle *object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int size = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)) * object->scale / 4096;
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        int handle;

        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20(object->position, 0, size, size, (s16)D_800F3368.parameter02 + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80].word >> 5,
            &D_8019A3D0);
    }
    return 0;
}

int func_80192C34(int event, State *state)
{
    switch (event) {
    case 0:

        state->target = 16384;
        state->current = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 6,
                             (FieldAnimCallbackListCallback)func_80192ADC);
        case 1:
    if (!state->target) {
        if (state->current <= 0) return 2;
    } else if (ANIM_OWNER_AS(Actor)->instance->frame >= 41) state->target = 0;
    if (D_800E27EC & 7) return 0;
    if (state->current < state->target) state->current += 4096;
    else if (state->target < state->current) state->current -= 4096;
    if (state->current) {
        int i;
        for (i = 0; i < 2; i++) {
            Particle *particle = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!particle) return 0;
            particle->position = &D_8019A778[i];
            particle->scale = state->current;
        }
    }
        break;
    case 2:

    {
        int unit = 16;
        int palette;
                palette = D_800E2850[D_800E11E8];
        D_800F3368.palette = 2; D_800F3368.parameter06 = 0;
        D_800F3368.parameter00 = unit; D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = unit; D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit; D_800F3368.extent_y = unit;
        D_800F3368.parameter0A = 0; D_800F3368.depth = 0; D_800F3368.tpage = palette;
    }
    }
    return 0;
}
