#include "pe1/battle_runtime.h"

void Battle_SetupEnemyAnims(void) {
    EnemyAnimTable animTable;
    EnemyOffenseScaleTable scaleTable;
    BattleAction *action;
    Combatant *combatant;
    u8 *animRow;
    unsigned int actionId;

    animTable = g_BattleActionAnimRows;
    scaleTable = g_BattleActionOffenseScale;

    combatant = D_8009D278;
    action = combatant->action;
    actionId = action->actionCode.bytes.actionIdByte;
    combatant->scaledOffense =
        (combatant->stat22 * scaleTable.values[actionId]) / 10;
    combatant->actionMode12 = 4;
    D_8009D278->actionMode13 = 5;
    D_8009D278->targetAngleActionModes[0] = 6;
    D_8009D278->followUpAngleActionModes[0] = 7;
    D_8009D278->targetAngleActionModes[1] = 8;
    D_8009D278->followUpAngleActionModes[1] = 9;
    D_8009D278->targetAngleActionModes[2] = 10;
    D_8009D278->followUpAngleActionModes[2] = 11;

    animRow = animTable.rows[actionId];
    D_8009D278->action->animMode[0] = animRow[0];
    D_8009D278->action->animMode[1] = animRow[1];
    D_8009D278->action->animMode[2] = animRow[2];
    D_8009D278->action->animMode[3] = animRow[3];
    D_8009D278->action->hitTrigger = animRow[4];
    Scene_SetStoryDay(D_8009D278->action->actionCode.actionId);
}
