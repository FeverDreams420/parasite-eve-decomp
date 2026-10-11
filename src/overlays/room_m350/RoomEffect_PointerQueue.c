#include "room_m350_data.h"
#include "room_m350_shared.h"
#include "pe1/room_m350_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"

typedef RoomM350Vector Vector;
typedef struct { Vector *position; short offset,reserved; } Particle;
typedef RoomM350PointerQueue Queue;

#include "pe1/gte_sine_table.h"
#include "RoomEffect_PointerQueue_externs.h"

int func_80194F04(int event, Particle *object)
{
    Vector position;
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        object->offset -= 16;
    } else if (event == 2) {
        int frame,kind,palette,handle;
        position = *object->position;
        position.y += object->offset;
        kind = D_800F3368.palette;
        frame = D_800E27EC - 1;
        palette = D_800E1204[kind];
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2);
        func_800CEE20((GteShortVector *)&position, 0, 8192, 8192, (s16)D_800F3368.parameter02 * (frame >> 1),
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)frame << 7) & 0xF80].word >> 5,
            &D_8019A4E8);
    }
    return 0;
}

int func_80195064(int event) {
    int firstIndex;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,8,16,
                             (FieldAnimCallbackListCallback)func_80194F04);
        case 1:
    if (D_8019A7FC.completionFlags[0] && D_8019A7FC.activeCount==0) return 2;
    {
        short *count=&D_8019A7FC.pointerCount;
        if (D_8019A7FC.pointerCount) {
            int i;
            firstIndex=0;
            i=firstIndex;
            if (D_8019A7FC.pointerCount>firstIndex) {
                Queue *queue=(Queue *)((char *)count-(unsigned long)&((Queue *)0)->count);
                Vector **position=queue->positions;
                while (i<queue->count) {
                    {
                        Particle *particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
                        if (!particle) break;
                        particle->position=*position;
                        particle->offset=0;
                    }
                    i++;
                    position++;
                }
            }
            D_8019A7FC.pointerCount=0;
        }
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
