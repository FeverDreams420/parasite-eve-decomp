#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/room_m350_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
#include "room_m350_data.h"
#include "pe1/gte_sine_table.h"
typedef RoomM350Vector Vector;
typedef struct { Vector position; short delay,reserved; } Particle;
typedef struct RoomM350Instance { char reserved[0x3A]; unsigned short yaw; } Instance;
typedef RoomM350Actor Actor;
typedef RoomM350PositionQueue Queue;

#include "RoomEffect_PositionQueue_externs.h"

int func_80195218(int event, Particle *object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (event == 2) {
        int frame = D_800E27EC - 2;
        if (frame >= 0) {
            int size,kind,palette,handle;
            vector[0] = 1024; vector[1] = object->position.pad;
            vector[2] = 1024; vector[3] = 1;
            size = D_800966EE[((unsigned int)frame << 7) & 0x1F80];
            kind = D_800F3368.palette; palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            handle = GetClut(0, palette);
            func_800CEE20((GteShortVector *)object, (struct GteRotation *)vector, size, size * 2, (s16)D_800F3368.parameter02 * object->delay + 64,
                (unsigned short)handle, 1,
                D_800966EC[((unsigned int)frame << 6) & 0xFC0].bits.cos >> 5,
                &D_8019A3D0);
        }
    }
    return 0;
}

int func_80195378(int event) {
    int firstIndex;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,12,50,
                             (FieldAnimCallbackListCallback)func_80195218);
        case 1:
    if (D_8019A7FC.completionFlags[0] && D_8019A7FC.activeCount==0) return 2;
    {
        short *count=&D_8019A7FC.positionCount;
        if (*count) {
            int i;
            firstIndex=0;
            i=firstIndex;
            if (*count>firstIndex) {
                Queue *queue=(Queue *)((char *)count-(unsigned long)&((Queue *)0)->count);
                Vector *position=queue->positions;
                do {
                    {
                        Particle *particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->position.pad=ANIM_OWNER_AS(Actor)->instance->yaw;
                        particle->delay=D_800E27EC & 7;
                    }
                    i++;
                    position++;
                } while(i<queue->count);
            }
            D_8019A7FC.positionCount=0;
        }
    }
        break;
    case 2:

    {
        int index=D_800E11EA, palette;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32; D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        palette=D_800E2850[index];
        D_800F3368.palette=3; D_800F3368.parameter06=0; D_800F3368.parameter0A=0; D_800F3368.depth=0; D_800F3368.tpage=palette;
    }
    }
    return 0;
}
