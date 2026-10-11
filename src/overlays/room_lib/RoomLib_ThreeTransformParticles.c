/*
 * Three transform particles: a sprite class whose Init seeds the sprite and
 * floor-shadow packet templates, whose effect owner places three particles,
 * draws them with their shadows and lets them fall as debris. The eight
 * functions, from the two no-ops after RoomLib_ThreeParticleClose to the
 * debris update, are the same run in eleven Central Park and Chrysler
 * rooms; the unit's rodata is the draw pass's rotation seed, the eight
 * bytes after each room's twelve-byte rodata header.
 */
#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/room_effect_state.h"
#include "pe1/room_debris_triplet.h"
#include "pe1/field_script_context.h"
#include "pe1/gte_short_vector.h"
#include "pe1/room_floor.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_sprite_state.h"
#include "pe1/floor_polygon.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_libc.h"
#include "pe1/pe_image.h"
#include "pe1/field_engine_calls.h"

extern void func_800C2B40(void *arg0);

int *func_800C2B28(int index);

/* Rotation seed of the draw pass, copied into its stack frame. */
static const RoomFxSeed8 s_ThreeParticleSeed = {
    {0x04, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/* The effect owner state: the owner's actor, a copy of its view matrix
 * (the translation is where the particles start), the reload timer, the
 * landed flag and the sprite asset whose low byte selects the page. */
typedef struct RoomDebrisClock {
    struct RoomDebrisActor *actor;    /* 0x00 */
    union {
        RoomEffectWords8 words;
        GteMatrix matrix;
    } view;                           /* 0x04 */
    unsigned short reload;            /* 0x24 */
    s16 cooldown;                     /* 0x26 */
    s16 landedFlag;                   /* 0x28 */
    s16 pad2A;
    union {
        int index;
        u8 page;
    } asset;                          /* 0x2C */
} RoomDebrisClock;

/* The owner's actor: its view matrices. */
typedef struct RoomDebrisActor {
    u8 pad000[0x238];
    RoomEffectWords8 *views;          /* 0x238 */
} RoomDebrisActor;

typedef struct RoomDebrisEntity {
    u8 pad00[0x8];
    RoomDebrisActor *actor;           /* 0x08 */
} RoomDebrisEntity;

int RoomLib_ThreeParticleNop0(void) {
    return 0;
}

int RoomLib_ThreeParticleNop1(void) {
    return 0;
}

void RoomLib_InitThreeParticlePackets(RoomDebrisEntity *arg0, void *arg1, RoomDebrisClock *state) {
    RoomEffectWords8 *src;
    RoomDebrisActor *table;

    func_800C2B40(state);
    state->asset.index = Asset_SearchByKeyType(0xA);
    table = arg0->actor;
    state->actor = table;
    src = table->views;
    state->view.words = *src;
    state->cooldown = 0;
    state->landedFlag = 0;
    state->reload = *func_800C2B28(6);

    RoomLib_ThreeParticlePacket.offset = -0x64;
    RoomLib_ThreeParticleFloorPacket.code = 4;
    RoomLib_ThreeParticleFloorPacket.mode = 1;
    RoomLib_ThreeParticlePacket.code = 0;
    RoomLib_ThreeParticlePacket.mode = 0;
    RoomLib_ThreeParticlePacket.depth = 0x80;
    RoomLib_ThreeParticlePacket.r = 0x80;
    RoomLib_ThreeParticlePacket.g = 0x80;
    RoomLib_ThreeParticlePacket.b = 0x80;
    RoomLib_ThreeParticlePacket.zero = 0;
    RoomLib_ThreeParticleFloorPacket.offset = 0x32;
    RoomLib_ThreeParticleFloorPacket.depth = 0x80;
    RoomLib_ThreeParticleFloorPacket.r = 0x80;
    RoomLib_ThreeParticleFloorPacket.g = 0x80;
    RoomLib_ThreeParticleFloorPacket.b = 0x80;
    RoomLib_ThreeParticleFloorPacket.zero = 0;
}

void RoomLib_ThreeParticleNop3(void) {
}


void RoomLib_UpdateThreeParticleTimer(int arg0, char *arg1, char *arg2) {
    RoomDebrisClock *state = (RoomDebrisClock *)arg2;

    if (state->cooldown != 0) {
        state->cooldown--;
    }

    if (state->landedFlag == 1) {
        int timer = state->cooldown;

        state->landedFlag = 0;
        if (timer == 0) {
            state->cooldown = state->reload;
            func_800C6C18(arg0);
        }
    }

    if (func_800C2B68() == 1) {
        arg1[1] = 2;
    }
}

typedef GteShortVector RoomInitThreeVec;

typedef struct RoomInitThreeStack {
    RoomInitThreeVec projectionSeed;
    RoomInitThreeVec firstSeed;
    RoomInitThreeVec secondSeed;
    RoomSpriteMatrix firstMatrix;
    RoomSpriteMatrix secondMatrix;
} RoomInitThreeStack;

int *func_800C2B10(int index);
int func_80071A54(void);
int func_800C66C8(void *object, int message, void *state);

/* Retail marks this function handwritten; only its COP2 windows remain ASM. */
void RoomLib_InitThreeTransformParticles(void *owner, void *unused,
                                         char *state) {
    RoomInitThreeStack stack;
    RoomSpriteMatrix *firstMatrix;
    register RoomSpriteMatrix *secondMatrix asm("$17");
    RoomInitThreeVec *seed;
    void *callOwner;
    register RoomDebrisTriplet *workState asm("$21");
    char *clock;
    int i;
    char *projectionMatrix;
    register char *positionCursor asm("$17");
    register char *velocityCursor asm("$18");
    register char *timerCursor asm("$19");
    int parameterIndex;
    char *clockResult;
    int *value;
    int randomValue;

    callOwner = owner;
    workState = (RoomDebrisTriplet *)state;
    clockResult = func_800C2B50();
    parameterIndex = 3;
    clock = clockResult;
        stack.firstSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.firstSeed.y = -*value;
    stack.firstSeed.z = 0;
    seed = &stack.firstSeed;
    asm("" : : "r"(seed));
    firstMatrix = &stack.firstMatrix;
    RotMatrixYXZ(seed, firstMatrix);

    parameterIndex = 3;
    stack.secondSeed.x = 0;
    value = func_800C2B28(parameterIndex);
    stack.secondSeed.y = *value;
    stack.secondSeed.z = 0;
    seed = &stack.secondSeed;
    asm("" : : "r"(seed));
    secondMatrix = &stack.secondMatrix;
    RotMatrixYXZ(seed, secondMatrix);

    gte_ldrotmatrix(firstMatrix);
    gte_ldrtir12_matrix_column(clock + 4);
    gte_stir123_column(firstMatrix);
    gte_ldrtir12_matrix_column(clock + 6);
    gte_stir123_column_at(&stack.firstMatrix.m[0][1]);
    gte_ldrtir12_matrix_column(clock + 8);
    gte_stir123_column_at(&stack.firstMatrix.m[0][2]);
    gte_ldtransmatrix(firstMatrix);
    {
        unsigned short *translation = (unsigned short *)(clock + 0x18);
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(stack.firstMatrix.t);
        gte_swc2_10_4(stack.firstMatrix.t);
        gte_swc2_11_8(stack.firstMatrix.t);

        gte_ldrotmatrix(secondMatrix);
        gte_ldrtir12_matrix_column(clock + 4);
        gte_stir123_column(secondMatrix);
        gte_ldrtir12_matrix_column(clock + 6);
        gte_stir123_column_at(&stack.secondMatrix.m[0][1]);
        gte_ldrtir12_matrix_column(clock + 8);
        gte_stir123_column_at(&stack.secondMatrix.m[0][2]);
        gte_ldtransmatrix(secondMatrix);
        gte_ldv0_word3(translation);
        gte_cop2_hazard_slot();
        gte_cop2_hazard_slot();
        gte_rtv0tr_sf0();
        gte_swc2_9_0(stack.secondMatrix.t);
        gte_swc2_10_4(stack.secondMatrix.t);
        gte_swc2_11_8(stack.secondMatrix.t);
    }

    i = 0;
    velocityCursor = (char *)workState;
    positionCursor = (char *)workState;
    timerCursor = (char *)workState;
    for (; i < 3; i++) {
        randomValue = func_80071A54();
        workState->size[i] = 0;
        workState->countdown[i] = (randomValue % 10) + 1;
        ((RoomDebrisTriplet *)timerCursor)->life[0] = *func_800C2B28(2);
        ((RoomDebrisTriplet *)positionCursor)->position[0].x = ((RoomDebrisClock *)clock)->view.matrix.t[0] << 16;
        ((RoomDebrisTriplet *)positionCursor)->position[0].y = ((RoomDebrisClock *)clock)->view.matrix.t[1] << 16;
        ((RoomDebrisTriplet *)positionCursor)->position[0].z = ((RoomDebrisClock *)clock)->view.matrix.t[2] << 16;
        ((RoomDebrisTriplet *)velocityCursor)->rotation[0].x = 0;
        ((RoomDebrisTriplet *)velocityCursor)->rotation[0].y = 0;
        ((RoomDebrisTriplet *)velocityCursor)->rotation[0].z = 0;
        ((RoomDebrisTriplet *)timerCursor)->swing[0] = 0;
        workState->mode[i] = 1;
        workState->active[i] = 0;
        workState->landed[i] = 0;
        velocityCursor += 8;
        positionCursor += 0x10;
        timerCursor += 2;
    }

    workState->remaining = 3;
    workState->speed = *func_800C2B28(0);
    workState->acceleration = *func_800C2B28(1);
    workState->swingLimit = *func_800C2B28(5);
    workState->keepAlive = *func_800C2B28(4);
    workState->reserved08 = *func_800C2B10(1);

    stack.projectionSeed.x = 0;
    stack.projectionSeed.y = 0;
    stack.projectionSeed.z = 0x1000;
    ApplyMatrixSV(&stack.firstMatrix, &stack.projectionSeed,
                  (GteShortVector *)&workState->velocity[0]);
    projectionMatrix = clock + 4;
    ApplyMatrixSV((RoomSpriteMatrix *)projectionMatrix, &stack.projectionSeed,
                  (GteShortVector *)&workState->velocity[1]);
    ApplyMatrixSV(&stack.secondMatrix, &stack.projectionSeed,
                  (GteShortVector *)&workState->velocity[2]);
    func_800C66C8(callOwner, 0x573, projectionMatrix);
}

typedef GteShortVector RoomDrawThreeVec;

typedef struct RoomDrawThreeStack {
    RoomSpriteMatrix primaryMatrix;
    RoomSpriteMatrix secondaryMatrix;
    RoomFxSeed8 seed;
    RoomDrawThreeVec position;
    RoomFxVec4 scale;
    RoomFxVec4 sourceScale;
    RoomFxVec4 floorScale;
    RoomFxVec4 sourceFloorScale;
    s16 *args;
} RoomDrawThreeStack;

void func_800C3098(int depth);
void func_800C2FF0(int width, int height);

void RoomLib_DrawThreeTransformParticles(void *unused, s16 *args, char *state) {
    RoomDrawThreeStack stack;
    register RoomDebrisTriplet *workState asm("$18");
    unsigned int i;
    RoomFxVec4 *sourceScale;
    RoomSpriteMatrix *secondaryMatrix;
    register char *positionCursor asm("$19");
    register char *shortCursor asm("$17");
    s16 scaleValue;
    void *arg0;
    RoomSpriteMatrix *inputArg;
    int frame;
    register s16 *callArgs asm("$7");
    RoomDebrisClock *clock;
    int computedScale;
    int alpha;
    int baseScale;

    workState = (RoomDebrisTriplet *)state;
    stack.args = args;
    clock = func_800C2B50();
    stack.seed = s_ThreeParticleSeed;
    i = 0;
    sourceScale = &stack.sourceScale;
    secondaryMatrix = &stack.secondaryMatrix;
    positionCursor = (char *)workState;
    shortCursor = (char *)workState;
    func_800C2EAC(clock->asset.page);
    func_800C3098(0x10);

    for (; i < 3; i++) {
        if (workState->active[i] >= 0) {
            arg0 = &workState->rotation[i];
            RotMatrixYXZ(arg0, &stack.primaryMatrix);
            memset(sourceScale, 0, 0x10);
            stack.sourceScale.x = ((RoomDebrisTriplet *)shortCursor)->swing[0] >> 1;
            stack.sourceScale.y = ((RoomDebrisTriplet *)shortCursor)->swing[0] >> 1;
            stack.sourceScale.z = ((RoomDebrisTriplet *)shortCursor)->swing[0] >> 1;
            stack.scale = stack.sourceScale;
            ScaleMatrix(&stack.primaryMatrix, &stack.scale);

            stack.primaryMatrix.t[0] = (((RoomDebrisTriplet *)positionCursor)->position[0].x >> 16) +
                                       workState->jitter[0];
            stack.primaryMatrix.t[1] = (((RoomDebrisTriplet *)positionCursor)->position[0].y >> 16) +
                                       workState->jitter[1];
            stack.primaryMatrix.t[2] = (((RoomDebrisTriplet *)positionCursor)->position[0].z >> 16) +
                                       workState->jitter[2];
            callArgs = stack.args;
            alpha = workState->size[i];
            arg0 = RoomLib_ThreeParticleTable;
            RoomLib_ThreeParticleAlpha = alpha;
            asm volatile("" ::: "memory");
            frame = callArgs[1];
            func_800C3134(arg0, frame, (unsigned char *)&RoomLib_ThreeParticlePacket);
            func_800C2FF0(0x40, 0x40);
            func_800C3238(2);
            func_800C42A4((FieldGlowSprite *)&RoomLib_ThreeParticlePacket,
                          &stack.primaryMatrix, 1);

            if (workState->reserved08 == 1) {
                callArgs = stack.args;
                computedScale = rsin(callArgs[1] << 7) >> 3;
                baseScale = ((RoomDebrisTriplet *)shortCursor)->phase[0];
                computedScale += baseScale;
                scaleValue = computedScale;
                if (((RoomDebrisTriplet *)shortCursor)->swing[0] < (s16)computedScale) {
                    scaleValue = ((RoomDebrisTriplet *)shortCursor)->swing[0];
                }

                stack.position = workState->rotation[i];
                arg0 = &stack.position;
                inputArg = &stack.primaryMatrix;
                asm("" : "=r"(arg0), "=r"(inputArg)
                    : "0"(arg0), "1"(inputArg));
                scaleValue = (s16)scaleValue >> 1;
                stack.position.z += ((RoomDebrisTriplet *)shortCursor)->life[0] << 4;
                RotMatrixYXZ(arg0, inputArg);
                memset(sourceScale, 0, 0x10);
                stack.sourceScale.x = scaleValue;
                stack.sourceScale.y = scaleValue;
                stack.sourceScale.z = scaleValue;
                ScaleMatrix(&stack.primaryMatrix, sourceScale);
                stack.primaryMatrix.t[0] = (((RoomDebrisTriplet *)positionCursor)->position[0].x >> 16) +
                                           workState->jitter[0];
                stack.primaryMatrix.t[1] = (((RoomDebrisTriplet *)positionCursor)->position[0].y >> 16) +
                                           workState->jitter[1];
                stack.primaryMatrix.t[2] = (((RoomDebrisTriplet *)positionCursor)->position[0].z >> 16) +
                                           workState->jitter[2];
                func_800C42A4((FieldGlowSprite *)&RoomLib_ThreeParticlePacket,
                              &stack.primaryMatrix, 1);

                RotMatrix(&stack.seed.vector, secondaryMatrix);
                memset(&stack.sourceFloorScale, 0, 0x10);
                stack.sourceFloorScale.x = ((RoomDebrisTriplet *)shortCursor)->swing[0];
                stack.sourceFloorScale.y = ((RoomDebrisTriplet *)shortCursor)->swing[0];
                stack.sourceFloorScale.z = ((RoomDebrisTriplet *)shortCursor)->swing[0];
                stack.floorScale = stack.sourceFloorScale;
                ScaleMatrix(secondaryMatrix, &stack.floorScale);
                computedScale = (((RoomDebrisTriplet *)positionCursor)->position[0].x >> 16);
                baseScale = g_RoomFloorY->y;
                stack.secondaryMatrix.t[0] = computedScale;
                stack.secondaryMatrix.t[1] = baseScale;
                computedScale = (((RoomDebrisTriplet *)positionCursor)->position[0].z >> 16);
                
                stack.secondaryMatrix.t[2] = computedScale;
                alpha = workState->size[i];
                RoomLib_ThreeParticleFloorPacket.depth = (unsigned int)alpha >> 1;
                func_800C2FF0(0x20, 0x20);
                func_800C3238(3);
                func_800C42A4((FieldGlowSprite *)&RoomLib_ThreeParticleFloorPacket,
                              secondaryMatrix, 0);
            }
        }
        positionCursor += 0x10;
        shortCursor += 2;
    }
}

typedef struct RoomDebrisOwner {
    u8 reserved00;
    u8 phase;                     /* 0x01 */
    s16 angle;                    /* 0x02 */
} RoomDebrisOwner;

PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, position) == 0x3C,
                  room_debris_triplet_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomDebrisTriplet, velocity) == 0x9C,
                  room_debris_triplet_velocity_offset);

