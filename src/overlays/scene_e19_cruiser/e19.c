#include "common.h"
#include "pe1/scene_grab_trigger.h"
#include "pe1/scene_grab_head.h"
#include "pe1/scene_e19_selection.h"
#include "scene_e19_shared.h"
#include "pe1/geom_state.h"
#include "pe1/field_sprite_params.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"
#include "pe1/scene_e19_blast.h"
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/fieldfx_task.h"
#include "pe1/field_actor.h"
#include "pe1/overlay_math.h"
#include "pe1/random.h"
#include "pe1/room_floor.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/scene_e19_2_homing_burst.h"
#include "pe1/scene_e19_2_spin_ray.h"
#include "pe1/scene_e19_2_ember.h"
#include "pe1/scene_e19_2_twin_glow.h"
#include "scene_e19_state.h"

typedef struct SceneE19FrameGateLink {
    u8 reserved_00[0x16];
    u16 winLo;
} SceneE19FrameGateLink;

typedef struct SceneE19FrameGateEntity {
    u8 reserved_00[8];
    SceneE19FrameGateLink *link;
} SceneE19FrameGateEntity;

typedef struct SceneE19Effect {
    u16 x, y, z, pad;
    s16 state;
    s16 frame;
} SceneE19Effect;

/* Full callback reconstruction; source-level register constraints are tracked
 * in crutch debt because stock GCC needs them to reproduce the retail code. */
typedef struct SceneE19HomingParticle {
    GteShortVector position; /* position.pad is the particle index */
    GteShortVector angles;
    GteShortVector target;
    s16 speed;
    s16 turnPhase;
    s16 state;
    s16 timer;
} SceneE19HomingParticle;

#include "e19_externs.h"

void RoomLib_GrabOnApproach(SceneGrabHead *head) {
    SceneGrabTrigger *trigger = (SceneGrabTrigger *)head;
    FieldActor *actor = trigger->actor;
    SceneGrabControl *control = &trigger->control;
    FieldActorState *state = actor->state;

    if (state != 0 && state->control10.command_value <= 1000000) {
        *trigger->control.signal = 4;
        func_80192D04((char *)trigger);
        return;
    }
    if (g_PlayerEntity->mode < 18) {
        int frame = actor->anim.parts.integer;

        if (frame < 4) {
            return;
        }
        if (frame < 12) {
            SceneGrabPoint *target = (SceneGrabPoint *)0x1F800008;

            target->x = actor->render_object.matrices[11].translation[0] << 16;
            target->z = actor->render_object.matrices[11].translation[2] << 16;
            if (func_800DFE20((Vec3Fixed *)&g_PlayerEntity->pos_x, (Vec3Fixed *)&target->x) < 0x140) {
                FieldActor *player;

                control->callback = func_80192878;
                func_80020C74();
                player = g_PlayerEntity;
                player->render_object.animation_source = &actor->render_object;
                player->render_object.animation_state = 4;
                player->render_object.animation_id = 11;
                player->flags |= 0x10000;
                player->render_object.flags_9C |= 0x400;
                player->render_object.model_matrix = control->player_matrix;
                control->grabbed = 1;
                {
                    FieldActor *mover = g_PlayerEntity;

                    control->saved_x = mover->pos_x;
                    control->saved_z = mover->pos_z;
                }
            }
            return;
        }
    }
    *control->signal = 3;
    control->callback = SceneE19_RunAfterFrame37;
}

/* Grab sequence driver: pins the player to the selection matrix and steps
 * the partner's animation events. Both frame numbers are the integer halves
 * of the 16.16 positions taken with an unsigned shift, which gives retail's
 * lhu loads and the copies cse keeps for the later cases. */

void func_80192878(SceneE19Selection *selection) {
    FieldActor *actor = selection->actor;
    SceneE19SelectionTail *tail = &selection->tail;
    FieldActorState *playerState;
    SceneE19ActorState *actorState;
    u16 currentFrame;
    u16 previousFrame;

    g_PlayerEntity->render_object.model_matrix = selection->matrix;
    currentFrame = (unsigned int)actor->anim.fixed >> 16;
    previousFrame = (unsigned int)actor->anim_prev >> 16;
    g_PlayerEntity->pos_x = selection->savedX;
    g_PlayerEntity->pos_z = selection->savedZ;
    if (g_PlayerEntity->state == 0 || g_PlayerEntity->state->amount <= 0) {
        func_80192B10(actor, tail);
        func_80192BFC(selection);
        return;
    }

    switch (actor->mode) {
    case 16:
        if ((s16)currentFrame >= 11 && (s16)previousFrame < 11) {
            actor->anim.fixed = 0x280000;
        } else if ((s16)currentFrame >= actor->action - 1) {
            *tail->signal = 2;
        }
        return;
    case 13:
        actorState = (SceneE19ActorState *)actor->state;
        if (actorState != 0 && *actorState->substate == 1) {
            *actorState->substate = 2;
        }
        if ((s16)previousFrame < 18 && (s16)currentFrame >= 18) {
            func_80020CE4();
            func_80192B10(actor, tail);
            return;
        }
        if ((s16)previousFrame < 3 && (s16)currentFrame >= 3) {
            playerState = g_PlayerEntity->state;
            if (playerState != 0 && playerState->amount > 0) {
                playerState->flags |= 0x4000;
                if (actor->state != 0) {
                    actor->state->core_flags |= 0x80000000;
                }
            }
            return;
        }
        if ((s16)currentFrame < actor->action - 1) {
            return;
        }
        actorState = (SceneE19ActorState *)actor->state;
        if (actorState != 0) {
            *actorState->substate = 4;
        }
        func_80192BFC(selection);
        return;
    case 12:
        return;
    default:
        func_80020CE4();
        func_80192B10(actor, tail);
        func_80192BFC(selection);
        return;
    }
}

s32 func_8001AA78();

void func_80192B10(void *arg0, unsigned char *state) {
    {
        SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;

        actor->reset_word = 0;
        actor->reset_halfword = 0;
        actor->flags &= 0xFFFEFFFF;
        actor->h250 &= 0xFBFF;
        func_8001AA78(actor);
    }

    {
        SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;
        int x = actor->pos[0];
        int y = actor->pos[1];
        int z = actor->pos[2];
        int *flags = (int *)&D_800BCF88;

        actor->vel[0] = 0;
        actor->vel[1] = 0;
        actor->vel[2] = 0;
        actor->move[0] = 0;
        actor->move[1] = 0;
        actor->move[2] = 0;
        actor->base_pos[0] = x;
        actor->base_pos[1] = y;
        actor->base_pos[2] = z;
        *flags |= 0x80;
    }

    state[0x38] = 0;
}

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19FrameGateLink, winLo) == 0x16,
                  scene_e19_frame_gate_window_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19FrameGateLink) == 0x18,
                  scene_e19_frame_gate_link_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19FrameGateEntity, link) == 8,
                  scene_e19_frame_gate_entity_link_offset);
PE1_STATIC_ASSERT(sizeof(SceneE19FrameGateEntity) == 0x0C,
                  scene_e19_frame_gate_entity_size);

void SceneE19_RunAfterFrame37(SceneE19FrameGateEntity *o)
{
    if (o->link->winLo >= 0x26) {
        func_80192BFC();
    }
}

int func_80192BFC(SceneE19ResetTarget *ent) {
    unsigned char *state;

    *ent->signal = 0;
    ent->state = 4;
    state = (unsigned char *)ent + 0xC;
    if (ent->enabled != 0) {
        {
            SceneE19ActorChild *child = ((SceneE19Actor *)D_8009D254)->child;

            if (child != 0 && child->h0C > 0) {
                func_80020CE4();
            }
        }

        {
            SceneE19Actor *actor = (SceneE19Actor *)D_8009D254;

            actor->reset_word = 0;
            actor->reset_halfword = 0;
            actor->flags &= 0xFFFEFFFF;
            actor->h250 &= 0xFBFF;
            func_8001AA78(actor);
        }

        {
            volatile SceneE19Actor *actor;
            int x;
            int y;
            int z;

            actor = (SceneE19Actor *)D_8009D254;
            x = actor->pos[0];
            y = actor->pos[1];
            z = actor->pos[2];
            actor->vel[0] = 0;
            actor->vel[1] = 0;
            actor->vel[2] = 0;
            actor->move[0] = 0;
            actor->move[1] = 0;
            actor->move[2] = 0;
            actor->base_pos[0] = x;
            actor->base_pos[1] = y;
            actor->base_pos[2] = z;
            D_800BCF88 |= 0x80;
            state[0x38] = 0;
        }
    }

    return 0;
}

