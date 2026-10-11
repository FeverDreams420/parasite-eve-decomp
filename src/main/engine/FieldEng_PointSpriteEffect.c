#include "common.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_anim.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_engine_slot.h"
#include "pe1/battle.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/gte_types.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/akao/commands.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_sprite_draw.h"

#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))

#include "FieldEng_PointSpriteEffect_externs.h"


int func_800CBCA4(u8 *arg0) {
    u8 *obj;
    register int tmp asm("$2");
    int y_scale;
    int color;
    register int z asm("$6");
    register int setupValue asm("$4");
    void **slot;

    slot = FieldEng_GetSlot((char *)arg0);
    *slot = D_800E0D6C;
    obj = (u8 *)((FieldAnimObjectPrefix *)arg0)->actor;

    tmp = S16(obj, 0x2A);
    setupValue = -0x64;
    D_800E2290.x = tmp;
    tmp = S16(obj, 0x2E);
    y_scale = 0x400;
    D_800F3470 = obj;
    D_800E2292 = tmp;

    tmp = 0;
    asm volatile("" : "=r"(tmp) : "0"(tmp) : "memory");
    z = S16(obj, 0x32);
    color = 0x68;
    D_800F3454 = color;
    D_800E2284 = color;
    D_800F3304 = color;

    color = 0xA0;
    D_800F3300 = color;
    D_800F3301 = color;
    D_800F3302 = color;

    color = 0x46;
    D_800F3456 = setupValue;
    D_800F3306 = setupValue;

    setupValue = 0x80;
    D_800F3438 = 0;
    D_800F343A = 0;
    D_800F343C = 0;
    D_800F3455 = 0;

    D_800E2268 = y_scale;
    D_800E226A = 0;
    D_800E226C = 0;
    D_800E2285 = 0;
    D_800E2286 = 0;

    D_800F32E8 = y_scale;
    D_800F32EA = 0;
    D_800F32EC = 0;
    D_800F3305 = 0;

    D_800F3388 = y_scale;
    D_800F338A = 0;
    D_800F338C = 0;

    D_800E2294_state = z;
    D_800F33A4 = color;

    color = 0x30;
    D_800F33A5 = color;

    color = -0x50;
    D_800F33A6 = color;

    color = 0x1A8;
    D_800E2828 = color;
    D_800E282C = color;
    D_800E2830 = color;

    color = 0xBC;
    D_800E283C = color;

    color = 8;
    D_800E283D = color;

    color = 0x42;
    D_800F335C = color;

    color = 0x20;
    D_800F335D = color;

    color = 0x32;
    D_800F33A0 = setupValue;
    D_800F33A1 = setupValue;
    D_800F33A2 = setupValue;

    D_800E2820 = 0;
    D_800E2822 = 0;
    D_800E2824 = 0;
    D_800E283E = 0;
    D_800E2838 = setupValue;
    D_800E2839 = setupValue;
    D_800E283A = setupValue;

    D_800F3340 = y_scale;
    D_800F3342 = 0;
    D_800F3344 = 0;
    D_800F335E = color;
    D_800F3358 = setupValue;
    D_800F3359 = setupValue;
    D_800F335A = setupValue;
    return tmp;
}

#include "FieldEng_PointSpriteEffect_externs_1.h"

int func_800CBEE0(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

#include "FieldEng_PointSpriteEffect_externs_2.h"

int func_800CBF0C(void *o) {
    FieldEng_Register(o, D_800E0D08);
    return 0;
}

#include "FieldEng_PointSpriteEffect_externs_3.h"

int func_800CBF34(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0D24);
    second = func_800C2758(obj, D_800E0CEC, D_800E0D40);
    status = first | second;
    if (status == -1) {
        func_800CBFA4(obj);
    }
    return 0;
}

