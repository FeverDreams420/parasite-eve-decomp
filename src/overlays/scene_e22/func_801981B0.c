#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"

typedef struct SceneE22Effect {
    u16 x, y, z;
    s16 speed;
    s16 state;
    s16 frame;
} SceneE22Effect;

extern GteRotation D_8018F1F4;

int func_801981B0(int mode, SceneE22Effect *effect) {
    GteRotation position = D_8018F1F4;
    int angle, sine, brightness, palette, kind;
    u16 clut;

    switch (mode) {
    case 1: {
        if (effect->state != 0) return 0;
        effect->frame++;
        effect->y -= (effect->speed * 24) / 4096;
        if (effect->frame >= 32) return 1;
        break;
    }
    case 2: {
        if (effect->state != 0) return 0;
        angle = effect->frame << 5;
        position.z = D_800E27EC << 5;
        sine = rcos(angle);
        brightness = (effect->speed * sine) / 4096;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(112, palette);
        func_800CEE20((GteShortVector *)effect, &position, brightness, brightness, 0x22, clut, 255, 128, 0);
        break;
    }
    }
    return 0;
}