s32 func_80192D04(char *obj)
{
  s32 *out;
  char *sub = obj + 0xC;
  char *e;
  SceneE19ResetTarget *target = (SceneE19ResetTarget *)obj;
  target->state = 4;
  if (target->enabled != 0)
  {
    SceneE19ActorChild *node = ((SceneE19Actor *)D_8009D254)->child;
    if (node != 0)
    {
      if (node->h0C > 0)
      {
        func_80020CE4();
      }
    }
    ;
    ((SceneE19Actor *)D_8009D254)->flags &= 0xFFFEFFFF;
    ((SceneE19Actor *)D_8009D254)->h250 &= 0xFBFF;
    ((SceneE19Actor *)D_8009D254)->reset_word = 0;
    ((SceneE19Actor *)D_8009D254)->reset_halfword = 0;
    func_8001AA78();
    e = (char *)D_8009D254;
    *((s32 *) (e + 0x68)) = 0;
    *((s32 *) (e + 0x6C)) = 0;
    *((s32 *) (e + 0x70)) = 0;
    *((s32 *) (e + 0x78)) = 0;
    *((s32 *) (e + 0x7C)) = 0;
    out = (s32 *) (e + 0x40);
    *((s32 *) (e + 0x80)) = 0;
    *out = *((s32 *) (e + 0x28));
    *((s32 *) (e + 0x44)) = *((s32 *) (e + 0x2C));
    *((s32 *) (e + 0x48)) = *((s32 *) (e + 0x30));
    D_800BCF88 |= 0x80;
    sub[0x38] = 0;
  }
  return 0;
}

int func_80192E00(void) {
    return 0;
}

int func_80192E08(int mode, SceneE19Effect *effect) {
    GteRotation position = D_8018F1CC;
    int angle, cosine, sine, brightness, palette, kind;
    u16 clut;

    switch (mode) {
    case 1: {
        if (effect->state != 0) return 0;
        effect->frame++;
        effect->y -= 128;
        if (effect->frame >= 8) return 1;
        break;
    }
    case 2: {
        if (effect->state != 0) return 0;
        position.z = 1024;
        angle = effect->frame << 7;
        cosine = rsin(angle);
        sine = rcos(angle);
        brightness = (D_8019B668 * sine) / 4096;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(144, palette);
        func_800CEE20((GteShortVector *)effect, &position, 4096, cosine, 0x88, clut, 1, brightness, 0);
        break;
    }
    }
    return 0;
}