int func_800CBFA4(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CBFB4(void) {
    return 0;
}

int func_800CBFBC(void) {
    return 0;
}

int rand(void);
int *func_800C2B10(int index);

#include "FieldEng_PointSpriteEffect_externs_4.h"

int func_800CBFC4(void *arg0, void *arg1, u8 *anim) {
    int x_base_a1;
    int z_base_a1;
    int x_rand;
    int z_rand;

    D_800F3450 = *func_800C2B10(0);
    D_800F3451 = *func_800C2B10(1);
    D_800F3452 = *func_800C2B10(2);

    x_rand = rand();
    x_base_a1 = D_800E2290.x - 0x64;
    *(u16 *)(anim + 0x6) = x_base_a1 + (x_rand % 201);
    *(u16 *)(anim + 0x8) = D_800E2292;

    z_rand = rand();
    z_base_a1 = D_800E2294 - 0x64;
    *(u16 *)(anim + 0x4) = 0x20C;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z_base_a1 + (z_rand % 201);
}

#include "FieldEng_PointSpriteEffect_externs_5.h"

void FieldAnim_InitRadialEntries(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
    u16 *base_s5;
    volatile int stack_pad;
    int neg_s6;
    int i;
    int angle;
    BattleEntity *entity;
    Combatant *core;
    BattleAction *mode_data;

    D_800E2280 = *func_800C2B10(3);
    D_800E2281 = *func_800C2B10(4);
    D_800E2282 = *func_800C2B10(5);

    anim->scale = 0x7F;
    entity = (BattleEntity *)D_8009D254;
    core = (Combatant *)entity->core;
    mode_data = core->action;
    if (mode_data->actionCode.actionId == 3) {
        anim->count = 8;
    } else {
        anim->count = 0x10;
    }

    if ((s8)anim->count > 0) {
        i = 0;
        base_s5 = &D_800E2290.x;
        neg_s6 = -0x1400;
        do {
            anim->points[i].position.x = base_s5[0];
            anim->points[i].position.y = base_s5[1];
            anim->points[i].position.z = base_s5[2];

            if ((s8)anim->count == 0x10) {
                angle = i << 8;
            } else {
                angle = i << 9;
            }

            anim->velocity[i].x = rsin(angle);
            anim->velocity[i].y = neg_s6;
            anim->velocity[i].z = rcos(angle);
            anim->extent[i] = 0x258;

        } while (++i < (s8)anim->count);
    }
}

int func_800CC244(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290.x;
    *(u16 *)(anim + 8) = D_800E2292;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0xBE8;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}

int func_800CC284(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290.x;
    *(u16 *)(anim + 8) = D_800E2292 - 0x64;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}

void func_800CC2C4(void *arg0, void *arg1, FieldAnimScatteredParticles *state)
{
    int i;

    state->points.scale = 127;
    if (((Combatant *)D_8009D254->core)->action->actionCode.actionId == 3)
        state->points.count = 4;
    else
        state->points.count = 16;

    for (i = 0; i < state->points.count; i++) {
        state->points.x[i] = D_800E2290.x;
        state->points.y[i] = D_800E2290.y;
        state->points.z[i] = D_800E2290.z;
        state->velocity_x[i] = (rand() % 11 - 5) * 256;
        state->velocity_y[i] = -(rand() % 16 + 20) * 256;
        state->velocity_z[i] = (rand() % 11 - 5) * 256;
    }
}

/* Point sprite setup, draw and update callbacks, including radial and
 * scattered particle arrays. Shared payloads are declared in field_anim.h. */

int func_800CC440(void *arg0, void *arg1, u8 *anim) {
    u16 z;

    *(u16 *)(anim + 6) = D_800E2290.x;
    *(u16 *)(anim + 8) = D_800E2292;
    z = D_800E2294;
    *(u16 *)(anim + 4) = 0x224;
    anim[3] = 0x7F;
    *(u16 *)(anim + 0xA) = z;
}

int func_800CC480(void *arg0, FieldEngSlot *params, FieldAnimPointSprite *anim) {
    short *base_a0;
    FieldBillboard *output;
    FieldAnimPointSprite *pointData = anim;
    u32 value_v0;
    int value_v1;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v1 = params->counter;
    value_v0 = 0x80;
    value_v1 <<= 1;
    value_v0 -= value_v1;
    value_v1 = value_v0;
    value_v0 <<= 16;
    if (value_v0 > 0x7FFFFFFFU) {
        value_v1 = 0;
    }

    base_a0 = (short *)&D_800F3430.oriented.brightness;
    base_a0[0] = value_v1;
    value_v0 = pointData->point.x;
    output = (FieldBillboard *)((u8 *)base_a0 -
        PE1_OFFSETOF(FieldBillboard, brightness));
    output->position.x = value_v0;
    D_800F3430.oriented.position.y = pointData->point.y;
    D_800F3430.oriented.position.z = pointData->point.z;
    D_800F3430.oriented.scale.x = pointData->extent;
    D_800F3430.oriented.scale.y = pointData->extent * 2;
    D_800F3430.oriented.scale.z = pointData->extent;
    func_800C3B04(output);
}

void FieldAnim_ProcessInterleavedPoints(void *arg0, void *arg1, FieldAnimRadialParticles *anim) {
    volatile int stack_pad;
    FieldBillboard *out;
    unsigned int i;

    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    D_800E2260.oriented.scale.x = *(s16 *)&anim->extent[0];
    D_800E2260.oriented.scale.y = *(s16 *)&anim->extent[0];
    D_800E2260.oriented.scale.z = *(s16 *)&anim->extent[0];
    D_800E2260.billboard.brightness = anim->scale;

    if (anim->count != 0) {
        i = 0;
        out = &D_800E2260.billboard;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->position.x = anim->points[i].position.x;
                out->position.y = anim->points[i].position.y;
                out->position.z = anim->points[i].position.z;
                func_800C3B04(out);
                i++;
            } while (i < anim->count);
        } while (0);
    }
}

