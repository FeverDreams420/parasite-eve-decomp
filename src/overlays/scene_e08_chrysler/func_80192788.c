#include "scene_e08.h"
#include "scene_particle_slots.h"

void func_80192788(void *arg0, void *arg1, SceneMovingSlots *slots) {
    unsigned int i;

    for (i = 0; i < 16; i++) {
        slots->active[i] = 0;
    }
}
