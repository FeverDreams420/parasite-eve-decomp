#include "pe1/aya.h"
#include "pe1/menu_dialog.h"

/* Learns the parasite spell whose unlock key matches and announces it.
 * Returns whether the key named one of the 20 spells. */
int Aya_UnlockParasiteSpellById(int key)
{
    ParasiteSpellEntry *table;
    int spell;
    int found;

    table = Aya_GetParasiteSpellUnlockTable();
    for (spell = 0; spell < 20; spell++) {
        if (table[spell].unlockKey == key) {
            break;
        }
    }
    found = spell < 20;
    if (found) {
        g_AyaParasiteSpellFlags[0] |= 1 << spell;
        Menu_CreateParasiteSpellInfoPanel(spell);
    }
    return found;
}
