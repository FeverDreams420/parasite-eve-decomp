/* MASPSX_FLAGS: -G8 --use-comm-section */
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/field_actor.h"
#include "pe1/game_state.h"
#include "pe1/battle_context.h"

extern int g_SceneDispatchToken;
extern int g_PlayTimeFrameCounter;

int Str_EncodeBase32(char *out, unsigned int value);

int Task_EncodeBarcode(int **arg0) {
    int tmp[2];
    int value;
    int *counter;

    Str_EncodeBase32(&tmp[0], *arg0[0]);
    value = *arg0[0];
    g_GameStateFlags |= 0x2000;
    g_SceneDispatchToken = value;

    if (*arg0[0] == 0xA9400048) {
        counter = &g_PlayTimeFrameCounter;
        if (*counter == 0x7D0) {
            *counter = 0;
        } else {
            Sys_Shutdown();
        }
    }
    return 0;
}

void Render_SetScrollMode(int arg0, int arg1, unsigned int arg2, int arg3);

int Task_SetFogParams(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 0);
    return 1;
}

int Task_SetFogParamsAlt(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 8);
    return 1;
}

int Task_IsBattlePhase4(int **arg0) {
    *arg0[0] = ((g_RenderStateFlags & 7) == 4);
    return 1;
}

extern FieldActor *g_PlayerEntity[];

int Task_ResetPlayerCameraPos(void) {
    Render_UpdateScrollPosition(&g_PlayerEntity[0]->pos_x, -1, -1);
    return 1;
}

int Task_SetBattleStateMode(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr = (*ptr & -8) | 0x80;
    return 1;
}

extern FieldActor *g_CurrentEntity;

int Task_SetEntityField224(short **arg0) {
    FieldActor *entity;
    int value;

    entity = g_CurrentEntity;
    value = (*arg0)[1];
    entity->render_object.hit_cylinder.radius = value;
    return 1;
}

extern int g_FieldMoveLock;

int Entity_ClearGlobalFlag1(void) {
    g_FieldMoveLock &= -2;
    return 1;
}

int Entity_SetGlobalFlag1(void) {
    g_FieldMoveLock |= 1;
    return 1;
}

int Entity_SetCurrentFlag40(void) {
    g_CurrentEntity->flags |= 0x40;
    return 1;
}

int Entity_ClearCurrentFlag40(void) {
    g_CurrentEntity->flags &= -0x41;
    return 1;
}

/* Script opcodes: read the menu choice and branch the actor script on
 * inequality. Contiguous pair; the choice reader has no data, so the
 * branch's assembler options leave it unchanged. */

int g_SceneDataTable0;

int Menu_GetEquipSlotIndex(void);

int Task_GetMenuChoice(int **arg0) {
    int value;

    value = Menu_GetEquipSlotIndex();
    **arg0 = (signed char)value;
    return 1;
}

int Task_CondBranch(int **arg0) {
    int *a;
    int *b;
    int *c;

    a = arg0[0];
    b = arg0[1];
    if (*a != *b) {
        c = arg0[2];
        g_SceneDataTable0 = (int)(g_CurrentEntity->script_base + (*c * 2));
    }
    return 1;
}

/* Contiguous current-actor flag and animation opcodes. */

int Entity_TestCurrentFlags(int **arg0) {
    FieldActor *entity;
    int flags;
    int mask;

    entity = g_CurrentEntity;
    flags = entity->flags;
    mask = *arg0[0];
    *arg0[1] = (flags & mask) == mask;
    return 1;
}

int Task_Noop(void) {
    return 1;
}

int Task_SetEntityField1C(int **arg0) {
    g_CurrentEntity->anim_step = *arg0[0];
    return 1;
}

int Task_SetEntityAnimSpeed(u16 **arg0) {
    FieldActor *entity;
    unsigned int value;

    entity = g_CurrentEntity;
    value = **arg0;
    if (entity->action < value) {
        value = entity->action;
    }
    entity->anim.fixed = value << 16;
    return 1;
}

