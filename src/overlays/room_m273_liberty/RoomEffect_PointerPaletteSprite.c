#include "pe1/field_sprite_params.h"
#include "pe1/field_actor.h"
#include "room_m273_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"

typedef struct { short x, y, z, pad; } Sprite;

extern unsigned short D_800966EC[];
extern GteRotation D_8019AB68;
extern FieldActor *g_PlayerEntity;
extern unsigned char D_8019AE5C;

int func_80193870(int mode, RoomM273SpritePoolEffect *state) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
    } else if (mode == 2) {
        int index = ((D_800E27EC - 1) << 8) & 0x3f00;
        Sprite sprite;
        int product;
        int sample;
        short size;
        short shade;
        RoomM273WorldPosition *p = state->position;
        int kind;
        int palette;
        int x;

        product = ((short *)D_800966EC)[index / 2] * 2 + 4096;
        size = product;
        sample = ((short *)D_800966EC)[index / 2 + 1];
        shade = (unsigned int)sample >> 5;
        x = p->x;
        sprite.y = p->y;
        sprite.z = p->z;
        sprite.x = x;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        func_800CEE20((GteShortVector *)&sprite, &D_8019AB68, (short)size, (short)size, 102,
                        GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5), 1, (short)shade, 0);
    }
    return 0;
}

int func_801939B4(int mode) {
    switch (mode) {
    case 0:
        D_8019AE5C = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool, 4, 1,
                             (FieldAnimCallbackListCallback)func_80193870);
    case 1:
        if (D_800E27EC >= 20) return 2;
        if (D_8019AE5C) return 2;
        if (g_PlayerEntity->mode < 4) {
            RoomM273SpritePoolEffect *effect =
                func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
            if (effect) {
                RoomM273PlayerTransform *transform = (RoomM273PlayerTransform *)g_PlayerEntity->render_object.matrices;
                D_8019AE5C = 1;
                effect->position = &transform->position;
            }
        }
        break;
    case 2:
        if (D_8019AE5C) {
            int index = D_800E11FA;
            int palette;
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            palette = D_800E2850[index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0;
            D_800F3368.tpage = palette;
        }
        break;
    }
    return 0;
}
