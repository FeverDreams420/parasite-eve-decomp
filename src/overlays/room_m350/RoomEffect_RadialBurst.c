#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
typedef RoomM350OverlayTransform Transform;
typedef struct RoomM350Instance { char reserved[0x238]; Transform *transform; } Instance;
typedef RoomM350Actor Actor;

#include "pe1/gte_sine_table.h"
extern int D_8019A634[];

int func_80197A04(int event, short *object)
{
    short position[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int scale = (4096 - *(short *)((char *)D_800966EC +
            (((unsigned int)(D_800E27EC - 1) << 9) & 0x3E00))) * 2;
        int *origin = ANIM_OWNER_AS(Actor)->instance->transform->position;
        int i;
        int kind, palette, handle;

        for (i = 0; i < 3; i++) {
            position[i] = object[i] * scale / 4096 + origin[i];
        }
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) {
            palette += 4;
        }
        handle = GetClut(32, palette);
        func_800CEE20((GteShortVector *)position, 0, 5120, 5120, (s16)D_800F3368.parameter02 * 2 + 216,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)D_8019A634);
    }
    return 0;
}
/* MASPSX_FLAGS: --expand-div */

typedef RoomM350TransformMatrix Matrix;
typedef RoomM350Vector Vector;
extern unsigned char D_8019A859;
/* Retail writes both fields twice during configuration. */
extern int func_80197A04(int, short *);
extern int Inv_ScrambleGrid(void);

int func_80197B98(int event)
{
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 8, 16,
                             (FieldAnimCallbackListCallback)func_80197A04);
        case 1:
    if (D_8019A86E) return 2;
    if (D_8019A859) {
        register int i asm("$18") = 0;
        do {
            Matrix matrix;
            Vector vector;
            int random;
            int highByte;
            int divisor;
            short *effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!effect) break;
            i++;
            vector.x = Inv_ScrambleGrid() << 4;
            vector.y = Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            /* Keep the two phases separate in CSE without emitting instructions. */
            asm volatile("" : : "i"(&&offset));
offset:
            vector.x = 0;
            vector.y = 0;
            highByte = Inv_ScrambleGrid();
            random = (highByte << 8) | Inv_ScrambleGrid();
            divisor = 64;
            vector.z = random % divisor + 256;
            ApplyMatrixSV(&matrix, &vector, (Vector *)effect);
        } while (i < 2);
        {
            unsigned char *request = &D_8019A859;
            *request = *request - 1;
        }
    }
        break;
    case 2:

    {
        int unit = 16;
        int index = D_800E11E8;
        int palette;
        D_800F3368.parameter0A = 0;
        D_800F3368.parameter00 = unit;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;

        palette = D_800E2850[index];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}
