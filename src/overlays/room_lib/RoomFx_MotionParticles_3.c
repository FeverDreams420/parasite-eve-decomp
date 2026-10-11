/*
 * The motion particle set, part 3 of 3: the particle motion update and the render pair
 * (init, draw, update).
 *
 * room_m075, room_m080, room_m082, scene_e09 and scene_e10 link the same
 * fifteen functions in the same order, after the room prelude and before
 * the room library, with the same 0x18 bytes of read-only data. The object
 * is split in three units only because its particle draw needs
 * -fno-strength-reduce, which the setup loop must not get; they are
 * merged once they compile together. Trails, glyphs and lookup codes live
 * in each room's own data (pe1/room_motion_init.h).
 */
#include "pe1/room_motion_init.h"
#include "pe1/room_ground_eruption.h"
#include "common.h"
#include "room_lib.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/field_script_context.h"
#include "RoomLib_RenderLayouts.h"
#include "RoomLib_DrawMotion.h"
#include "RoomSharedGlobalState.h"
#include "pe1/room_floor.h"


#include "RoomFx_MotionParticles_3_externs.h"


void RoomFx_UpdateParticleMotion(int arg0, RoomStatePair *control, RoomMotionState *state) {
    unsigned int i;
    RoomStatePair *controlReg = control;
    RoomMotionState *stateReg = state;
    s16 count;
    int amplitude;
    s16 value;
    char scratch[0x18];

    PE1_COMPILER_LAUNDER_MEM(controlReg);

    i = 0;
    if (stateReg->count != 0) {
        do {
            stateReg->lookup[i].id = func_80071A54() % 8;
            stateReg->lookup[i].pad = func_80071A54() % 8;
            count = stateReg->count;
            i++;
        } while (i < count);
    }

    if (controlReg->h2 >= 31) {
        amplitude = (s16)stateReg->depth;
        value = amplitude;
        PE1_COMPILER_LAUNDER(value);
        if (amplitude >= 5) {
            stateReg->depth = value - 4;
        } else {
            stateReg->depth = 0;
        }

        i = 0;
        if (stateReg->count != 0) {
            do {
                stateReg->particle[i].primaryX += stateReg->velocity[i].primaryX;
                stateReg->particle[i].primaryY += stateReg->velocity[i].primaryY;
                stateReg->particle[i].primaryZ += stateReg->velocity[i].primaryZ;
                stateReg->particle[i].secondaryX += stateReg->velocity[i].secondaryX;
                stateReg->particle[i].secondaryY += stateReg->velocity[i].secondaryY;
                stateReg->particle[i].secondaryZ += stateReg->velocity[i].secondaryZ;
                i++;
            } while (i < stateReg->count);
        }
    }

    if ((u32)((u16)controlReg->h2 - 1) < 31) {
        stateReg->depth += 4;
    }
    if (controlReg->h2 == 65) {
        controlReg->b1 = 2;
    }
}

typedef struct RoomParticleRenderRecord {
    void *packet;
    s16 field04;
    s16 field06;
    s16 field08;
    s16 field0A;
    s16 field0C;
    u8 pad0E[2];
    u8 field10;
    u8 field11;
    u8 field12;
    u8 pad13;
    RoomSpriteMatrix matrix;
} RoomParticleRenderRecord;

typedef struct RoomParticleRenderPairState {
    u8 pad00[0x78];
    RoomParticleRenderRecord first;
    RoomParticleRenderRecord second;
} RoomParticleRenderPairState;


void func_800C5538(RoomParticleRenderRecord *record);

/* The effect owner: its actor (whose joint transforms place the two
 * records), the hit flag the update raises and the texture page. */
typedef struct RoomParticlePairOwner {
    RoomFxTransformOwner *actor;  /* 0x00 */
    u8 pad04[0x64];
    s16 hit;                      /* 0x68 */
    s16 pad6A;
    u8 page;                      /* 0x6C */
} RoomParticlePairOwner;

void RoomFx_InitParticleRenderPair(
    void *owner, void *unused, RoomParticleRenderPairState *state) {
    RoomSpriteMatrix secondMatrix;
    RoomSpriteMatrix firstMatrix;
    void *callOwner;
    RoomParticleRenderPairState *workState;
    RoomParticlePairOwner *root;

    callOwner = owner;
    workState = state;
    root = func_800C2B50();
    firstMatrix = *(RoomSpriteMatrix *)root->actor->transforms;
    secondMatrix = *(RoomSpriteMatrix *)root->actor->transforms;

    firstMatrix.t[0] = root->actor->transforms[9].x;
    firstMatrix.t[1] = g_RoomFloorY->y;
    firstMatrix.t[2] = root->actor->transforms[9].z;
    secondMatrix.t[0] = root->actor->transforms[13].x;
    secondMatrix.t[1] = g_RoomFloorY->y;
    secondMatrix.t[2] = root->actor->transforms[13].z;

    workState->second.field04 = 10;
    workState->second.packet = g_RoomMotionTrailB;
    workState->second.field06 = 0;
    workState->second.field08 = 400;
    workState->second.field0A = 300;
    workState->second.field0C = 2;
    workState->second.matrix = secondMatrix;
    workState->second.field10 = 32;
    workState->second.field11 = 1;
    workState->second.field12 = 1;
    func_800C5538(&workState->second);

    workState->first.field04 = 10;
    workState->first.packet = g_RoomMotionTrailA;
    workState->first.field06 = 0;
    workState->first.field08 = 400;
    workState->first.field0A = 300;
    workState->first.field0C = 2;
    workState->first.matrix = firstMatrix;
    workState->first.field10 = 32;
    workState->first.field11 = 1;
    workState->first.field12 = 1;
    func_800C5538(&workState->first);

    func_800C66C8(callOwner, 0x576, &firstMatrix);
}

#include "RoomFx_MotionParticles_3_externs_1.h"

void RoomFx_DrawParticleRenderPair(void *arg0, void *arg1, char *sys) {
    char *sysp = sys;
    RoomParticlePairOwner *root;

    root = func_800C2B50();
    func_800C2EAC(root->page);
    func_800C2FF0(0x10, 0x40);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C5A40(sysp + 0xAC);
    func_800C5A40(sysp + 0x78);
}

#include "RoomFx_MotionParticles_3_externs_2.h"

void RoomFx_UpdateParticleRenderPair(void *arg0, char *state, char *sys) {
    char *statep = state;
    char *sysp = sys;
    int *resultp;
    RoomParticlePairOwner *root;
    RoomSharedGlobalState *global;
    int value0;
    int value1;
    int value2;
    s16 values[3];
    int result;
    int ret;

    root = func_800C2B50();
    resultp = &result;
    global = (RoomSharedGlobalState *)D_8009D254;
    value0 = global->field2A;
    values[0] = value0;
    value1 = global->field2A;
    values[1] = value1;
    value2 = global->field2A;
    values[2] = value2;

    func_800C5EB0(sysp + 0xAC, values, resultp);
    ret = func_800C5EB0(sysp + 0x78, values, resultp);

    if (result == 1) {
        root->hit = 1;
    }

    if (ret == 1) {
        statep[1] = 2;
    }
}
