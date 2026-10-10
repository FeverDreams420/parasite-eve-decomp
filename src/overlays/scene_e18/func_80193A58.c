#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/gte_types.h"

typedef struct SceneE18WaveEffect {
    s16 x, y, z, field06;
    s16 field08, field0a, field0c, field0e;
    s16 size, wave, amplitude;
} SceneE18WaveEffect;
extern u32 D_80194180[], D_80194184[];
int func_80077AA4(int,int);

int func_80193A58(int mode, SceneE18WaveEffect *effect) {
    s16 position[4];
    s16 params[4];
    int palette;
    register u16 index;
    if (mode == 1) {
        effect->wave = (rsin(D_800E27EC << 4) * effect->amplitude) >> 12;
        if (D_800E27EC < 8) return 0;
        effect->size -= 8;
        if (effect->size >= 16) goto ret0;
        return 1;
    }
    if (mode != 2) return 0;
    index = D_800F3368.palette;
    palette = D_800E1204[index];
    palette = func_80077AA4(0, (index == 4 && D_800F3428 != 0) ? palette + 7 : palette + 3);
    func_800CEE20((struct GteShortVector *)effect, (struct GteRotation *)&effect->field08, effect->wave, effect->wave,
                  0x40, palette & 0xffff, 3, effect->size, 0);
    position[0] = effect->x;
    position[1] = *(int *)(*(char **)((char *)ANIM_OWNER_AS(void) + 8) + 0x200);
    position[2] = effect->z;
    params[0] = 0x400;
    params[1] = (D_800E27EC * 3) << 6;
    params[2] = 0;
    params[3] = 1;
    func_800D004C((struct GteShortVector *)position, 0x100, 0x100, 10, (struct GteRotation *)params,
                  effect->wave, effect->wave, (struct RenderColor *)D_80194180, (struct RenderColor *)D_80194184,
                  effect->size, 1);
ret0:
    return 0;
}
