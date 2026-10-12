#include "pe1/battle.h"
#include "pe1/battle_runtime.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/aya.h"

/* Applies a weapon change, then writes the loaded ammo back to the item. */
void BattleCmd_ChangeWeaponAndSync(int arg0) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, D_8009D254);
    if (D_8009D254 != 0) {
        current = D_8009D254->core;
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

int Aya_HasParasiteSpell(int spell)
{
    return (g_AyaParasiteSpellFlags[0] >> spell) & 1;
}
