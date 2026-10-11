
#include "pe1/battle.h"
#include "pe1/asset_find.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/player_entity.h"

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
