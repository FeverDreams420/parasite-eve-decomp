#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
#include "room_m350_data.h"

typedef struct { short position[3]; short size; } Particle;
typedef RoomM350TransformMatrix Transform;
typedef struct RoomM350Instance { char reserved[22]; unsigned short frame; char reserved18[0x220]; Transform *transforms; } Instance;
typedef RoomM350Actor Actor;

typedef struct { unsigned char index, alternate; } Entry;
extern unsigned char D_8019A804;
extern Entry D_8019A4D4[][2];
extern int D_800966EC[];
extern unsigned char D_8019A4EC[];
extern RenderColor D_8019A3D0;

int func_80195564(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 8 : texture + 4);
        func_800CEE20((GteShortVector *)position, 0, position[3], position[3],
            (s16)D_800F3368.parameter02 * D_8019A4EC[(D_800E27EC - 1) / 2] + 128,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80] >> 5,
            &D_8019A3D0);
    }
    return 0;
}

int func_8019569C(int event) {
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,8,10,
                             (FieldAnimCallbackListCallback)func_80195564);
        case 1:
    if(D_8019A804) return 2;
    {
        int frame=ANIM_OWNER_AS(Actor)->instance->frame;
        unsigned int relative=frame-7;
        int i;
        if(relative>=6) return 0;
        for(i=0;i<2;i++) {
            Entry *entry=&D_8019A4D4[frame][i];
            Particle *particle;
            Instance *instance;
            int j;
            if(!entry->index) return 0;
            particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if(!particle) return 0;
            if(entry->alternate) instance=(Instance *)D_8019A7F8;
            else instance=ANIM_OWNER_AS(Actor)->instance;
            {
                int *source=instance->transforms[entry->index].position;
                for(j=0;j<3;j++) particle->position[j]=source[j];
            }
            particle->size=D_800966EC[(relative<<7)&0xF80]+2048;
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
