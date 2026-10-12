#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "room_m350_data.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"

typedef RoomM350Vector Vector;
#include "RoomEffect_TrailQueueController_externs.h"
/* Retail writes both fields twice during configuration. */

int func_8019784C(int event)
{
    switch (event) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldAnimCallbackListCallback)func_8019A014);
    case 1:
    if (D_8019A86E) return 2;
    {
        int i;
        for (i = 0; i < D_8019A85C; i++) {
            short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            effect[0] = g_RoomEffectTrailPositions.points[i].x;
            effect[1] = g_RoomEffectTrailPositions.points[i].y;
            effect[2] = g_RoomEffectTrailPositions.points[i].z;
            /* Finish the copy before reloading the possibly aliased count. */
            asm("" : "=m"(D_8019A85C) : "m"(D_8019A85C), "m"(effect[2]));
        }
        D_8019A85C = 0;
    }
    break;
    case 2:
    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = unit;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        palette = D_800E2850[index];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}
