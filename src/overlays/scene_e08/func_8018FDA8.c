#include "common.h"
#include "pe1/room_fx.h"
#include "scene_particle_slots.h"

void func_8018FDA8(void *unused, RoomFxControl *signal, u8 *state) {
    SceneParticleCluster *slots = (SceneParticleCluster *)state;
    int eight;
    unsigned i;
    int one;

    i = 0;
    one = 1;
    eight = 8;
    for (; i < 30; i++) {
        u8 timer = slots->timer[i];
        int done;
        if (timer != 0) {
            timer--;
            slots->timer[i] = timer;
            if (timer == 0) {
                slots->active[i] = one;
            }
        }

        if (slots->active[i] != one) {
            continue;
        }

        slots->brightness[i] -= 0x10;
        slots->counter[i]++;
        done = 0;

        if (slots->texture[i] == 0x44) {
            slots->scale[i] += 0xC8;
            if (slots->texture[i] == 0x44 && slots->counter[i] == eight) {
                done = 1;
            }
        }
        if (slots->texture[i] == 0x4C && slots->counter[i] == eight) {
            done = 1;
        }
        if (slots->texture[i] == 0x58 && slots->counter[i] == 0x10) {
            done = 1;
        }
        if (done == one) {
            slots->active[i] = 0;
            slots->count--;
            if (slots->count == 0) {
                signal->status = 2;
            }
        }
    }
}
