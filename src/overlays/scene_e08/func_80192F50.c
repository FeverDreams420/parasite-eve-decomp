#include "pe1/scene_e08_orbit_particles.h"
#include "common.h"

extern int func_80071A54(void);

void func_80192F50(void *unused, char *condition_arg, SceneE08OrbitParticles *effect) {
    /* The byte cursor and loop-limit base remain pinned for matching. */
    char *condition = condition_arg;
    register char *base asm("$18") = (char *)effect;
    register char *state asm("$16") = base;
    int random;
    unsigned i = 0;

    do {
        if (((SceneE08OrbitParticles *)state)->active[0] == 0) {
            if (func_80071A54() % 30 == 0 && *(s16 *)(condition + 2) < 50) {
                ((SceneE08OrbitParticles *)state)->active[0] = 1;
                ((SceneE08OrbitParticles *)state)->frame[0] = 0;
                effect->angle[i] = func_80071A54() % 4096;
                effect->size[i] = 0x7e8;
                effect->radius[i] = 0x7e8;
                random = func_80071A54();
                ((SceneE08OrbitParticles *)state)->radialSpeed[0] = 0;
                ((SceneE08OrbitParticles *)state)->angularSpeed[0] = random % 20 + 40;
            }
        } else {
            ((SceneE08OrbitParticles *)state)->frame[0] += 1;
            effect->angle[i] += ((SceneE08OrbitParticles *)state)->angularSpeed[0];
            effect->radius[i] -= ((SceneE08OrbitParticles *)state)->radialSpeed[0];
            ((SceneE08OrbitParticles *)state)->radialSpeed[0] += 2;
            effect->size[i] -= 40;
            if (effect->size[i] < 0) {
                effect->size[i] = 0;
            }
            if (effect->radius[i] < 0) {
                ((SceneE08OrbitParticles *)state)->active[0] = 0;
            }
        }
        i++;
        state++;
    } while (state < base + 32);

    if (*(s16 *)(condition + 2) >= 121) {
        *(u8 *)(condition + 1) = 2;
    }
}
