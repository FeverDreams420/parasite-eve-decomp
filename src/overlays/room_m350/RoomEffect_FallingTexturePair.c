#include "room_m350_shared.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte_sine_table.h"

typedef struct { short x,y,z,frame; } Particle;
typedef RoomM350TransformOwner Instance;


/* The spawner owns the falling particles updated and rendered by this callback. */
extern FieldActor *g_PlayerEntity;
extern RenderColor D_8019A464;
extern unsigned char D_8019A79A,D_8019A79C;

int func_801937B4(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3]++;
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 6 : texture + 2);
        func_800CEE20((GteShortVector *)position, 0, 8192, 8192,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            D_800966EC[((unsigned int)(D_800E27EC - 1) << 6) & 0xFC0].bits.cos >> 5,
            &D_8019A464);
    }
    return 0;
}

int func_801938E4(int event) {
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,8,4,
                             (FieldAnimCallbackListCallback)func_801937B4);
        case 1:
    if(D_8019A79A) return 2;
    if(D_8019A79C) {
        Particle *particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        Instance *player;
        if(!particle) return 0;
        do {
            player=(Instance *)g_PlayerEntity;
            particle->x=player->transforms->position[0];
            particle->y=player->transforms->position[1];
            particle->z=player->transforms->position[2];
            particle->frame=0;
        } while (0);
        D_8019A79C=0;
    }
        break;
    case 2:

    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 0;
    }
    return 0;
}
