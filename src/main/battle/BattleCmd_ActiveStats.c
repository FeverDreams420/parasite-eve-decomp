#include "pe1/battle.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/aya.h"
#include "pe1/player_entity.h"

/* g_PlayerEntity points at the player-side field actor; its core is the
 * Combatant record used by the command paths below. */

void BattleCmd_SyncActiveAmmo(void) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    entity = (BattleEntity *)g_PlayerEntity;
    if (entity != 0) {
        current = entity->core;
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    entry->ammo = current->action->attackWord & 0x3FF;
                }
                Inv_SelectActiveList(saved);
            }
        }
    }
}


void BattleCmd_SetCurrentHP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->curHP = arg0;
        }
    }

    g_AyaHpCurrent[0] = arg0;
}
