#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"

extern short D_800966EE[];
extern RenderColor D_8019A62C;
typedef RoomM350SignedHalfPair SignedHalf;

int func_80197594(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] -= position[3];
        position[3] += 2;
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int texture = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? texture + 6 : texture + 2);
        func_800CEE20((GteShortVector *)position, 0, 6144, 6144,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7),
            (unsigned short)handle, 1,
            ((SignedHalf *)((char *)D_800966EE - 2 +
              (((unsigned int)(D_800E27EC - 1) << 8) & 0x3F00)))->value >> 5,
            &D_8019A62C);
    }
    return 0;
}

extern unsigned char D_8019A857;
extern short D_8019A864[3];

int func_801976C8(int event)
{
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 4,
                             (FieldAnimCallbackListCallback)func_80197594);
        case 1:
    if (D_8019A86E) return 2;
    if (D_8019A857) {
        short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!effect) return 0;
        do {
            effect[0] = D_8019A864[0];
            effect[1] = D_8019A864[1];
            effect[2] = D_8019A864[2];
            effect[3] = 0;
        } while (0);
        D_8019A857 = 0;
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
