#include "common.h"
#include "pe1/battle.h"

void Entity_CheckActionIdMatch(BattleEntity *arg0, void *arg1, EnemyCombatant **arg2, int arg3) {
    EnemyCombatant *entry;
    int i;
    int kind;
    int offset;
    int value;

    entry = *arg2;
    if (entry == 0) {
        return;
    }

    if (entry->effect->state != 1) {
        return;
    }

    kind = (entry->coreFlags >> 21) & 7;
    if (kind >= 3) {
        return;
    }

    if (entry->hpAlive <= 0) {
        return;
    }

    i = 0;
    arg3 = (short)arg3;
    do {
        offset = (entry->coreFlags >> 19) & 0x1C;
        value = (entry->contactActionIds + offset)[i & 0xFFFF];
        if (arg3 == value) {
            ((Combatant *)arg0->core)->stateFlags |= 0x4000;
            entry->coreFlags |= 0x80000000;
            return;
        }

        if (value < 0) {
            return;
        }

        i++;
    } while ((u16)i < 4);
}
