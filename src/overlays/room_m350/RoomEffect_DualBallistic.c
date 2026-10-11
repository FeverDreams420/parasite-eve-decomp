#include "pe1/field_sprite_params.h"
#include "pe1/field_actor.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"
typedef RoomM350TransformMatrix Matrix;
typedef RoomM350TransformOwner Instance;
typedef RoomM350Actor Actor;

typedef struct { short x, y, z, velocityY, gravity, reserved; } Particle;
extern FieldActor *g_PlayerEntity;
extern unsigned char D_8019A82F;
#include "pe1/gte_sine_table.h"
extern int D_8019A598[];
extern int Inv_ScrambleGrid(void);

int func_801968D8(int event, short *position) {
    if (event == 1) {
        if (D_800E27EC >= 16) return 1;
        position[1] += position[3];
        position[3] -= position[4];
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        int handle;
        if (kind == 4 && D_800F3428) palette += 4;
        handle = GetClut(16, palette);
        func_800CEE20((GteShortVector *)position, 0, 12288, 12288,
            (s16)D_800F3368.parameter02 * (((D_800E27EC - 1) >> 1) & 7) + 200,
            (unsigned short)handle, 1,
            (short)D_800966EC[((unsigned int)(D_800E27EC - 1) << 7) & 0xF80].word >> 5,
            (struct RenderColor *)D_8019A598);
    }
    return 0;
}

int func_80196A14(int event)
{
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 12, 16,
                             (FieldAnimCallbackListCallback)func_801968D8);
        case 1:
    if (D_800E27EC >= 107) return 2;
    if (D_800E27EC & 1) {
        Matrix *matrices = ((Instance *)ANIM_OWNER_AS(Actor)->instance)->transforms;
        Particle *particle = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (particle) {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[33].position[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[33].position[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[33].position[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((D_800E27EC >> 1) & 1) + 2;
        }
    }
    if (D_8019A82F) {
        Matrix *matrices = ((Instance *)g_PlayerEntity)->transforms;
        Particle *particle = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if (!particle) return 0;
        {
            int random = Inv_ScrambleGrid();
            particle->x = (unsigned int)matrices[0].position[0] - (((unsigned int)random << 1) - 256);
            particle->y = matrices[0].position[1];
            random = Inv_ScrambleGrid();
            particle->z = (unsigned int)matrices[0].position[2] - (((unsigned int)random << 1) - 256);
            particle->velocityY = 0;
            particle->gravity = ((D_800E27EC >> 1) & 1) + 2;
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
