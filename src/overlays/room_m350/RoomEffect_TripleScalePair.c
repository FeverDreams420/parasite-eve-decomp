#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"

typedef RoomM350Vector Vector;
typedef struct { Vector *position; } Particle;
typedef struct RoomM350Instance {
    char reserved[14];
    unsigned char animation;
    char reserved0F[7];
    unsigned short frame;
    short reserved18;
    unsigned short previousFrame;
} Instance;
typedef RoomM350Actor Actor;

/* The controller owns the position pointers rendered by this callback. */
extern int D_800966EC[];
extern Vector D_8019A778[];
extern int GetClut(int, int);

int func_80193A80(int event, void **object)
{
    if (event == 1) {
        if (D_800E27EC >= 16) {
            return 1;
        }
    } else if (event == 2) {
        int sample = *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00));
        int kind = D_800F3368.palette;
        int size = sample * 3;
        int palette = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 8 : palette + 4);
        func_800CEE20((GteShortVector *)*object, 0, size, size, (s16)D_800F3368.parameter02 * 6 + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5, 0);
    }
    return 0;
}

int func_80193BCC(int event) {
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,4,2,
                             (FieldAnimCallbackListCallback)func_80193A80);
        case 1:
    {
        Instance *instance = ANIM_OWNER_AS(Actor)->instance;
        int frame, previous, i;
        if (instance->animation != 11) return 0;
        frame = instance->frame;
        previous = instance->previousFrame;
        if (frame >= 5 && previous < 5) {
            for (i=0;i<2;i++) {
                Particle *particle = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
                if (!particle) break;
                particle->position = &D_8019A778[i];
            }
            return 0;
        }
        if (frame >= 41) return 2;
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