int func_80192F9C(int mode, SceneE19Blast *blast)
{
    GteShortVector center;
    GteShortVector ringPoint;
    GteRotation flareRotation = { 0x400, 0, 0, 1 };
    GteShortVector rotation;
    RenderColor glowColor = { 0x8C, 0x8C, 0x46, 0 };
    RenderColor flareColor = { 0x8C, 0x46, 0x8C, 0 };
    GteShortVector floorGlow0;
    GteShortVector floorRotation0;
    GteShortVector floorGlow1;
    GteShortVector floorRotation1;
    GteMatrix shellMatrix1;
    GteVector shellScale1;
    GteShortVector floorGlow2;
    GteShortVector floorRotation2;
    GteMatrix shellMatrix2;
    GteVector shellScale2;
    GteMatrix shellMatrix3a;
    GteVector shellScale3a;
    GteMatrix shellMatrix3b;
    GteVector shellScale3b;
    GteMatrix shellMatrix3c;
    GteVector shellScale3c;
    GteMatrix shellMatrix3d;
    GteVector shellScale3d;
    GteMatrix shellMatrix3e;
    GteVector shellScale3e;
    GteMatrix shellMatrix4a;
    GteVector shellScale4a;
    GteMatrix shellMatrix4b;
    GteVector shellScale4b;
    GteMatrix shellMatrix4c;
    GteVector shellScale4c;
    GteMatrix shellMatrix4d;
    GteVector shellScale4d;
    SceneE19BlastSpark *spark;
    SceneE19BlastActor *actor;
    u16 *pageSelector;
    RenderEffectParameters *params;
    u16 *palettes;
    GteShortVector *target;
    int spread;
    int phase;
    int modelPhase;
    int intensity;
    int radialScale;
    int verticalScale;
    int page;
    int palette;
    int i;
    int value;
    int height;
    s16 timer;

    switch (mode) {
    case 0:
        D_8019B680 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC54C0704);
        FieldFx_ModelScrollUv(D_8019B680, 0, 0);
        D_8019B684 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC58C0704);
        FieldFx_ModelScrollUv(D_8019B684, 0, 0);
        D_8019B688 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5CC0704);
        FieldFx_ModelScrollUv(D_8019B688, 0, 0);
        D_8019B68C = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC60C0704);
        FieldFx_ModelScrollUv(D_8019B68C, 0, 0);
        D_8019B690 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC64C0704);
        FieldFx_ModelScrollUv(D_8019B690, 0, 0);
        blast->state = 0;
        blast->timer = 0;
        func_800CE870((char *)ANIM_OWNER_AS(SceneE19BlastChannel)->pool, 1, &blast->position.x);
        blast->position.y = g_RoomFloorY->raw;
        func_800CE870((char *)ANIM_OWNER_AS(SceneE19BlastChannel)->pool, 0, &blast->target.x);
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(SceneE19BlastChannel)->pool, 0, &blast->origin);
        return func_800CE560((char *)ANIM_POOL_AS(SceneE19BlastChannel)->pool, 0xC, 8, func_80192E08);
    case 1:
        switch (blast->state) {
        case 0:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer == 2 && D_800B0E64[0] != 0) {
                Akao_SendTableCommand(D_800B0E64[0], 0x5E3, func_800D3FD8(), 0x80, 0x7F);
                if (D_800B0E64[0] != 0) {
                    Akao_SendTableCommand(D_800B0E64[0], 0x5E4, 0x80, 0x80, 0x7F);
                }
            }
            if (blast->timer < 0x10) {
                return 0;
            }
            blast->state = 1;
            blast->timer = 0;
            break;
        case 1:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer < 0x10) {
                return 0;
            }
            blast->state = 2;
            blast->timer = 0;
            break;
        case 2:
            timer = blast->timer + 1;
            blast->timer = timer;
            if (timer < 0x10) {
                return 0;
            }
            blast->state = 3;
            blast->timer = 0;
            break;
        case 3:
            blast->timer++;
            if (func_800C6B90(&blast->position, 0x960) != 0
                && (func_800C6B90(&blast->position, 0x4B0) != 0
                    || func_800C6B90(&blast->position, 0x6A4) == 0)
                && FX_TASK_AS(SceneE19BlastScript)->active != 0
                && ((*ANIM_OWNER_AS(SceneE19BlastChannel)->pool)->flags & 0x3F000000) == 0x01000000) {
                (*((SceneE19BlastPlayer **)D_8009D254))->flags |= 0x4000;
                actor = *ANIM_OWNER_AS(SceneE19BlastChannel)->pool;
                actor->flags = (actor->flags & 0xC0FFFFFF) | 0x2D000000;
                actor = *ANIM_OWNER_AS(SceneE19BlastChannel)->pool;
                actor->flags |= 0x80000000;
            }
            if (blast->timer < 0x11) {
                func_800D1D24(2, 0x10, blast->timer);
            }
            spread = 0x200;
            spark = func_800CE610((char *)ANIM_POOL_AS(SceneE19BlastChannel)->pool);
            if (spark != 0) {
                spark->x = blast->position.x;
                spark->y = blast->position.y;
                spark->z = blast->position.z;
                spark->y -= func_80071A54() % 800;
                spark->x += func_80071A54() % spread - 0x100;
                value = func_80071A54() % spread;
                spark->state = 0;
                spark->timer = 0;
                spark->z += value - 0x100;
            }
            D_8019B668 = 0x80;
            if (blast->timer < 0x20) {
                return 0;
            }
            blast->state = 4;
            blast->timer = 0;
            break;
        case 4:
            blast->timer++;
            spread = 0x200;
            spark = func_800CE610((char *)ANIM_POOL_AS(SceneE19BlastChannel)->pool);
            if (spark != 0) {
                spark->x = blast->position.x;
                spark->y = blast->position.y;
                spark->z = blast->position.z;
                spark->y -= func_80071A54() % 800;
                spark->x += func_80071A54() % spread - 0x100;
                value = func_80071A54() % spread;
                spark->state = 0;
                spark->timer = 0;
                spark->z += value - 0x100;
            }
            D_8019B668 = rcos((blast->timer << 10) / 24) / 32;
            if (blast->timer < 0x18) {
                return 0;
            }
            blast->state = 4;
            blast->timer = 0;
            return 1;
        }
        break;
    case 2:
        center.x = blast->position.x;
        center.y = blast->position.y;
        center.z = blast->position.z;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);

        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        switch (blast->state) {
        case 0:
            phase = blast->timer << 6;
            intensity = rsin(phase) / 32;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            if (D_800E27EC & 1) {
                intensity = intensity * 2 / 3;
            }
            floorRotation0.pad = 1;
            floorRotation0.x = 0x400;
            floorRotation0.y = 0;
            floorRotation0.z = 0;
            floorGlow0.x = center.x;
            floorGlow0.z = center.z;
            floorGlow0.y = g_RoomFloorY->raw;
            func_800CEE20(&floorGlow0, (GteRotation *)&floorRotation0, 0x2000, 0x2000, 0,
                          GetClut(0, D_800E120A + 2), 1, intensity, 0);
            break;
        case 1:
            phase = blast->timer << 6;
            intensity = 0x80;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            floorRotation1.pad = 1;
            floorRotation1.x = 0x400;
            floorRotation1.y = 0;
            floorRotation1.z = 0;
            floorGlow1.x = center.x;
            floorGlow1.z = center.z;
            floorGlow1.y = g_RoomFloorY->raw;
            func_800CEE20(&floorGlow1, (GteRotation *)&floorRotation1, 0x2000, 0x2000, 0,
                          GetClut(0, D_800E120A + 2), 1, intensity, 0);
            intensity = rsin(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 2 / 3;
            }
            radialScale = rsin(phase);
            if (D_800E27EC & 1) {
                radialScale = radialScale * 15 / 16;
            }
            rotation.x = -0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 5;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix1);
            shellScale1.x = radialScale / 2;
            shellScale1.y = radialScale / 2;
            shellMatrix1.t[0] = center.x;
            shellMatrix1.t[1] = center.y;
            shellMatrix1.t[2] = center.z;
            shellScale1.z = (blast->timer << 6) + 0x400;
            ScaleMatrix(&shellMatrix1, &shellScale1);
            FieldFx_ModelSaveColors(D_8019B684);
            FieldFx_ModelScaleColors(D_8019B684, 0x40);
            FieldFx_ModelDraw(D_8019B684, &shellMatrix1);
            FieldFx_ModelRestoreColors(D_8019B684);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D004C(&center, 0x12C, 0x12C, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            intensity = rcos(phase) / 32;
            radialScale = rsin(phase);
            func_800D0728(&center, 0x7D0, 0xB54, 0x18, &flareRotation, radialScale, radialScale, 0,
                          &glowColor, intensity, 1);
            break;
        case 2:
            phase = blast->timer << 6;
            intensity = 0x80;
            params = &D_800F3368;
            asm("" : "=r"(params) : "0"(params));
            params->parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            asm("" : "=r"(pageSelector) : "0"(&D_800E11E4[11]) : "$16");
            D_800F3368.tpage = D_800E2850[pageSelector[0]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            if (D_800E27EC & 1) {
                intensity = 0x55;
            }
            floorRotation2.pad = 1;
            floorRotation2.x = 0x400;
            floorRotation2.y = 0;
            floorRotation2.z = 0;
            floorGlow2.x = center.x;
            floorGlow2.z = center.z;
            floorGlow2.y = g_RoomFloorY->raw;
            func_800CEE20(&floorGlow2, (GteRotation *)&floorRotation2, 0x2000, 0x2000, 0,
                          GetClut(0, D_800E120A + 2), 1, intensity, 0);
            radialScale = 0x1000;
            {
                s32 textureDepth;
                s32 blendMode;
                s32 textureX;
                u32 pageIndex;
                palettes = D_800E1204;
                if (D_800E27EC & 1) {
                    radialScale = 0xF00;
                }
                textureDepth = 0;
                blendMode = 1;
                textureX = 0;
                rotation.x = -0x400;
                rotation.y = 0;
                pageIndex = pageSelector[-8];
                rotation.z = D_800E27EC << 5;
                params->tpage = D_800E2850[pageIndex];
                params->palette = 3;
                params->parameter06 = 0;
                page = (u16)(D_800E2850[pageSelector[-8]] | GetTPage(textureDepth, blendMode, textureX, 0));
            }
            {
                s32 paletteValue;
                paletteValue = palettes[params->palette];
                if (params->palette == 4 && D_800F3428 != 0) {
                    paletteValue += 4;
                }
                func_800C6EC0(page, GetClut(0x20, paletteValue));
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix2);
            shellScale2.x = radialScale / 2;
            shellScale2.y = radialScale / 2;
            shellMatrix2.t[0] = center.x;
            shellMatrix2.t[1] = center.y;
            shellMatrix2.t[2] = center.z;
            shellScale2.z = (blast->timer << 6) + 0x800;
            ScaleMatrix(&shellMatrix2, &shellScale2);
            FieldFx_ModelSaveColors(D_8019B684);
            FieldFx_ModelScaleColors(D_8019B684, 0x40);
            FieldFx_ModelDraw(D_8019B684, &shellMatrix2);
            FieldFx_ModelRestoreColors(D_8019B684);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D004C(&center, 0x12C, 0x12C, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x64;
            }
            modelPhase = 2000;
            radialScale = rcos(phase) / 8;
            height = rsin(phase) * 2 / 3;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = 0;
            for (i = 0; i < 0x10; i++) {
                verticalScale = (i << 8) + D_800E27EC * 4;
                ringPoint.x = center.x;
                ringPoint.y = center.y;
                ringPoint.z = center.z;
                ringPoint.x += rcos(verticalScale) * modelPhase / 4096;
                ringPoint.z += rsin(verticalScale) * modelPhase / 4096;
                rotation.z = verticalScale + 0x400;
                func_800D0E88(&ringPoint, (GteRotation *)&rotation, height, radialScale,
                              &glowColor, 0, 0, (s16)intensity, 1);
            }
            intensity = rcos(phase) / 32;
            radialScale = rcos(phase);
            target = &blast->target;
            rotation.x = blast->origin.x;
            rotation.y = blast->origin.y;
            rotation.z = blast->origin.z;
            rotation.x -= 0x200;
            rotation.y += 0x400;
            func_800D0728(target, 0x7D0, 0xA8C, 0x20, (GteRotation *)&rotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            rotation.x += 0x400;
            func_800D0728(target, 0x7D0, 0xA8C, 0x20, (GteRotation *)&rotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            break;
        case 3:
            timer = blast->timer;
            phase = timer << 5;
            if (timer == 0) {
                func_800D1AE0((unsigned char *)&glowColor, 0x46, 2, 8);
            } else if (timer < 9) {
                func_800D1AE0((unsigned char *)&glowColor, 0x80 - timer * 0x10, 1, 8);
            }
            radialScale = rcos(phase) / 2 + 0x400;
            verticalScale = rsin(phase) / 6 + 0x555;
            intensity = 0x80;
            if (D_800E27EC & 1) {
                intensity = 0x78;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix3a);
            shellScale3a.x = radialScale;
            shellScale3a.y = verticalScale;
            shellScale3a.z = radialScale;
            shellMatrix3a.t[0] = center.x;
            shellMatrix3a.t[1] = center.y;
            shellMatrix3a.t[2] = center.z;
            ScaleMatrix(&shellMatrix3a, &shellScale3a);
            FieldFx_ModelSaveColors(D_8019B680);
            FieldFx_ModelScaleColors(D_8019B680, (u16)intensity);
            FieldFx_ModelDraw(D_8019B680, &shellMatrix3a);
            FieldFx_ModelRestoreColors(D_8019B680);
            radialScale = rsin(phase) / 3 + 0x400;
            verticalScale = rcos(phase) / 2;
            intensity = rcos(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC * 0x30;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix3b);
            shellScale3b.x = radialScale;
            shellScale3b.y = verticalScale;
            shellScale3b.z = radialScale;
            shellMatrix3b.t[0] = center.x;
            shellMatrix3b.t[1] = center.y;
            shellMatrix3b.t[2] = center.z;
            ScaleMatrix(&shellMatrix3b, &shellScale3b);
            FieldFx_ModelSaveColors(D_8019B688);
            FieldFx_ModelScaleColors(D_8019B688, (u16)(intensity / 2));
            FieldFx_ModelDraw(D_8019B688, &shellMatrix3b);
            FieldFx_ModelRestoreColors(D_8019B688);
            modelPhase = (blast->timer << 10) / 56;
            radialScale = rsin(modelPhase) / 4 + 0xC00;
            verticalScale = rcos(modelPhase) / 4 + 0x400;
            intensity = rcos(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            height = center.y;
            center.y -= blast->timer * 0x18;
            RotMatrixYXZ(&rotation, &shellMatrix3c);
            shellScale3c.x = radialScale;
            shellScale3c.y = verticalScale;
            shellScale3c.z = radialScale;
            shellMatrix3c.t[0] = center.x;
            shellMatrix3c.t[1] = center.y;
            shellMatrix3c.t[2] = center.z;
            ScaleMatrix(&shellMatrix3c, &shellScale3c);
            FieldFx_ModelSaveColors(D_8019B68C);
            FieldFx_ModelScaleColors(D_8019B68C, (u16)(intensity / 2));
            FieldFx_ModelDraw(D_8019B68C, &shellMatrix3c);
            FieldFx_ModelRestoreColors(D_8019B68C);
            center.y = height;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D004C(&center, 0x9C4, 0x9C4, 0xC, 0, 0x1000, 0x1000, &glowColor, 0, intensity, 1);
            radialScale = rsin(phase) / 4 + 0xC00;
            intensity = rcos(phase) / 32;
            func_800D0728(&center, 0x76C, 0xA28, 0x18, &flareRotation, radialScale, radialScale,
                          &flareColor, 0, intensity, 1);
            center.y -= 0x400;
            radialScale = rsin(phase) * 3 / 2 + 0x1000;
            func_800D0728(&center, 0x3E8, 0x5DC, 0x18, &flareRotation, radialScale, radialScale,
                          &glowColor, 0, intensity, 1);
            center.y += 0x400;
            radialScale = rsin(modelPhase) / 4 + 0x1200;
            verticalScale = rcos(modelPhase);
            intensity = rcos(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = (D_800E27EC << 5) + 0x400;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x60, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix3d);
            shellScale3d.x = radialScale;
            shellScale3d.y = verticalScale;
            shellScale3d.z = radialScale;
            shellMatrix3d.t[0] = center.x;
            shellMatrix3d.t[1] = center.y;
            shellMatrix3d.t[2] = center.z;
            ScaleMatrix(&shellMatrix3d, &shellScale3d);
            FieldFx_ModelSaveColors(D_8019B690);
            FieldFx_ModelScaleColors(D_8019B690, (u16)intensity);
            FieldFx_ModelDraw(D_8019B690, &shellMatrix3d);
            FieldFx_ModelRestoreColors(D_8019B690);
            radialScale = rsin(modelPhase) / 6 + 0x1200;
            verticalScale = rcos(modelPhase) * 2;
            intensity = rcos(modelPhase) / 32;
            if (!(D_800E27EC & 1)) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x60, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix3e);
            shellScale3e.x = radialScale;
            shellScale3e.y = verticalScale;
            shellScale3e.z = radialScale;
            shellMatrix3e.t[0] = center.x;
            shellMatrix3e.t[1] = center.y;
            shellMatrix3e.t[2] = center.z;
            ScaleMatrix(&shellMatrix3e, &shellScale3e);
            FieldFx_ModelSaveColors(D_8019B690);
            FieldFx_ModelScaleColors(D_8019B690, (u16)intensity);
            FieldFx_ModelDraw(D_8019B690, &shellMatrix3e);
            FieldFx_ModelRestoreColors(D_8019B690);
            break;
        case 4:
            phase = (blast->timer << 10) / 24;
            radialScale = (0x400 - phase) / 2 + 0x200;
            verticalScale = rsin(phase) / 6 + 0x6AA;
            intensity = rcos(phase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix4a);
            shellScale4a.x = radialScale;
            shellScale4a.y = verticalScale;
            shellScale4a.z = radialScale;
            shellMatrix4a.t[0] = center.x;
            shellMatrix4a.t[1] = center.y;
            shellMatrix4a.t[2] = center.z;
            ScaleMatrix(&shellMatrix4a, &shellScale4a);
            FieldFx_ModelSaveColors(D_8019B680);
            FieldFx_ModelScaleColors(D_8019B680, (u16)intensity);
            FieldFx_ModelDraw(D_8019B680, &shellMatrix4a);
            FieldFx_ModelRestoreColors(D_8019B680);
            modelPhase = ((blast->timer + 0x20) << 10) / 56;
            radialScale = rsin(modelPhase) / 4 + 0xC00;
            verticalScale = rcos(modelPhase) / 4 + 0x400;
            intensity = rcos(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = -D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x20, palette));
            FieldFx_ModelSetSemiTrans(1);
            height = center.y;
            center.y -= (blast->timer + 0x20) * 0x18;
            RotMatrixYXZ(&rotation, &shellMatrix4b);
            shellScale4b.x = radialScale;
            shellScale4b.y = verticalScale;
            shellScale4b.z = radialScale;
            shellMatrix4b.t[0] = center.x;
            shellMatrix4b.t[1] = center.y;
            shellMatrix4b.t[2] = center.z;
            ScaleMatrix(&shellMatrix4b, &shellScale4b);
            FieldFx_ModelSaveColors(D_8019B68C);
            FieldFx_ModelScaleColors(D_8019B68C, (u16)(intensity / 2));
            FieldFx_ModelDraw(D_8019B68C, &shellMatrix4b);
            FieldFx_ModelRestoreColors(D_8019B68C);
            center.y = height;
            radialScale = rsin(modelPhase) / 4 + 0x1200;
            verticalScale = rcos(modelPhase);
            intensity = rcos(modelPhase) / 32;
            if (D_800E27EC & 1) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = (D_800E27EC << 5) + 0x400;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x60, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix4c);
            shellScale4c.x = radialScale;
            shellScale4c.y = verticalScale;
            shellScale4c.z = radialScale;
            shellMatrix4c.t[0] = center.x;
            shellMatrix4c.t[1] = center.y;
            shellMatrix4c.t[2] = center.z;
            ScaleMatrix(&shellMatrix4c, &shellScale4c);
            FieldFx_ModelSaveColors(D_8019B690);
            FieldFx_ModelScaleColors(D_8019B690, (u16)intensity);
            FieldFx_ModelDraw(D_8019B690, &shellMatrix4c);
            FieldFx_ModelRestoreColors(D_8019B690);
            radialScale = rsin(modelPhase) / 6 + 0x1200;
            verticalScale = rcos(modelPhase) * 2;
            intensity = rcos(modelPhase) / 32;
            if (!(D_800E27EC & 1)) {
                intensity = intensity * 15 / 16;
            }
            rotation.x = 0;
            rotation.y = D_800E27EC << 5;
            rotation.z = 0;
            D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[D_800E11E4[3]] | GetTPage(0, 1, 0, 0));
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                palette += 4;
            }
            func_800C6EC0(page, GetClut(0x60, palette));
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &shellMatrix4d);
            shellScale4d.x = radialScale;
            shellScale4d.y = verticalScale;
            shellScale4d.z = radialScale;
            shellMatrix4d.t[0] = center.x;
            shellMatrix4d.t[1] = center.y;
            shellMatrix4d.t[2] = center.z;
            ScaleMatrix(&shellMatrix4d, &shellScale4d);
            FieldFx_ModelSaveColors(D_8019B690);
            FieldFx_ModelScaleColors(D_8019B690, (u16)intensity);
            FieldFx_ModelDraw(D_8019B690, &shellMatrix4d);
            FieldFx_ModelRestoreColors(D_8019B690);
            break;
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.extent_x = 0x80;
        D_800F3368.extent_y = 0x10;
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