extern int func_80071A54(void);

void RoomLib_UpdateDebrisTriplet(void *unused, RoomDebrisOwner *owner,
                                        RoomDebrisTriplet *state) {
    s16 point[4];
    RoomDebrisClock *clock;
    unsigned int i;
    int step;

    clock = func_800C2B50();
    for (i = 0; i < 3; i++) {
        if (state->countdown[i] != 0) {
            state->countdown[i]--;
            if (state->countdown[i] == 0) state->active[i] = 0;
        }
        if (state->active[i] == 0) {
            if (state->size[i] < 0x80) {
                state->size[i] += 4;
            } else {
                state->active[i] = 1;
            }
        }
        if (state->mode[i] == 1) {
            state->position[i].x += (state->velocity[i].x * state->speed) >> 4;
            state->position[i].y += (state->velocity[i].y * state->speed) >> 4;
            state->position[i].z += (state->velocity[i].z * state->speed) >> 4;
        }
        if (state->swing[i] < state->swingLimit) {
            state->swing[i] += func_80071A54() % 80 + 20;
            state->phase[i] += func_80071A54() % 40 + 40;
            state->position[i].y += -0xA0000;
        }
        state->swing[i] += rsin(owner->angle << 5) >> 7;
        if (state->life[i] < 0x14) {
            if (state->size[i] >= 7) state->size[i] -= 6;
        }
        if (state->landed[i] == 0) {
            if (!Geo_PointInPoly((state->position[i].x >> 16) << 16,
                               (state->position[i].z >> 16) << 16,
                               D_8009D248, D_8009D1CC)) {
                state->landed[i] = 1;
                state->mode[i] = 0;
                if (state->keepAlive == 0) state->life[i] = 0x14;
            }
        }
        state->life[i]--;
        if (state->life[i] == 0) {
            state->active[i] = -1;
            state->remaining--;
        }
        point[0] = (state->position[i].x >> 16);
        point[1] = (state->position[i].y >> 16);
        point[2] = (state->position[i].z >> 16);
        state->jitter[0] = func_80071A54() % 11 - 5;
        state->jitter[1] = func_80071A54() % 11 - 5;
        state->jitter[2] = func_80071A54() % 11 - 5;
        if (func_800C6B90(point, state->swing[i] >> 4)) {
            if (state->landed[i] == 0) {
                clock->landedFlag = 1;
                state->life[i] = 0x14;
                state->mode[i] = 0;
            }
            state->landed[i] = 1;
        }
    }
    state->speed += state->acceleration;
    if ((s16)state->speed < 0) state->speed = 0;
    if (state->remaining == 0) owner->phase = 2;
}
