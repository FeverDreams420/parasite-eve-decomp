#include "pe1/scene_e08_orbit_particles.h"
#include "common.h"

extern int func_80071A54(void);

void func_80192F50(void *unused, char *condition_arg, SceneE08OrbitParticles *effect) {
    /* Keep the four live cursors in the registers used by the original loop. */
    char *condition = condition_arg;
    register char *base asm("$18") = (char *)effect;
    register char *state asm("$16") = base;
    register char *data asm("$17") = base;
    int random;

    do {
        if (((SceneE08OrbitParticles *)state)->active[0] == 0) {
            if (func_80071A54() % 30 == 0 && *(s16 *)(condition + 2) < 50) {
                ((SceneE08OrbitParticles *)state)->active[0] = 1;
                ((SceneE08OrbitParticles *)state)->frame[0] = 0;
                ((SceneE08OrbitParticles *)data)->angle[0] = func_80071A54() % 4096;
                ((SceneE08OrbitParticles *)data)->size[0] = 0x7e8;
                ((SceneE08OrbitParticles *)data)->radius[0] = 0x7e8;
                random = func_80071A54();
                ((SceneE08OrbitParticles *)state)->radialSpeed[0] = 0;
                ((SceneE08OrbitParticles *)state)->angularSpeed[0] = random % 20 + 40;
            }
        } else {
            ((SceneE08OrbitParticles *)state)->frame[0] += 1;
            ((SceneE08OrbitParticles *)data)->angle[0] += ((SceneE08OrbitParticles *)state)->angularSpeed[0];
            ((SceneE08OrbitParticles *)data)->radius[0] -= ((SceneE08OrbitParticles *)state)->radialSpeed[0];
            ((SceneE08OrbitParticles *)state)->radialSpeed[0] += 2;
            ((SceneE08OrbitParticles *)data)->size[0] -= 40;
            if (((SceneE08OrbitParticles *)data)->size[0] < 0) {
                ((SceneE08OrbitParticles *)data)->size[0] = 0;
            }
            if (((SceneE08OrbitParticles *)data)->radius[0] < 0) {
                ((SceneE08OrbitParticles *)state)->active[0] = 0;
            }
        }
        state++;
        data += 2;
    } while (state < base + 32);

    if (*(s16 *)(condition + 2) >= 121) {
        *(u8 *)(condition + 1) = 2;
    }
}
