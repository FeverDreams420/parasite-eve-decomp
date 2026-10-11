#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/player_entity.h"

void Battle_SubActionStep(void) {
    Combatant *state;
    BattleEntity *actor;

    state = g_ActiveActor;
    if (state->subActionCounter >= state->subActionPeriod) {
        state->curHP -= state->subActionStep;
        state->hpMirror -= state->subActionStep;
        state->subActionCounter = 0;
        state->panelAux_val = state->subActionStep;
        actor = (BattleEntity *)g_PlayerEntity;
        g_ActiveActor->panelAux_x = actor->renderObject.projected_x;
        g_ActiveActor->panelAux_y = actor->renderObject.projected_y;
        g_ActiveActor->panelAux_timer = 0x1E;
        g_ActiveActor->panelAux_mode = 3;
    }

    g_ActiveActor->subActionCounter++;
}
