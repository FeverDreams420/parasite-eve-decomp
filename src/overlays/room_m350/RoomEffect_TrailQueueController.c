#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_texture_tables.h"

typedef RoomM350Vector Vector;
extern RoomM350Emitter *D_800F33E0;
extern unsigned char D_8019A86E;
extern short D_8019A85C;
extern Vector g_RoomEffectTrailPositions[4];
extern unsigned short D_800E11E8;
/* Retail writes both fields twice during configuration. */
extern int func_8019A014(int, short *);

int func_8019784C(int event)
{
    if (event == 1) goto update;
    if (event < 2) {
        if (event == 0) goto setup;
        return 0;
    }
    if (event == 2) goto configure;
    return 0;
setup:
    return func_800CE560(D_800F33E0->pool, 8, 16,
                         (FieldAnimCallbackListCallback)func_8019A014);
update:
    if (D_8019A86E) return 2;
    {
        int i;
        for (i = 0; i < D_8019A85C; i++) {
            short *effect = func_800CE610(D_800F33E0->pool);
            if (!effect) break;
            effect[0] = g_RoomEffectTrailPositions[i].x;
            effect[1] = g_RoomEffectTrailPositions[i].y;
            effect[2] = g_RoomEffectTrailPositions[i].z;
            /* Finish the copy before reloading the possibly aliased count. */
            asm("" : "=m"(D_8019A85C) : "m"(D_8019A85C), "m"(effect[2]));
        }
        D_8019A85C = 0;
    }
    goto done;
configure:
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
done:
    return 0;
}
