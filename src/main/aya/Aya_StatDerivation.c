/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G0 --use-comm-section */
#include "pe1/aya.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_modifiers.h"

void Inv_RecalcSlotStats(void) {
    s16 *p = (s16 *)D_800C0E00.stats.levels;
    int level, hp;
    Combatant *actor;
    AyaLevelStats *row;

    Stat_QueryLevelAndSubLevel(0, *p++, &level, 0);
    row = Aya_LookupLevelStats(level);
    hp = ((D_800A1B30[0] + 20) * row->hp) / 20;
    D_800C0E00.max_hp = hp;
    if (D_8009D254 && (actor = D_8009D254->core)) {
        if ((s16)(actor->maxHP = hp) < (s16)actor->curHP)
            actor->curHP = hp;
        if ((s16)actor->maxHP < (s16)actor->hpMirror)
            actor->hpMirror = actor->maxHP;
        if (D_800C0E00.max_hp < D_800C0E00.current_hp)
            D_800C0E00.current_hp = D_800C0E00.max_hp;

        Stat_QueryLevelAndSubLevel(1, *p++, &level, 0);
        actor->stat1E = Aya_LookupLevelStats(level + D_800A1B30[1])->offense;
        Stat_QueryLevelAndSubLevel(2, *p++, &level, 0);
        actor->stat20 = Aya_LookupLevelStats(level + D_800A1B30[2])->defense;
        Stat_QueryLevelAndSubLevel(3, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + D_800A1B30[3]);
        actor->maxAtk = row->battleMaxAtk;
        actor->atbRate = row->battleAtbRate;
        actor->atbStep = row->battleAtbStep;
        Stat_QueryLevelAndSubLevel(4, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + D_800A1B30[4]);
        actor->statusStep3C = row->statusStep3C;
        actor->statusStep3E = row->statusStep3E;
        Stat_QueryLevelAndSubLevel(5, *p++, &level, 0);
        actor->stat22 = Aya_LookupLevelStats(level + D_800A1B30[5])->battleStat22;
        Stat_QueryLevelAndSubLevel(6, *p, &level, 0);
        actor->stat26 = Aya_LookupLevelStats(level)->inventoryCapacity;
        Inv_SetAyaSlotCount(actor->stat26);
        actor->field04.fieldId04 = D_800C0E00.level + 1;
    }
}

/* Reports the active-time step and rate of the current PE energy level. */
void Aya_DeriveStats(int *out_max, int *out_current) {
    int base;
    AyaLevelStats *stats;

    Stat_QueryLevelAndSubLevel(3, D_800C0E00.stats.levels[3], &base, 0);
    stats = Aya_LookupLevelStats(base + D_800A1B30[3]);

    if (out_max != 0) {
        *out_max = stats->battleAtbStep;
    }
    if (out_current != 0) {
        *out_current = stats->battleAtbRate;
    }
}

/* Recomputes the battle stats, then refills the player's current HP. */
void Battle_SyncEquipSlots(void) {
    Combatant *actor;
    u16 maxHP;

    Inv_RecalcSlotStats();
    if (D_8009D254 != 0) {
        actor = D_8009D254->core;
        if (actor != 0) {
            maxHP = actor->maxHP;
            actor->hpMirror = maxHP;
            actor->curHP = maxHP;
        }
    }
}
