/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/player_entity.h"
#include "pe1/battle.h"
#include "pe1/entity_animation.h"
#include "pe1/asset_find.h"


int rand(void);
s16 Entity_ApplyHitAndSetAnim(void *arg0);

void Entity_ResolveDropTable(void *arg0) {
    Combatant *state;
    EnemyCombatant *entry;
    u16 scale;
    u32 flags;
    u32 word;
    int masked;
    int value;
    int roll;

    state = g_ActiveActor;
    scale = (u16)state->stat20 / 5U;
    entry = *(EnemyCombatant **)arg0;
    flags = state->stateFlags;

    if (flags & 0x1000) {
        scale = (u16)(((u16)scale * 3) / 10);
    }
    if (flags & 0x100) {
        scale = (u16)(((u16)scale * 3) / 10);
    }

    word = entry->coreFlags;
    if ((int)((word >> 21) & 7) < 3) {
        EnemyActionEffect *action;

        action = entry->effect;
        masked = state->attributes->parameterWord.fields.first;
        if (action->category == 0) {
            action->state = 4;
        } else if (action->category != 1) {
            action->state = 3;
        }
    } else {
        int secondaryParameter = state->attributes->parameterWord.fields.second;
        masked = secondaryParameter;
    }

    value = entry->effect->power - (u16)scale - masked;
    roll = rand();

    {
        Combatant *chance_state;
        int chance;

        roll %= 100;
        chance_state = g_ActiveActor;
        chance = ((int)entry->effectChance *
                  (100 - (int)chance_state->attributes->parameterWord.fields.third)) /
                 100;
        if (roll < chance) {
            value = value * 3 / 2;
            chance_state->stateFlags |= 0x8000;
        }
    }

    {
        Combatant *after_state;

        after_state = g_ActiveActor;
        if (!(after_state->stateFlags & 0x200)) {
            if (after_state->atbGauge > 0) {
                value /= 2;
            }
            if (value > 0) {
                EnemyActionEffect *action;

                action = entry->effect;
                if (action->effectType == 0xA) {
                    value /= 2;
                }
                after_state->curHP = after_state->curHP - value;
            }
            if ((s16)g_ActiveActor->curHP != 0) {
                Entity_ApplyHitAndSetAnim(arg0);
                if (!(g_PlayerEntity->flags & 0x100)) {
                    g_BattlePendingEnemySpawn[0] = arg0;
                }
            }
        } else if (value > 0) {
            EnemyActionEffect *action;

            action = entry->effect;
            if (action->effectType == 0xA) {
                value /= 2;
            }
            after_state->exp_or_acc -= value * ((after_state->maxAtk * 4) / (s16)after_state->maxHP);
        }
    }

    if (entry->effect->effectType != 0) {
        Battle_ApplyEnemyAttack(entry);
    }
}

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
u16 D_8009D298;

int Battle_CalcRelativeAngle(BattleEntity *arg0, BattleEntity *actor);

static inline FieldActor *CurrentPlayer(void) {
    return g_PlayerEntity;
}

s16 Entity_ApplyHitAndSetAnim(void *arg0) {
    void *saved_arg;
    s32 value;
    FieldActor *switch_actor;
    FieldActor *actor;
    Combatant *state;
    short bucket;
    u32 flags;

    saved_arg = arg0;
    value = 0;
    switch_actor = CurrentPlayer();

    switch (switch_actor->mode) {
    case 6:
    case 8:
    case 10:
    case 12:
    case 13:
    case 14:
    case 15: {
        FieldActor *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = case_actor->mode;
        g_BattleHitActionMode[0] = mode;
        submode = case_actor->action;
        D_8009D298 = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = *(u32 *)&case_actor->anim;
        break;
    }
    case 7:
    case 9:
    case 11: {
        FieldActor *case_actor;
        u8 mode;
        u8 submode;

        case_actor = CurrentPlayer();
        mode = case_actor->mode;
        g_BattleHitActionMode[0] = mode;
        submode = case_actor->action;
        D_8009D298 = 1;
        g_BattleHitActionSubmode[0] = submode;
        g_BattleHitActionParam[0] = case_actor->action << 16;
        break;
    }
    }

    state = g_ActiveActor;
    if ((state->stateFlags & 0x12000) == 0) {
        value = Battle_CalcRelativeAngle(saved_arg, CurrentPlayer());
        if ((s16)value < 0x200) {
            bucket = 0;
        } else if ((s16)value < 0x600) {
            bucket = 2;
        } else if ((s16)value < 0xA00) {
            bucket = 1;
        } else if ((s16)value < 0xE00) {
            bucket = 3;
        } else {
            bucket = 0;
        }

        Entity_SetActionMode((BattleEntity *)CurrentPlayer(), bucket);

        {
            FieldActor *effect_actor;

            effect_actor = CurrentPlayer();
            Asset_Find08Alt(0x46A, 0, Pe1Fixed_Integer(&effect_actor->pos_x), Pe1Fixed_Integer(&effect_actor->pos_y), Pe1Fixed_Integer(&effect_actor->pos_z));
        }

        {
            FieldActor *flag_actor;

            flag_actor = CurrentPlayer();
            flags = flag_actor->flags;
            actor = flag_actor;
        }
        if (flags & 0x100) {
            actor->flags = flags & ~0x100;
            D_8009D298 = 2;
        }
    }

    return (s16)value;
}
