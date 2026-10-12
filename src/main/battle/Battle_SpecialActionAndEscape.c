/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle.h"
#include "pe1/asset_find.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/player_entity.h"
#include "pe1/random.h"
#include "pe1/save.h"

void Battle_DispatchSpecialAction(int arg0) {
    BattleEntity *actor;

    if ((g_GameStateFlags & 2) == 0) {
        actor = (BattleEntity *)g_PlayerEntity;
        g_ActiveActor = actor->core;
        Asset_Find08Alt(0x453, 1, actor->posX.parts.integer,
                       actor->posY.parts.integer, actor->posZ.parts.integer);
    }

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        g_ActiveActor->actionMode12 = 0xD;
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    }
}

void Battle_DispatchInit(int arg0) {
    if (g_GameStateFlags & 2) {
        return;
    }

    g_ActiveActor = ((BattleEntity *)g_PlayerEntity)->core;

    switch (arg0) {
    case 0x197:
        Battle_CopyPadStateToRecord();
        Battle_SetupEnemyAnims();
        break;
    case 0x198:
        Battle_SyncEnemyAttributes();
        break;
    default:
        break;
    }
}

int Battle_RollEscapeChance(void) {
    BattleEntity *entity = (BattleEntity *)D_8009D20C;
    s16 highestRank = 0;
    /* Retail narrows the probability after every multiplier. */
    s8 result = 0;
    for (; entity; entity = entity->next) {
        EnemyCombatant *enemy;
        if (entity == D_8009D254) continue;
        enemy = entity->core;
        if (!enemy || enemy->hpAlive <= 0) continue;
        if (highestRank < (s8)enemy->field04.bytes.rank)
            highestRank = (s8)enemy->field04.bytes.rank;
        if (enemy->statusFlags2 & 0x40000) {
            result = -1;
            break;
        }
    }
    if (!result) {
        int difference = (s8)(D_8009D278->field04.bytes.rank - highestRank);
        if (difference >= 2) result = 80;
        else if (difference == 1) result = 40;
        else if (difference == 0) result = 40;
        else if (difference == -1) result = 25;
        else result = 15;
        if ((s16)D_8009D278->curHP * 10 < (s16)D_8009D278->maxHP)
            result = result * 3 / 2;
        switch (((unsigned int)D_8009D278->stateFlags >> 25) & 7) {
        case 1: result = result * 3 / 2; break;
        case 2: result *= 2; break;
        case 3: result *= 3; break;
        case 4: result *= 4; break;
        case 5: result = 100; break;
        }
        {
            int chance = result;
            result = rand() % 100 < chance;
        }
    }
    if (result <= 0) {
        Combatant *player;
        int attempts;
        D_8009D1CE = 1;
        if (result == -1)
            D_8009D1F8 = D_800915C0 + Save_GetMetadataWindowIndex() * 14;
        else
            D_8009D1F8 = D_8009159C + Save_GetMetadataWindowIndex() * 17;
        player = D_8009D278;
        attempts = ((unsigned int)player->stateFlags >> 25) & 7;
        if (attempts < 5)
            player->stateFlags = (player->stateFlags & 0xF1FFFFFF) |
                (((attempts + 1) & 7) << 25);
    }
    return result;
}
