#include "scene_e08.h"
#include "pe1/scene_e08_orbit_particles.h"
#include "pe1/gte_types.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_script_context.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef GteMatrix SceneE08Matrix;
typedef GteVector SceneE08Vector;

extern u8 D_801996A0[];
extern char D_80198848[];

void func_800C2FF0(int, int);
void func_800C3098(int);
void func_80192D90(void *unused0, void *unused1, SceneE08OrbitParticles *effect) {
    SceneE08Matrix matrix;
    s16 temp[3];
    SceneE08Vector transformed, v;
    /* Keep the matrix scale live before the first rendering setup call. */
    register s16 scale asm("$19") = 0x1000;
    char *table = (char *)D_801996A0;
    Ovl178Position *owner;
    unsigned i;

    owner = func_800C2B50();
    i = 0;
    asm volatile("" : : "r"(scale));
    func_800C2EAC(owner->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    for (; i < 32; i++) {
        if (effect->active[i] == 1) {
            temp[0] = (rsin(effect->angle[i]) * effect->radius[i]) >> 12;
            temp[1] = 0;
            temp[2] = (rcos(effect->angle[i]) * effect->radius[i]) >> 12;
            matrix.m[2][2] = scale;
            matrix.m[1][1] = scale;
            matrix.m[0][0] = scale;
            matrix.t[2] = 0;
            matrix.t[1] = 0;
            matrix.t[0] = 0;
            matrix.m[2][1] = 0;
            matrix.m[2][0] = 0;
            matrix.m[1][2] = 0;
            matrix.m[1][0] = 0;
            matrix.m[0][2] = 0;
            matrix.m[0][1] = 0;
            memset(&v, 0, 0x10);
            v.x = effect->size[i];
            v.y = effect->size[i];
            v.z = scale;
            transformed = v;
            ScaleMatrix(&matrix, &transformed);
            matrix.t[0] = temp[0] + D_8019956C;
            matrix.t[1] = temp[1];
            matrix.t[2] = temp[2] + D_8019957C;
            func_800C3134(D_80198848, effect->frame[i], table);
            func_800C42A4((FieldGlowSprite *)table, &matrix, 1);
        }
    }
}
