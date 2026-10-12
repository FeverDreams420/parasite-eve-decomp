#include "common.h"
#include "pe1/akao/commands.h"
#include "pe1/field_anim.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_billboard.h"

/* Burst and spark effect setup, draw and lifecycle callbacks. */

int func_800CD50C(void *arg0, void *arg1, FieldAnimGlowPoint *anim) {
    int *base_a1;
    u16 *field_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    base_a1 = &D_800F3478.t[0];
    field_v1 = &D_800E27E0.depth;
    base_a1[0] = (s16)anim->position.x;
    D_800F3478.t[1] = (s16)anim->position.y;
    D_800F3478.t[2] = (s16)anim->position.z;
    *field_v1 = anim->brightness;
    func_800C42A4(&D_800E27E0, &D_800F3478, 1);
}

void func_800CD59C(void) {
}

int func_800CD5A4(void *arg0, FieldEngSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}

int func_800CD5B0(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    anim->brightness = anim->brightness - 0xA;
    (*(u16 *)&anim->scale) = (*(u16 *)&anim->scale) + 0x3C;
    if ((s16)anim->brightness < 0x14) {
        anim->brightness = 0;
        state->flag = 2;
    }
}

int func_800CD5EC(void *arg0, FieldEngSlot *state, FieldAnimTwoPointMotion *anim) {
    unsigned int i = 0;

    do {
        unsigned slot = i & 0xFFFF;

        i++;
        anim->position[slot].x = anim->position[slot].x + anim->velocity[slot].x;
        anim->position[slot].y = anim->position[slot].y + anim->velocity[slot].y;
        anim->position[slot].z = anim->position[slot].z + anim->velocity[slot].z;
    } while ((i & 0xFFFF) < 2);

    anim->age++;
    if ((signed char)anim->age >= 8) {
        state->flag = 2;
    }
}

int func_800CD678(void *arg0, FieldEngSlot *state, FieldAnimGlowPoint *anim) {
    int value = rand() % 11;
    int jitter;
    FieldAnimGlowPoint *output;
    u16 positionY = anim->position.y;
    int brightness = anim->brightness;
    int positionX = anim->position.x;
    positionY -= 8;
    brightness -= 8;
    output = anim;
    positionX -= 5;
    output->position.y = positionY;
    output->brightness = brightness;
    jitter = value;
    value = 8;
    positionX += jitter;
    anim->position.x = positionX;
    if (value > (short)brightness) {
        state->flag = 2;
    }
}

int func_800CD71C(void *arg0, FieldEngSlot *state) {
    int ret = 2;
    state->flag = ret;
    return ret;
}