int func_800CC644(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(3);

    value_v0 = anim->point.x;
    output = &D_800F32E0.oriented;
    output->position.x = value_v0;
    D_800F32E0.oriented.position.y = anim->point.y;
    D_800F32E0.oriented.position.z = anim->point.z;
    D_800F32E0.oriented.scale.x = anim->extent;
    D_800F32E0.oriented.scale.y = anim->extent;
    D_800F32E0.oriented.scale.z = anim->extent;
    D_800F32E0.oriented.brightness = (signed char)anim->scale;
    func_800C3324(output);
}

int func_800CC6F8(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldBillboard *output;
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = anim->point.x;
    output = &D_800F3380.billboard;
    output->position.x = value_v0;
    D_800F3380.oriented.position.y = anim->point.y;
    D_800F3380.oriented.position.z = anim->point.z;
    D_800F3380.oriented.scale.x = anim->extent;
    D_800F3380.oriented.scale.y = anim->extent;
    D_800F3380.oriented.scale.z = anim->extent;
    D_800F3380.oriented.brightness = (signed char)anim->scale;
    func_800C3B04(output);
}

void FieldAnim_ProcessPointTriples(void *arg0, void *arg1,
                                   FieldAnimPointData *points) {
    volatile int stack_pad;
    FieldBillboard *out;
    unsigned int i;

    i = 0;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x10, 0x10);
    func_800C3238(0);

    D_800E2818.billboard.brightness = (points->scale << 24) >> 23;

    if (points->count != 0) {
        out = &D_800E2818.billboard;
        /* This one-shot block preserves retail GCC's s1/s2 allocation. */
        do {
            do {
                out->position.x = points->x[i];
                out->position.y = points->y[i];
                out->position.z = points->z[i];
                func_800C3B04(out);
                i++;
            } while (i < points->count);
        } while (0);
    }
}

int func_800CC878(void *arg0, void *arg1, FieldAnimPointSprite *anim) {
    u16 value_v0;
    FieldOrientedSprite *output;
    func_800C2EAC(3);
    func_800C3098(0x100);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = anim->point.x;
    output = &D_800F3338.oriented;
    output->position.x = value_v0;
    D_800F3338.oriented.position.y = anim->point.y;
    D_800F3338.oriented.position.z = anim->point.z;
    D_800F3338.oriented.scale.x = anim->extent;
    D_800F3338.oriented.scale.y = anim->extent;
    D_800F3338.oriented.scale.z = anim->extent;
    D_800F3338.oriented.brightness = (signed char)anim->scale;
    func_800C3324(output);
}

