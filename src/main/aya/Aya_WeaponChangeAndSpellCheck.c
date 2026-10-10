#include "pe1/battle.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/aya.h"

extern void **g_PlayerEntity;

int Inv_IsActiveListOverrideSelected(void);

void BattleCmd_ChangeWeaponAndSync(int arg0) {
    Combatant *current;
    ItemDataRecord *entry;
    int saved;

    Battle_ApplySpellEffect(arg0, (BattleEntity *)g_PlayerEntity);
    if (g_PlayerEntity != 0) {
        current = g_PlayerEntity[0];
        if (current != 0) {
            g_AyaHpCurrent[0] = current->curHP;
            if (*(void **)&current->action != 0) {
                saved = Inv_IsActiveListOverrideSelected();
                Inv_SelectActiveList(0);
                entry = Inv_LookupActiveListData(g_AyaEquippedWeaponSlot[0]);
                if (entry != 0) {
                    *(short *)&entry->ammo = *(int *)&((BattleAction *)*(void **)&current->action)->attackWord & 0x3FF;
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
