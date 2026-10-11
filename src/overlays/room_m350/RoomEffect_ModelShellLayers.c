#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
#include "room_m350_data.h"
/* MASPSX_FLAGS: --expand-div */
typedef RoomM350Vector Vector;
typedef RoomM350TransformMatrix Matrix;
typedef struct { int color; short x, y, z, size; } Particle;
typedef RoomM350TransformOwner Instance;

#include "pe1/gte_sine_table.h"
extern int D_8019A3CC[];
extern int Inv_ScrambleGrid(void);

int func_8019404C(int event, Particle *particle)
{
    char frameGap[8];
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        int size = D_800966EC[(((unsigned int)D_800E27EC - 1) << 7) & 0xF80].part.sin * particle->size / 4096;
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        unsigned short clut;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20((GteShortVector *)&particle->x, 0, size, size, (s16)D_800F3368.parameter02 * 5 + 216, clut, 1,
            D_800966EC[(((unsigned int)D_800E27EC - 1) << 7) & 0xF80].bits.cos >> 5, (struct RenderColor *)particle);
        func_800D004C((GteShortVector *)&particle->x, 96, 96, 8, 0, size, size, (struct RenderColor *)D_8019A3CC, &D_8019A3C8,
            (short)D_800966EC[(((unsigned int)D_800E27EC - 1) << 8) & 0xF00].word >> 5, 1);
    }
    return 0;
}

int func_8019421C(int event)
{
    Vector offset;
    Matrix matrix;
    Vector vector;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 12, 64,
                             (FieldAnimCallbackListCallback)func_8019404C);
        case 1:
    {
        int i = 0;
        Vector *out;
        if (D_800E27EC >= 41) return 2;
        out = &offset;
        for (; i < 2; i++) {
            int random;
            int highByte;
            int r, g, b, divisor;
            int j, *position;
            Particle *particle = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
            if (!particle) return 0;
            vector.x = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.y = (unsigned int)Inv_ScrambleGrid() << 4;
            vector.z = 0;
            RotMatrixYXZ(&vector, &matrix);
            asm volatile("" : : "i"(&&offset_start));
offset_start:
            vector.x = 0; vector.y = 0;
            highByte = Inv_ScrambleGrid();
            random = ((unsigned int)highByte << 8) | (unsigned int)Inv_ScrambleGrid();
            divisor = 192;
            vector.z = random % divisor + 128;
            ApplyMatrixSV(&matrix, &vector, out);
            position = D_8019A7F8->render_object.matrices[i ? 11 : 15].translation;
            for (j = 0; j < 3; j++) (&particle->x)[j] = ((short *)out)[j] + (unsigned int)position[j];
            particle->size = (unsigned int)D_800966EC[((int)((unsigned int)D_800E27EC << 11) / 40) & 4095].word + 4096;
            r = Inv_ScrambleGrid(); g = Inv_ScrambleGrid(); b = Inv_ScrambleGrid();
            particle->color = (r >> 1) | ((unsigned int)(g >> 1) << 8) | ((unsigned int)(b >> 1) << 16);
        }
    }
        break;
    case 2:

    {
        int index = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = 16; D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16; D_800F3368.extent_y = 16;
        D_800F3368.extent_x = 16; D_800F3368.extent_y = 16;
        palette = D_800E2850[index];
        D_800F3368.palette = 2; D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0; D_800F3368.depth = 0; D_800F3368.tpage = palette;
    }
    }
    return 0;
}