int func_800CC92C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    u16 value;
    register int frame asm("$3");

    value = anim->point.x;
    frame = anim->scale;
    anim->point.x = value;
    value = anim->point.y;
    frame -= 2;
    anim->scale = frame;
    value -= 0xA;
    anim->point.y = value;
    value = (u16)anim->extent;
    frame = (s8)anim->scale;
    frame = frame < 0x1E;
    value += 0x1E;
    anim->extent = value;
    if (frame) {
        state->flag = 2;
    }
}

int func_800CC974(void *arg0, FieldEngSlot *state, FieldAnimRadialParticles *anim) {
    int i;
    FieldEngSlot *state_t2;
    register int velocityValue asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

        state_t2 = state;
    asm volatile("" : "=r"(state_t2) : "0"(state_t2));
    i = 0;
    if ((signed char)anim->count > 0) {
        do {
            register int pos asm("$2");

            velocityValue = anim->velocity[i].x >> 9;
            pos = anim->points[i].position.x;
            dy = anim->velocity[i].y >> 9;
            dz = anim->velocity[i].z >> 9;
            pos += velocityValue;
            anim->points[i].position.x = pos;
            pos = anim->points[i].position.y;
            velocityValue = (u16)anim->velocity[i].y;
            pos += dy;
            anim->points[i].position.y = pos;
            pos = anim->points[i].position.z;
            velocityValue += 0xB4;
            anim->velocity[i].y = velocityValue;
            pos += dz;
            anim->points[i].position.z = pos;
            pos = anim->extent[i];
            pos += 0x18;
            anim->extent[i] = pos;
            i++;
        } while (i < anim->count);
    }

    anim->scale -= 2;
    if (anim->scale < 2) {
        state_t2->flag = 2;
    }
    asm volatile("" : : "r"(&frame));
}

int func_800CCA40(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 2;
    anim->extent = (u16)anim->extent + 0x28;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

int func_800CCA78(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 6;
    anim->extent = (u16)anim->extent + 0xB4;
    if ((s8)anim->scale < 0x1E) {
        state->flag = 2;
    }
}

int func_800CCAB0(void *arg0, FieldEngSlot *state, FieldAnimScatteredParticles *anim) {
    int i;
    FieldEngSlot *state_t1;
    register int velocityValue asm("$3");
    int dy;
    register int dz asm("$5");
    int frame;

    state_t1 = state;
    asm volatile("" : "=r"(state_t1) : "0"(state_t1));
    i = 0;
    if (anim->points.count > 0) {
        do {
            register int pos asm("$2");

            velocityValue = anim->velocity_x[i] >> 8;
            pos = anim->points.x[i];
            dy = anim->velocity_y[i] >> 8;
            dz = anim->velocity_z[i] >> 8;
            pos += velocityValue;
            anim->points.x[i] = pos;
            pos = anim->points.y[i];
            velocityValue = (u16)anim->velocity_y[i];
            pos += dy;
            anim->points.y[i] = pos;
            pos = anim->points.z[i];
            velocityValue += 0xB4;
            anim->velocity_y[i] = velocityValue;
            pos += dz;
            anim->points.z[i] = pos;
            i++;
        } while (i < anim->points.count);
    }

    anim->points.scale -= 2;
    if ((signed char)anim->points.scale < 2) {
        state_t1->flag = 2;
    }
    asm volatile("" : : "r"(&frame));
}

int func_800CCB6C(void *arg0, FieldEngSlot *state, FieldAnimPointSprite *anim) {
    anim->scale -= 8;
    anim->extent = (u16)anim->extent + 0x1A4;
    if ((s8)anim->scale < 0x14) {
        anim->scale = 0;
        state->flag = 2;
    }
}