PE1_STATIC_ASSERT(sizeof(SceneE19HomingParticle) == 0x20,
                  scene_e19_homing_particle_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneE19HomingParticle, state) == 0x1C,
                  scene_e19_homing_particle_state);
int func_800CFE94(s16 *from, s16 *to);

/* Not declared in production headers; parameter order inferred from both
 * call sites and the ABI. The eighth argument is zero at both sites. */

int func_8019549C(int mode, SceneE19HomingParticle *particle)
{
    GteRotation templatePoint = D_8018F210;
    GteShortVector temporaryPosition;
    GteShortVector rotation;
    RenderColor color = D_8018F218;
    GteShortVector floor_position;
    GteRotation floor_rotation;
    register int divider asm("$16");
    int bias, random;
    register int remMagic asm("$5");
    register int test asm("$2");
    register int sum asm("$3");
    int intensity, scale, palette, pulseSum;
    u16 clut;
    u16 floorY;
    u16 floorZ;
    register int floorIndex asm("$4");
    int floorTimer;
    register int negValue asm("$2");
    int signedValue;
    register int returnTest asm("$2");

    switch (mode) {
    case 1:
        switch (particle->state) {
    case 0:
        particle->timer = 0;
        particle->state = 1;
        particle->angles.pad = 0;
        FieldFx_TransformMatrixPoint(
            (struct RoomFxTransformOwner *)D_800F32D0->actor,
            0, (GteShortVector *)&templatePoint, &particle->position);
        func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->actor,
                     0, &rotation);
        rotation.x = 0;
        func_800CE870((char *)D_8009D254, 1, &particle->target.x);
        func_800CE870((char *)D_8009D254, 0, &temporaryPosition.x);

        test = func_800CFE94(&particle->position.x, &temporaryPosition.x) < 1900;
        test = -test;
        bias = test & 500;
        random = func_80071A54();
        remMagic = 0x66660000;
        asm("" : : "r"(remMagic));
        random &= 1023;
        asm volatile("" : : "r"(random));
        sum = bias + 390;
        rotation.x += random - sum;
        asm("" : : "r"(sum));
        rotation.y += -512 + (s16)(particle->position.pad % 5) * 256;
        particle->angles.x = rotation.x;
        particle->angles.y = rotation.y;
        particle->angles.z = rotation.z;
        particle->speed = 24;
        particle->turnPhase = 0;
        asm("" : : : "$3", "$18", "$9", "$10", "$11", "$15", "$24", "$25");
        break;

    case 1:
        particle->timer++;
        FieldFx_CalculateLookAngles(&particle->position, &particle->target,
                                    &rotation);
        func_800CFD50((u16 *)&rotation, (u16 *)&particle->angles,
                     (u16)(rsin(particle->turnPhase) / 5));

        if (particle->speed < 62)
            particle->speed += 4;
        if (particle->speed >= 40) {
            if (particle->turnPhase < 2048)
                particle->turnPhase += 102;
            if (particle->turnPhase > 2048)
                particle->turnPhase = 2048;
        }

        func_800CFB7C(&particle->angles,
                     (s16)(particle->speed * particle->speed / 16),
                     &temporaryPosition);
        particle->position.x += temporaryPosition.x;
        particle->position.y += temporaryPosition.y;
        particle->position.z += temporaryPosition.z;

        if (func_800C6B90(&particle->position.x, 190) &&
            particle->position.y > g_RoomFloorY->y - 514) {
            if (D_800E2368->flags &&
                (D_800F32D0->actor->state->core_flags & 0x3F000000)
                    == 0x01000000) {
                ((FieldActor *)D_8009D254)->state->flags |= 0x4000;
                D_800F32D0->actor->state->core_flags =
                    (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF)
                    | 0x29000000;
                D_800F32D0->actor->state->core_flags |= 0x80000000;
            }
            particle->state = 2;
            particle->timer = 0;
        }

        if (particle->position.y >= g_RoomFloorY->y) {
            particle->position.y = g_RoomFloorY->y;
            particle->state = 2;
            particle->timer = 0;
        }
        return 0;

    case 2:
        particle->timer++;
        if (func_800C6B90(&particle->position.x, 200) &&
            D_800E2368->flags &&
            (D_800F32D0->actor->state->core_flags & 0x3F000000)
                == 0x01000000) {
            ((FieldActor *)D_8009D254)->state->flags |= 0x4000;
            D_800F32D0->actor->state->core_flags =
                (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF)
                | 0x29000000;
            D_800F32D0->actor->state->core_flags |= 0x80000000;
        }
        returnTest = particle->timer < 24;
        if (returnTest) return 0;
        return 1;

    case 3:
        returnTest = (unsigned short)particle->timer + 1;
        particle->timer = returnTest;

        returnTest = (s16)returnTest < 12;
test_timer:

        if (returnTest) return 0;
        return 1;

        }
        return 0;
    case 2:
        switch (particle->state) {
    case 1:
        intensity = 128;
        if (particle->timer & 1)
            intensity = 92;
        rotation.x = 0;
        rotation.y = 0;
        signedValue = particle->timer;
        rotation.z = signedValue << 7;
        sum = 64;
        D_800F3368.parameter00 = sum;
        D_800F3368.extent_x = sum;
        D_800F3368.extent_y = sum;
        rotation.pad = 0;
        D_800F3368.parameter02 = 4;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&particle->position, (GteRotation *)&rotation, 6144, 6144,
                     68, clut, 1, intensity / 2, 0);
        D_800F3368.parameter02 = 2;
        asm volatile("" : :);
        D_800F3368.parameter00 = 32;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        negValue = rotation.z;
        rotation.z = -negValue;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&particle->position, (GteRotation *)&rotation, 8192, 8192,
                     152, clut, 1, intensity, 0);
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D004C(&particle->position, 600, 600, 5,
                     0, 4096, 4096, &color, 0, 70, 1);
        if (particle->timer < 24 && (particle->position.pad & 1)) {
            intensity = rcos((particle->timer << 10) / 24) / 64;
floorIndex = D_800F3368.palette;
temporaryPosition.x = particle->position.x;
temporaryPosition.y = particle->position.y;
pulseSum = 64;
floorZ = particle->position.z;
asm volatile("" : :);

temporaryPosition.z = floorZ;
D_800F3368.parameter02 = 4;
D_800F3368.parameter00 = pulseSum;
D_800F3368.extent_x = pulseSum;
D_800F3368.extent_y = pulseSum;
palette = D_800E1204[floorIndex];
if (floorIndex == 4 && D_800F3428) palette += 4;
            clut = GetClut(160, palette);
            func_800D3BC8(&temporaryPosition, 6144, 6144, 12,
                         clut, 1, intensity / 2, 0, 3072);
            D_800F3368.parameter00 = 32;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 32;
            D_800F3368.extent_y = 32;
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428)
                palette += 4;
            clut = GetClut(176, palette);
            func_800D3BC8(&temporaryPosition, 6144, 6144, 154,
                         clut, 1, intensity / 2, 0, 1536);
        }
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        if (particle->timer < 13 && !(particle->position.pad & 1)) {
            func_800D0E88(&particle->position, (GteRotation *)&particle->angles,
                         1900, (12 - particle->timer) << 8,
                         &color, 0, 0, 62, 1);
        }
        break;
    case 2:
        divider = 0x2aaa0000;
