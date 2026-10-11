#include "common.h"
#include "pe1/battle.h"

#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/psyq_libc.h"


void Battle_CheckDropChance(void)
{
    Combatant *ctx;
    int flags;
    u16 chance;

    ctx = g_ActiveActor;
    flags = ctx->action->turnWord;
    if (flags & 0x10000) {
        chance = ctx->stat22;
        if ((rand() % 100) < chance) {
            g_ActiveActor->hpAlive = 0x2328;
        }
    }
}


void Battle_RollEnemySpawn(void *arg0)
{
    EnemyCombatant *ctx;
    int diff;
    int threshold;
    int bonus;

    ctx = ((BattleEntity *)arg0)->core;
    if (ctx->hpAlive <= 0) {
        return;
    }

    diff = (s8)(g_ActiveActor->field04.bytes.rank -
                ctx->field04.bytes.rank);
    if (diff > 0) {
        threshold = Battle_GetAgilityBonus();
        if ((rand() % 100) * 10 < threshold) {
            Battle_SetupEntityTarget((BattleEntity *)arg0);
        }
    } else if (diff == 0) {
        bonus = Battle_GetAgilityBonus();
        threshold = bonus * 2;
        threshold += bonus;
        if ((rand() % 100) * 50 < threshold) {
            Battle_SetupEntityTarget((BattleEntity *)arg0);
        }
    } else if (diff == -1) {
        bonus = Battle_GetAgilityBonus();
        threshold = bonus * 2;
        threshold += bonus;
        if ((rand() % 100) * 100 < threshold) {
            Battle_SetupEntityTarget((BattleEntity *)arg0);
        }
    } else if (diff < -1) {
        threshold = Battle_GetAgilityBonus();
        if ((rand() % 100) * 50 < threshold) {
            Battle_SetupEntityTarget((BattleEntity *)arg0);
        }
    }
}
