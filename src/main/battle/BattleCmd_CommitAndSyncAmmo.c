
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/aya.h"
#include "pe1/player_entity.h"

void BattleCmd_SetCurrentMP(int arg0) {
    Combatant *current;

    if (g_PlayerEntity != 0) {
        current = ((BattleEntity *)g_PlayerEntity)->core;
        if (current != 0) {
            current->exp_or_acc = arg0 << 16;
        }
    }
}

void BattleCmd_CommitAndSyncAmmo(int item) {
    BattleEntity *entity;
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplyDamage(item);
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