asm("" : : "r"(divider));
        D_800F3368.parameter00 = 64;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 64;
        D_800F3368.extent_y = 64;
        asm volatile("" : :);
        intensity = rcos((particle->timer << 10) / 24) / 32;
        scale = rcos((particle->timer << 10) / 24) * 2;
        if (particle->timer & 1)
            intensity = intensity * 2 / 3;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&particle->position, 0, scale, scale,
                     8, clut, 1, intensity, 0);
        floorIndex = D_800F3368.palette;
        floor_rotation.x = 1024;
        floor_rotation.y = 0;
        floorTimer = particle->timer;
        floor_rotation.z = floorTimer << 5;
        floor_rotation.flags = 1;
        floor_position.x = particle->position.x;
        floorY = g_RoomFloorY->y;
        sum = (u16)particle->position.z;

        floor_position.y = floorY;
        floor_position.z = sum;
        palette = D_800E1204[floorIndex];
        if (floorIndex == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&floor_position, &floor_rotation,
                     8192 - scale, 8192 - scale,
                     64, clut, 1, intensity / 2, 0);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        break;

        }
        return 0;
    }
    return 0;
timer_return_zero:
    return 0;
timer_return_one:
    return 1;
}

/* Mode 0 anchors the burst on the room actor and wakes the actor's
 * object; mode 1 releases a homing particle on odd frames (twenty in all)
 * and reports 2 after eight frames; mode 2 draws the flash, the screen
 * tint and the glows that shrink over 32 frames. */
