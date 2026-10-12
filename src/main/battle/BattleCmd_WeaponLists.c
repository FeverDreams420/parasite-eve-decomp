/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/inventory_slots.h"
#include "pe1/inventory_queries.h"
#include "pe1/weapon_list_output.h"
#include "pe1/battle_modifiers.h"
#include "pe1/aya.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int D_8009D018;

static inline int ClampStat(ItemDataRecord *item, int which) {
    int value = item->baseStats[which] + item->bonusStats[which];
    int result = 999;
    if (value < 1000) result = value;
    return result;
}
void Inv_BuildWeaponList(int unused, WeaponListOutput *out) {
    ItemDataRecord *item;
    int i, kind, code, root, shift, trial, radicand;
    int product;
    Inv_SelectActiveList(0);
    item = Inv_LookupActiveListData(D_800C0E20.tracked[0]);
    if (item->baseStats[0] + item->bonusStats[0] >= 1000) out->attack = 999;
    else out->attack = item->baseStats[0] + item->bonusStats[0];
    {
        int base2 = item->baseStats[1];
        int bonus2 = item->bonusStats[1];
        if (base2 + bonus2 < 1000) product = (base2 + bonus2) * 22500;
        else product = 0x156fabc;
        radicand = product;
    }
    root = 0;
    for (shift = 30; shift >= 0; shift -= 2) {
        trial = ((root << 2) + 1) << shift;
        root <<= 1;
        if (radicand >= trial) { radicand -= trial; root |= 1; }
    }
    out->range = root;
    out->kind = item->kind;
    out->packed.bits.count = item->ammo;
    out->packed.bits.power = ClampStat(item, 2);
    kind = item->kind;
    if (kind != 0 && (unsigned)kind < 8) {
        code = kind - 4;
        if (code <= 0) {
            code = 1;
        }
    } else {
        code = 0;
        if ((unsigned)kind >= 19) code = kind - 18;
    }
    out->effects.word = 0x11;
    out->packed.bits.category = code;
    for (i = 0; i < item->tailCount; i++) {
        int effect = item->tailData[i] & 31;
        switch (effect) {
        case 1: case 2: case 3: case 4: case 5:
            out->effects.bits.actions = g_WeaponEffectCodes[effect];
            break;
        case 6: case 7: case 8:
            out->effects.bits.mode = effect - 5;
            break;
        case 9: out->effects.bits.element8 = 1; break;
        case 10: out->effects.bits.element9 = 1; break;
        case 11: out->effects.bits.element10 = 1; break;
        case 12: out->effects.bits.element11 = 1; break;
        case 13: out->effects.bits.element12 = 1; break;
        case 14: out->effects.bits.flag15 = 1; break;
        case 15: out->effects.bits.flag16 = 1; break;
        case 16: case 17:
            out->effects.bits.hitState = effect - 15;
            break;
        case 18: out->effects.bits.flag17 = 1; break;
        case 19: case 20:
            out->effects.bits.slot = effect - 17;
            break;
        }
    }
    if (out->effects.bits.hitState == 1) out->attack = (s16)out->attack >> 1;
}

void BattleCmd_LoadWeaponModifiers(void)
{
    int saved = Inv_IsActiveListOverrideSelected();
    int i;
    ItemDataRecord *item;
    Inv_SelectActiveList(0);
    D_8009D018 = 0;
    for (i = 6; i >= 0; --i) D_800A1B30[i] = 0;
    /* Historical function name notwithstanding, this is the armor index. */
    item = Inv_LookupActiveListData(D_800C0E22[0]);
    if (item) {
        for (i = 0; i < item->tailCount; ++i) {
            unsigned mod = item->tailData[i] & 31;
            /* These are decoded low-five-bit values, not full ItemMod IDs. */
            switch (mod) {
            case 8: case 9: case 10: D_8009D018 = 1 << (mod - 8); break;
            case 11: D_800A1B30[0] = 3; break;
            case 12: D_800A1B30[1] = 2; break;
            case 13: D_800A1B30[5] = -2; break;
            case 15: D_800A1B30[1] = -2; break;
            }
        }
    }
    Inv_RecalcSlotStats();
    Inv_SelectActiveList(0);
    Inv_SelectActiveList(saved);
}

int Battle_GetScaledMaxHP(int level) {
    unsigned short maxHp;

    maxHp = Aya_LookupLevelStats(level)->hp;
    return ((D_800A1B30[0] + 0x14) * maxHp) / 20;
}
