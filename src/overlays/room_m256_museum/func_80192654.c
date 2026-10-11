/* MASPSX_FLAGS: --expand-div */
#include "pe1/room_m256_swing.h"
/* Cosine halves of the packed sine table words (pe1/gte_sine_table.h). */
#include "func_80192654_externs.h"


void func_80192680(RoomM256Swing *state);

void func_80192654(RoomM256Swing *obj) {
    FieldActor *slot = obj->actor;

    if (slot->mode == 0xB) {
        obj->cycle.callback = (void (*)(void))func_80192680;
    }
}

void func_80192680(RoomM256Swing *state) {
    FieldActor *actor = state->actor;
    RoomM256SwingCycle *base = &state->cycle;
    int first;
    int phase;
    int value;
    int timer;
    int baseValue;

    if (state->cycle.started == 0) {
        state->cycle.started = 1;
        state->cycle.tick = 0;
        first = base->angles[state->cycle.segment];
        state->cycle.startAngle = first;
        state->cycle.sweep = base->angles[state->cycle.segment + 1] - first;
        state->cycle.length = base->durations[state->cycle.segment];
    }

    timer = (u16)state->cycle.tick + 1;
    phase = ((timer << 16) >> 5) / state->cycle.length;
    state->cycle.tick = timer;
    value = (-D_800966EE[(phase & 0xFFF) * 2] + 0x1000) *
            state->cycle.sweep;
    if (value < 0) {
        value += 0x1FFF;
    }

    baseValue = (u16)state->cycle.startAngle;
    actor->render_object.rotation_overrides[0].matrix_index = 3;
    actor->render_object.rotation_overrides[0].flags = 2;
    actor->render_object.rotation_overrides[0].y = baseValue + (value >> 13);

    if (state->cycle.tick >= state->cycle.length) {
        state->cycle.segment = (u16)state->cycle.segment + 1;
        if (state->cycle.segment >= 3) {
            func_80192808(state);
        } else {
            state->cycle.started = 0;
        }
    }

    if (actor->anim.parts.integer >= actor->action) {
        actor->anim.fixed = 0;
    }
}