int func_801960C4(int mode, SceneHomingBurst *burst)
{
    GteShortVector position;
    GteRotation rotation = D_8018F210;
    GteRotation tilt = D_8018F1CC;
    RenderColor flashColor = D_8018F21C;
    RenderColor ringColor = D_8018F220;
    SceneHomingBurstChild *child;
    int fade;
    int scale;
    int time;

    switch (mode) {
    case 0:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneHomingBurstChannel)->pool, 0, &rotation, &burst->position);
        burst->timer = 0;
        burst->released = 0;
        if (FX_TASK_AS(SceneHomingBurstEvent)->active) {
            SceneHomingBurstPool *pool = ANIM_OWNER_AS(SceneHomingBurstChannel)->pool;
            if (pool) {
                SceneHomingBurstObject *object = pool->object;
                if (object) {
                    if (*object->status == 1) *object->status = 2;
                }
            }
        }
        return func_800CE560((char *)ANIM_POOL_AS(SceneHomingBurstChannel)->pool, 0x20, 0x20, func_8019549C);
    case 1:
        burst->timer++;
        if (burst->released < 0x14 && (burst->timer & 1)) {
            child = func_800CE610((char *)ANIM_POOL_AS(SceneHomingBurstChannel)->pool);
            if (child) {
                child->index = burst->released;
                child->state = 0;
                child->timer = 0;
            }
            burst->released++;
        }
        if (burst->timer < 8) break;
        return 2;
    case 2:
        position.x = burst->position.x;
        position.y = burst->position.y;
        position.z = burst->position.z;
        time = burst->timer;
        if (time < 0x19) {
            fade = 0x80 - (time << 7) / 24;
            scale = rsin((time << 10) / 24);
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800D0728(&position, 0x6A4, 0x9C4, 0x16, 0, scale, scale, 0, &flashColor,
                          fade, 1);
            time = burst->timer;
        }
        if (time < 0xD) {
            func_800D1AE0((unsigned char *)&flashColor, 0x80 - (time << 7) / 12, 1, 8);
        }
        if (burst->timer < 0x21) {
            {
                int tpage;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
                D_800F3368.palette = 3;
                D_800F3368.depth = 0x32;
                D_800F3368.parameter06 = 0;
                D_800F3368.parameter0A = 0;
                D_800F3368.tpage = tpage;
            }
            {
                int ticks = burst->timer;
                fade = 0x80 - ticks * 4;
                tilt.z = ticks << 4;
            }
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, &tilt, 0x3000, 0x3000, 0x98,
                              GetClut(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, &tilt, 0x1800, 0x1800, 0x98,
                              GetClut(0x70, palette), 1, fade, 0);
            }
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&position, 0, 0x2000, 0x2000, 8,
                              GetClut(0x90, palette), 1, fade / 2, 0);
            }
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            scale = (burst->timer << 5) + 0x1000;
            func_800D0728(&position, 0x2BC, 1000, 0x14, 0, scale, scale, 0, &ringColor,
                          fade / 2, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        {
            int tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Mode 1 spins and ages the ray (states return 1 when they finish);
 * mode 2 draws it as a growing ray, a sweeping blade or a glow sprite. */
int func_80196748(int mode, SceneSpinRay *ray)
{
    RenderColor color;
    RenderColor rayColor;
    RenderColor bladeHead;
    RenderColor bladeTail;
    GteShortVector offset;
    int time;
    int width;
    int glow;
    int angle;

    rayColor = D_8018F224;
    bladeHead = D_8018F228;
    bladeTail = D_8018F22C;
    switch (mode) {
    case 1:
        switch (ray->state) {
        case 0:
            ray->timer++;
            ray->spin.angles.x += ray->reach;
            ray->spin.angles.y += ray->spinY;
            ray->spin.angles.z += 0x18;
            break;
        case 1:
            ray->timer++;
            ray->spin.angles.y += ray->spinY;
            ray->spin.angles.x += 8;
            ray->spin.angles.z += 0x18;
            if ((s16)ray->timer < 0x30) break;
            return 1;
        case 2:
            ray->timer++;
            ray->spin.angles.x += 4;
            ray->spin.angles.y += 4;
            ray->spin.angles.z += 0x30;
            if ((s16)ray->timer < 0x10) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        switch (ray->state) {
        case 0:
            time = (s16)ray->timer;
            if (time >= 0x10) time = 0x10;
            width = rsin(time << 6) / 32;
            if (ray->timer & 1) glow = 0x5C;
            else glow = 0x80;
            func_800CF3AC(D_8019B3E4, &color, time);
            func_800D0E88(&D_8019B670, &ray->spin.rotation, 0x9C4, width, &rayColor, 0, 0,
                          (s16)glow, 1);
            break;
        case 1: {
            int kind;
            int palette;
            angle = rcos(((s16)ray->timer << 10) / 48);
            glow = ((s16)ray->timer << 7) / 48;
            width = rsin(((s16)ray->timer << 10) / 48) / 4 + 0x1F4;
            func_800CFB7C(&ray->spin.angles, (s16)(ray->reach * angle / 4096), &offset);
            offset.x += D_8019B670.x;
            offset.y += D_8019B670.y;
            offset.z += D_8019B670.z;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800D2370(&offset, &ray->spin.rotation, width, 0x8C, 0x80, 0x80, 0x80, 0x10,
                          (u16)GetClut(0x70, palette), &bladeHead, &bladeTail,
                          (s16)glow, 1);
            break;
        }
        case 2: {
            int kind;
            int palette;
            width = rcos((s16)ray->timer << 6) * 2;
            func_800CF3AC(D_8019B3E4, &color, (s16)ray->timer);
            {
                int tpage;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                tpage = D_800E2850[D_800E11EA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
            }
            ray->spin.rotation.flags = 1;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&D_8019B670, &ray->spin.rotation, width, width, 0x40,
                          (u16)GetClut(0, palette), 1, 0x80, &color);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Spin ray controller: for 48 frames it seeds spinning rays on odd frames,
 * casts one of its sixteen blades every fourth frame and fans out 26
 * blades on the first frame. While it runs it follows the room actor,
 * publishes that spot as the ray origin and draws a halo, two glows and
 * two rings there. */
int func_80196CCC(int mode, SceneSpinRayBurst *burst)
{
    GteRotation tilt = D_8018F210;
    GteShortVector position;
    RenderColor color;
    RenderColor ringColor = D_8018F230;
    GteShortVector floor;
    GteRotation flat;
    SceneSpinRay *child;
    int i;
    int scale;
    int glow;

    switch (mode) {
    case 0:
        burst->timer = 0;
        burst->blades = 0x10;
        func_800D3F64(0x5E1, func_800D3FD8());
        func_800D3F64(0x5E2, 0x80);
        return func_800CE560(ANIM_POOL_AS(SceneSpinRayChannel)->pool, 0x10, 0x32,
                             (FieldFxCallbackListCallback)func_80196748);
    case 1:
        if (++burst->timer & 1) {
            child = func_800CE610(ANIM_POOL_AS(SceneSpinRayChannel)->pool);
            if (child) {
                child->spin.angles.x = func_80071A54();
                child->spin.angles.y = func_80071A54();
                child->spin.angles.z = func_80071A54();
                child->timer = 0;
                child->state = 2;
            }
        }
        if (!(burst->timer & 3) && burst->blades > 0) {
            child = func_800CE610(ANIM_POOL_AS(SceneSpinRayChannel)->pool);
            if (child) {
                func_800CFFAC(child);
                child->timer = 0;
                child->state = 0;
                child->reach = (func_80071A54() & 0x1F) - 0x10;
                child->spinY = (func_80071A54() & 0x1F) - 0x10;
            }
            burst->blades--;
        }
        if ((s16)burst->timer == 1) {
            for (i = 0; i < 26; i++) {
                child = func_800CE610(ANIM_POOL_AS(SceneSpinRayChannel)->pool);
                if (child) {
                    func_800CFFAC(child);
                    child->timer = 0;
                    child->state = 1;
                    child->reach = (func_80071A54() & 0x1FF) + 600;
                    child->spinY = (func_80071A54() & 7) - 3;
                }
            }
        }
        if ((s16)burst->timer < 0x30) break;
        return 1;
    case 2:
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneSpinRayChannel)->pool, 0, &tilt, &position);
        D_8019B670.x = position.x;
        D_8019B670.y = position.y;
        D_8019B670.z = position.z;
        if (burst->timer & 1) glow = 0x60;
        else glow = 0x80;
        scale = rsin(((s16)burst->timer << 10) / 48) / 2 + 0x800;
        func_800CF3AC(D_8019B404, &color, (s16)burst->timer);
        func_800D004C(&position, 800, 800, 12, 0, scale, scale, &color, 0, 100, 1);
        {
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&position, 0, scale, scale, 8, GetClut(0x90, palette), 1,
                          glow, 0);
        }
        flat.x = 0x400;
        flat.y = 0;
        flat.z = 0;
        flat.flags = 1;
        floor.x = position.x;
        floor.z = position.z;
        floor.y = g_RoomFloorY->raw;
        {
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20(&floor, &flat, scale, scale, 8, GetClut(0x10, palette), 3,
                          glow, 0);
        }
        glow = ((s16)burst->timer << 7) / 48;
        func_800D0728(&position, 0x44C, 0x1770, 0x10, 0, 0x1000, 0x1000, 0, &ringColor,
                      glow, 1);
        func_800D0728(&position, 1000, 0x4B0, 0x18, 0, 0x1000, 0x1000, &ringColor, 0,
                      glow, 1);
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

/* Ember flare particle: embers drift and bounce off the floor while their
 * sideways speed decays (state 0 slowly, state 1 quickly while falling),
 * and a wide flare pulses in place (state 2). */
int func_801972EC(int mode, RoomOrbitTrailParticle *p) {
    GteRotation spin = D_8018F1CC;
    GteRotation turn = D_8018F1D4;
    RenderColor color = D_8018F234;
    int fall;
    int bounce;
    int size;
    int glow;

    switch (mode) {
    case 1:
        switch (p->state) {
        case 0:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 31 / 32;
            p->heading.z = p->heading.z * 31 / 32;
            fall = (u16)p->heading.y;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x10) break;
            return 1;
        case 1:
            p->timer++;
            p->x += p->heading.x;
            p->y += p->heading.y;
            p->z += p->heading.z;
            p->heading.x = p->heading.x * 127 / 128;
            p->heading.z = p->heading.z * 127 / 128;
            fall = (u16)p->heading.y - 1;
            p->heading.y = fall;
            if (p->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                p->heading.y = bounce;
            }
            if ((s16)p->timer < 0x18) break;
            return 1;
        case 2:
            p->timer++;
            if ((s16)p->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        switch (p->state) {
        case 0: {
            int kind;
            int palette;
            size = rsin((s16)p->timer << 7) * 3 / 2;
            glow = rsin((s16)p->timer << 7) / 160;
            spin.z = -((s16)p->timer * 32);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800CF3AC(D_8019B484, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0x98,
                          GetClut(0x90, palette), 2, glow, 0);
            break;
        }
        case 1: {
            int kind;
            int palette;
            size = rcos(((s16)p->timer << 10) / 24);
            spin.z = (s16)p->timer * 32;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800CF3AC(D_8019B484, &color, (s16)p->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            func_800CEE20((GteShortVector *)p, &spin, size, size, 0x98,
                          GetClut(0x80, palette), 1, 0x80, &color);
            break;
        }
        case 2:
            glow = rcos(((s16)p->timer << 10) / 24) / 32;
            size = rsin(((s16)p->timer << 10) / 24);
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            func_800CF3AC(D_8019B484, &color, (s16)p->timer * 2);
            func_800D0728((GteShortVector *)p, 0x320, 0x578, 0x18, &turn, size, size,
                          0, &color, glow, 1);
            break;
        }
        break;
    }
    return 0;
}

/* Ember flare controller: rides the room actor and glows up for 16
 * frames, then for 32 frames sprays orbiting sparks, embers on odd frames
 * and flares every sixth; then it fades and, on its first fading frame,
 * hands the actor to the scene script. While it glows it draws two floor
 * glows, a halo and a ring. */
int func_801978EC(int mode, SceneE22EmberBurst *burst) {
    RenderColor color = D_8018F238;
    GteShortVector position;
    GteRotation spin;
    GteShortVector floorPosition;
    GteRotation floorSpin;
    RoomOrbitTrailParticle *child;
    SceneE22EmberObjectChannel *channel;
    SceneE22EmberSlot *pool;
    SceneE22EmberObject *object;
    void **soundSlot;
    int volume;
    int glow;
    s16 z;

    switch (mode) {
    case 0:
        burst->state = 0;
        burst->timer = 0;
        burst->glow = 0;
        if (FX_TASK_AS(SceneE22EmberEvent)->active) {
            pool = ANIM_OWNER_AS(SceneE22EmberObjectChannel)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            Akao_SendTableCommand(*soundSlot, 0x5E5, func_800D3FD8(), 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5E6, 0x80, 0x80, volume);
        }
        func_800CE870((char *)RoomMain_ActorPtr, 1, (s16 *)burst);
        burst->y = g_RoomFloorY->y;
        return func_800CE560(ANIM_POOL_AS(SceneE22EmberChannel)->pool, 0x14, 0x28,
                             (FieldFxCallbackListCallback)func_801972EC);
    case 1:
        if (burst->state < 2)
            func_80020D50();
        switch (burst->state) {
        case 0:
            burst->timer++;
            burst->glow = rsin(burst->timer << 6) / 32;
            if (burst->timer < 0x10) break;
            burst->state = 1;
            burst->timer = 0;
            break;
        case 1:
            burst->timer++;
            child = func_800CE610(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
            if (child) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                child->heading.x = func_80071A54() % 50 - 25;
                child->heading.y = -(func_80071A54() % 32);
                child->heading.z = func_80071A54() % 50 - 25;
                child->state = 1;
                child->timer = 0;
            }
            if (burst->timer & 1) {
                child = func_800CE610(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->y -= func_80071A54() & 0x1FF;
                    child->x += (func_80071A54() & 0x7F) - 0x40;
                    child->z += (func_80071A54() & 0x7F) - 0x40;
                    child->heading.x = func_80071A54() % 24 - 12;
                    child->heading.y = func_80071A54() % 24 - 12;
                    child->heading.z = func_80071A54() % 24 - 12;
                    child->state = 0;
                    child->timer = 0;
                }
            }
            if (burst->timer % 6 == 0) {
                child = func_800CE610(ANIM_POOL_AS(SceneE22EmberChannel)->pool);
                if (child) {
                    child->x = burst->x;
                    child->y = burst->y;
                    child->z = burst->z;
                    child->state = 2;
                    child->timer = 0;
                }
            }
            if (burst->timer < 0x20) return 0;
            func_80020DD0();
            burst->state = 2;
            burst->timer = 0;
            break;
        case 2:
            burst->timer++;
            burst->glow = rcos((burst->timer << 10) / 24) / 32;
            if (burst->timer == 1 && FX_TASK_AS(SceneE22EmberEvent)->active) {
                channel = ANIM_OWNER_AS(SceneE22EmberObjectChannel);
                if ((channel->pool->object->flags & 0x3F000000) == 0x01000000) {
                    RoomMain_ActorPtr->state->flags |= 0x4000;
                    channel->pool->object->flags =
                        (channel->pool->object->flags & 0xC0FFFFFF) | 0x19000000;
                    channel->pool->object->flags |= 0x80000000;
                }
            }
            if (burst->timer < 0x18) break;
            return 1;
        }
        break;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.tpage = tpage;
        }
        if (burst->glow != 0) {
            glow = burst->glow;
            if (burst->timer & 1)
                glow = glow * 15 / 16;
            if (D_800E27EC & 1)
                glow = glow * 2 / 3;
            spin.x = 0x400;
            spin.z = D_800E27EC << 3;
            D_800F3368.depth = 0;
            spin.y = 0;
            spin.flags = 1;
            {
                int kind = D_800F3368.palette;
                int palette;
                position.x = burst->x;
                z = burst->z;
                position.y = g_RoomFloorY->y;
                position.z = z;
                palette = D_800E1204[kind];
                func_800CEE20(&position, &spin, 0x1000, 0x1000, 0,
                              GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6
                                                                        : palette + 2),
                              1, glow, 0);
            }
            {
                int tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
                D_800F3368.tpage = tpage;
                D_800F3368.parameter06 = 0;
                D_800F3368.palette = 3;
                floorSpin.x = 0x400;
                floorSpin.y = 0;
                floorSpin.z = D_800E27EC << 5;
                floorSpin.flags = 1;
            }
            floorPosition.x = burst->x;
            z = burst->z;
            floorPosition.y = g_RoomFloorY->y;
            floorPosition.z = z;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&floorPosition, &floorSpin, 0x1800, 0x1800, 0x40,
                              GetClut(0x10, palette), 1, glow, 0);
            }
            D_800F3368.depth = 0x28;
            func_800D004C((GteShortVector *)burst, 400, 400, 0x10, 0, 0x1000, 0x1000,
                          &color, 0, glow, 1);
            func_800D0728((GteShortVector *)burst, 500, 600, 0x18, 0, 0x1000, 0x1000, 0,
                          &color, glow, 3);
        }
        {
            int tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 4;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

#define ROOMEFFECT_ORBIT_SPIN_FUNC func_801982A4
#define ROOMEFFECT_ORBIT_SPIN_COLOR D_8018F234
#define ROOMEFFECT_ORBIT_SPIN_ORIGIN D_8019B678
#define ROOMEFFECT_ORBIT_SPIN_TRACK D_8019B4FC
#include "../room_lib/RoomEffect_OrbitSpinParticle.inc"

#undef ROOMEFFECT_ORBIT_SPIN_FUNC
#undef ROOMEFFECT_ORBIT_SPIN_COLOR
#undef ROOMEFFECT_ORBIT_SPIN_ORIGIN
#undef ROOMEFFECT_ORBIT_SPIN_TRACK

#define ROOMEFFECT_ORBIT_SPIN_BURST_FUNC func_80198914
#define ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK func_801982A4
#define ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0 D_8018F23C
#define ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1 D_8018F240
#define ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN D_8019B678
#include "../room_lib/RoomEffect_OrbitSpinBurst.inc"

#undef ROOMEFFECT_ORBIT_SPIN_BURST_FUNC
#undef ROOMEFFECT_ORBIT_SPIN_BURST_CALLBACK
#undef ROOMEFFECT_ORBIT_SPIN_BURST_COLOR0
#undef ROOMEFFECT_ORBIT_SPIN_BURST_COLOR1
#undef ROOMEFFECT_ORBIT_SPIN_BURST_ORIGIN

#define ROOMEFFECT_GLINT_SPARK_FUNC func_801991F8
#define ROOMEFFECT_GLINT_SPARK_COLOR D_8018F244
#include "../room_lib/RoomEffect_GlintSparkParticle.inc"

#undef ROOMEFFECT_GLINT_SPARK_FUNC
#undef ROOMEFFECT_GLINT_SPARK_COLOR

#define ROOMEFFECT_GLINT_RIBBON_FUNC func_80199684
#define ROOMEFFECT_GLINT_RIBBON_CALLBACK func_801991F8
#define ROOMEFFECT_GLINT_RIBBON_JOINTS D_8019B55C
#define ROOMEFFECT_GLINT_RIBBON_OFFSET0 D_8018F248
#define ROOMEFFECT_GLINT_RIBBON_OFFSET1 D_8018F250
#define ROOMEFFECT_GLINT_RIBBON_COLOR0 D_8018F258
#define ROOMEFFECT_GLINT_RIBBON_COLOR1 D_8018F25C
#define ROOMEFFECT_GLINT_RIBBON_COLOR2 D_8018F260
#include "../room_lib/RoomEffect_GlintRibbonController.inc"

#undef ROOMEFFECT_GLINT_RIBBON_FUNC
#undef ROOMEFFECT_GLINT_RIBBON_CALLBACK
#undef ROOMEFFECT_GLINT_RIBBON_JOINTS
#undef ROOMEFFECT_GLINT_RIBBON_OFFSET0
#undef ROOMEFFECT_GLINT_RIBBON_OFFSET1
#undef ROOMEFFECT_GLINT_RIBBON_COLOR0
#undef ROOMEFFECT_GLINT_RIBBON_COLOR1
#undef ROOMEFFECT_GLINT_RIBBON_COLOR2

#define ROOMEFFECT_PULSE_GLINT_FUNC func_80199C28
#include "../room_lib/RoomEffect_PulseGlintParticle.inc"

#undef ROOMEFFECT_PULSE_GLINT_FUNC

/* Mode 0 picks the two joints; mode 1 sprays a ring of sixteen sparks and
 * sixteen falling sparks at frame 15 and reports 2 after frame 33; mode 2
 * draws pulsing glows on both joints, then fading glows, a ring and a
 * floor glow. */
int func_8019A054(int mode, SceneTwinGlow *glow)
{
    GteShortVector jointA;
    GteShortVector jointB;
    GteRotation ring = D_8018F1D4;
    GteShortVector offset = D_8018F264;
    RenderColor color = D_8018F26C;
    GteShortVector floor;
    GteRotation flat;
    RoomOrbitTrailParticle *child;
    int i;
    int scale;
    int fade;
    int time;

    switch (mode) {
    case 0:
        glow->jointA = 6;
        glow->jointB = 0x12;
        glow->state = 0;
        glow->timer = 0;
        return func_800CE560(ANIM_POOL_AS(SceneTwinGlowChannel)->pool, 0x14, 0x20, func_80199C28);
    case 1:
        glow->timer++;
        if (glow->timer == 0xF) {
            for (i = 0; i < 16; i++) {
                time = i << 8;
                child = func_800CE610(ANIM_POOL_AS(SceneTwinGlowChannel)->pool);
                if (child) {
                    child->x = glow->position.x;
                    child->y = glow->position.y;
                    child->z = glow->position.z;
                    child->x += (func_80071A54() & 0xFF) - 0x80;
                    child->z += (func_80071A54() & 0xFF) - 0x80;
                    child->heading.x = rcos(time) * 60 / 4096;
                    child->heading.z = rsin(time) * 60 / 4096;
                    child->heading.y = -(func_80071A54() & 0xF) - 8;
                    child->state = 0;
                    child->timer = 0;
                }
                child = func_800CE610(ANIM_POOL_AS(SceneTwinGlowChannel)->pool);
                if (child) {
                    child->x = glow->position.x;
                    child->y = glow->position.y;
                    child->z = glow->position.z;
                    child->heading.x = func_80071A54() % 49 - 0x18;
                    child->heading.y = func_80071A54() % 128 - 0x40;
                    child->heading.z = func_80071A54() % 49 - 0x18;
                    child->state = 1;
                    child->timer = 0;
                }
            }
        }
        if (glow->timer < 0x22) break;
        return 2;
    case 2:
        {
            RenderMatrixSlot *matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneTwinGlowChannel)->pool, glow->jointA, &offset, &jointA);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(SceneTwinGlowChannel)->pool, glow->jointB, &offset, &jointB);
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int tpage = D_800E2850[TEXTURE_SLOT_RECORD(D_800E11EA)];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x20;
            D_800F3368.tpage = tpage;
        }
        if (glow->timer < 0xE) {
            scale = rsin(glow->timer << 5) / 2 + 0x800;
            if (glow->timer < 4) fade = glow->timer << 5;
            else fade = ((glow->timer & 1) << 6) + 0x80;
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              GetClut(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale, scale, 8, GetClut(0x10, palette), 3,
                              fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              GetClut(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale, scale, 8, GetClut(0x10, palette), 3,
                              fade, 0);
            }
        } else if (glow->timer < 0x23) {
            if (glow->timer == 0xE) {
                glow->position.x = jointA.x;
                glow->position.y = jointA.y;
                glow->position.z = jointA.z;
                glow->position.y = g_RoomFloorY->raw;
            }
            time = glow->timer - 0xE;
            scale = rsin((time << 10) / 20) + 0x1000;
            fade = 0x80 - (time << 7) / 20;
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              GetClut(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointA, 0, scale, scale, 8, GetClut(0x10, palette), 3,
                              fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x20;
                D_800F3368.parameter02 = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale * 2 / 3, scale * 2 / 3, 0x98,
                              GetClut(0x10, palette), 1, fade, 0);
            }
            {
                int kind;
                int palette;
                D_800F3368.parameter00 = 0x40;
                D_800F3368.parameter02 = 4;
                D_800F3368.extent_x = 0x40;
                D_800F3368.extent_y = 0x40;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&jointB, 0, scale, scale, 8, GetClut(0x10, palette), 3,
                              fade, 0);
            }
            func_800D0728(&glow->position, 800, 400, 0x14, &ring, scale, scale, &color, 0,
                          fade / 2, 1);
            scale = rsin(time * 1248 / 20 + 800) + 0x800;
            flat.x = 0x400;
            flat.y = 0;
            flat.z = 0;
            flat.flags = 1;
            floor.x = glow->position.x;
            floor.z = glow->position.z;
            floor.y = g_RoomFloorY->raw;
            {
                int kind;
                int palette;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                func_800CEE20(&floor, &flat, scale, scale, 8, GetClut(0x10, palette), 1,
                              fade * 2 / 3, 0);
            }
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

#define ROOMEFFECT_JITTER_SIZED_FUNC func_8019AC30
#define ROOMEFFECT_JITTER_SIZED_ROTATION D_8018F1CC
#include "../room_lib/RoomEffect_JitterSizedSpark.inc"

#undef ROOMEFFECT_JITTER_SIZED_FUNC
#undef ROOMEFFECT_JITTER_SIZED_ROTATION

#define ROOMEFFECT_JITTER_SPAWNER_FUNC func_8019AE8C
#define ROOMEFFECT_JITTER_SPAWNER_CALLBACK func_8019AC30
#include "../room_lib/RoomEffect_JitterSizedSpawner.inc"

#undef ROOMEFFECT_JITTER_SPAWNER_FUNC
#undef ROOMEFFECT_JITTER_SPAWNER_CALLBACK

u32 *func_8019B1D8(void) {
    return (u32 *)&D_8019B668;
}

s16 *func_8019B1E8(int unused, s16 first, s16 second) {
    s16 *result = &D_8019B66C;

    *result = first;
    D_8019B66E = second;
    return result;
}

u32 *func_8019B204(void) {
    return (u32 *)&D_8019B668;
}

u32 *func_8019B214(void) {
    return (u32 *)&D_8019B668;
}

u32 *func_8019B224(void) {
    return (u32 *)&D_8019B668;
}

u32 *func_8019B234(void) {
    return (u32 *)&D_8019B668;
}

u32 *func_8019B244(void) {
    return (u32 *)&D_8019B668;
}
