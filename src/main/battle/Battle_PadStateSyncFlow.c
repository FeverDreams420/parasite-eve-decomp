/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"

/* Copy the action the battle menu chose into the player's action record. */
void Battle_CopyPadStateToRecord(void) {
    BattleAction *record = D_8009D278->action;
    BattleAttackBits *attack = (BattleAttackBits *)&record->attackWord;
    BattleTurnBits *turn = (BattleTurnBits *)&record->turnWord;

    record->field00 = D_800A76D8.field00;
    record->range = D_800A76D8.range;
    record->field04 = D_800A76D8.field04;
    record->actionCode.actionId = D_800A76D8.actionCode.actionId;
    record->field08 = D_800A76D8.field08;
    attack->count = D_800A76E4.count;
    attack->power = D_800A76E4.power;
    attack->category = D_800A76E4.category;
    turn->actions = D_800A76E8.actions;
    turn->slot = D_800A76E8.slot;
    turn->mode = D_800A76E8.mode;
    turn->element8 = D_800A76E8.element8;
    turn->element9 = D_800A76E8.element9;
    turn->element10 = D_800A76E8.element10;
    turn->element11 = D_800A76E8.element11;
    turn->element12 = D_800A76E8.element12;
    turn->hitState = D_800A76E8.hitState;
    turn->flag15 = D_800A76E8.flag15;
    turn->flag16 = D_800A76E8.flag16;
    turn->flag17 = D_800A76E8.flag17;
}

/* Copy the enemy attributes the menu record carries into the active actor. */
void Battle_SyncEnemyAttributes(void) {
    BattleAttributes *source = (BattleAttributes *)&D_800A76D8;
    BattleAttributes *attributes = g_ActiveActor->attributes;
    BattleEffectBits *effects = (BattleEffectBits *)&attributes->effectFlags;

    attributes->parameterWord.fields.first = source->parameterWord.fields.first;
    attributes->parameterWord.fields.second = source->parameterWord.fields.second;
    attributes->parameterWord.fields.third = source->parameterWord.fields.third;
    attributes->parameterWord.fields.reserved = source->parameterWord.fields.reserved;
    effects->resistPoison = D_800A76DC.resistPoison;
    effects->resistConfusion = D_800A76DC.resistConfusion;
    effects->resistDarkness = D_800A76DC.resistDarkness;
    effects->resistStiffness = D_800A76DC.resistStiffness;
    effects->resistSteal = D_800A76DC.resistSteal;
    effects->armorMode = D_800A76DC.armorMode;
    effects->mod14 = D_800A76DC.mod14;
    effects->mod16 = D_800A76DC.mod16;
    effects->mod11 = D_800A76DC.mod11;
    effects->mod17 = D_800A76DC.mod17;
    effects->mod12 = D_800A76DC.mod12;
    effects->autoMedicine = D_800A76DC.autoMedicine;
    effects->autoCure = D_800A76DC.autoCure;
    effects->mod13 = D_800A76DC.mod13;
    effects->mod15 = D_800A76DC.mod15;
}