int Entity_SetCurrentFlag100(void) {
    g_CurrentEntity->flags |= 0x100;
    return 1;
}

int Entity_ClearCurrentFlag100(void) {
    g_CurrentEntity->flags &= -0x101;
    return 1;
}

/* Script opcodes: test, set and clear the global game-state flags, set the
 * battle mode state and read a battle context field of the current actor.
 * Contiguous default-profile handlers. */

extern int g_BattleModeState;

int Scene_TestGlobalFlags(int **arg0) {
    int flags;
    int mask;

    flags = g_GameStateFlags;
    mask = *arg0[0];
    if (flags & mask) {
        *arg0[1] = 1;
    } else {
        *arg0[1] = 0;
    }
    return 1;
}

int Scene_SetGlobalFlags(int **arg0) {
    g_GameStateFlags |= *arg0[0];
    return 1;
}

int Scene_ClearGlobalFlags(int **arg0) {
    g_GameStateFlags &= ~*arg0[0];
    return 1;
}

int Menu_SetModeState5(void) {
    g_BattleModeState = 5;
    return 1;
}

int Menu_SetModeState6(void) {
    g_BattleModeState = 6;
    return 1;
}

int Entity_GetField(char ***arg0) {
    FieldActor *ctx = g_CurrentEntity;

    if (ctx->type_id == 0) {
        *(int *)arg0[1] = Battle_GetContextField(*(unsigned char *)arg0[0]);
    } else {
        *(int *)arg0[1] = Battle_GetEnemyContextField(ctx, *(unsigned char *)arg0[0]);
    }
    return 1;
}

extern FieldActor *g_FieldActorListHead;

int Entity_FindByTypeId(int **arg0) {
    FieldActor *obj;
    register int type;
    int cmpType;

    type = *arg0[0];
    if (type == 0) {
        *arg0[3] = Battle_GetContextField(*(u8 *)arg0[2]);
    } else {
        obj = g_FieldActorListHead;
        cmpType = type;
        if (obj == 0) {
            return 1;
        }
        do {
            if (obj->type_id != cmpType) {
                obj = obj->next;
            } else if (obj->sub_id != *arg0[1]) {
                obj = obj->next;
            } else if ((obj->flags & 0x10) != 0) {
                obj = obj->next;
            } else {
                break;
            }
        } while (obj != 0);
        if (obj == 0) {
            return 1;
        }
        *arg0[3] = Battle_GetEnemyContextField(obj, *(u8 *)arg0[2]);
    }
    return 1;
}

int Entity_SetField(char ***arg0) {
    FieldActor *ctx = g_CurrentEntity;

    if (ctx->type_id == 0) {
        Battle_SetContextField(*(unsigned char *)arg0[0], *(int *)arg0[1]);
    } else {
        Entity_WriteFieldByCmd(ctx, *(unsigned char *)arg0[0], *(int *)arg0[1]);
    }

    return 1;
}

int Entity_SetFieldByTypeId(int **arg0) {
    FieldActor *obj;
    register int type;
    int cmpType;

    type = *arg0[0];
    if (type == 0) {
        Battle_SetContextField(*(u8 *)arg0[2], *arg0[3]);
    } else {
        obj = g_FieldActorListHead;
        cmpType = type;
        if (obj == 0) {
            return 1;
        }
        do {
            if (obj->type_id != cmpType) {
                obj = obj->next;
            } else if (obj->sub_id != *arg0[1]) {
                obj = obj->next;
            } else if ((obj->flags & 0x10) != 0) {
                obj = obj->next;
            } else {
                break;
            }
        } while (obj != 0);
        if (obj != 0) {
            Entity_WriteFieldByCmd(obj, *(u8 *)arg0[2], *arg0[3]);
        }
    }
    return 1;
}


int Task_ClearBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr &= -0xC1;
    return 1;
}

int Task_SetBattleFlags(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr |= 0xC0;
    return 1;
}

int Task_SetCurrentEntityField20(int **arg0) {
    g_CurrentEntity->move_factor = *arg0[0];
    return 1;
}
