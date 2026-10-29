/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/akao.h"
#include "pe1/aya.h"
#include "pe1/battle.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_runtime.h"
#include "pe1/game_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_sound.h"
#include "pe1/player_entity.h"
#include "pe1/stat_modifiers.h"
#include "pe1/textbox.h"
extern struct BattleEntity *D_8009CD10;

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvBonusSlotCount;



void Inv_RecalcSlotStats(void) {
    s16 *p;
    int level, hp;
    Combatant *actor;
    AyaLevelStats *row;

    if (D_8009CD10 && (actor = D_8009CD10->core)) {
        p = g_AyaSaveState.stats.levels;
        Aya_StatQueryLevelAndSubLevel(0, *p++, &level, 0);
        row = Aya_LookupLevelStats(level);
        hp = ((g_StatScaleBase[0] + 20) * row->hp) / 20;
        actor->maxHP = g_AyaSaveState.max_hp = hp;
        if ((s16)actor->maxHP < (s16)actor->hpMirror)
            actor->hpMirror = actor->maxHP;
        if (g_AyaSaveState.max_hp < g_AyaSaveState.current_hp)
            g_AyaSaveState.current_hp = g_AyaSaveState.max_hp;

        Aya_StatQueryLevelAndSubLevel(1, *p++, &level, 0);
        actor->stat1E = Aya_LookupLevelStats(level + g_StatScaleBase[1])->offense;
        Aya_StatQueryLevelAndSubLevel(2, *p++, &level, 0);
        actor->stat20 = Aya_LookupLevelStats(level + g_StatScaleBase[2])->defense;
        Aya_StatQueryLevelAndSubLevel(3, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + g_StatScaleBase[3]);
        actor->maxAtk = row->battleMaxAtk;
        actor->atbRate = row->battleAtbRate;
        actor->atbStep = row->battleAtbStep;
        Aya_StatQueryLevelAndSubLevel(4, *p++, &level, 0);
        row = Aya_LookupLevelStats(level + g_StatScaleBase[4]);
        actor->statusStep3C = row->statusStep3C;
        actor->statusStep3E = row->statusStep3E;
        Aya_StatQueryLevelAndSubLevel(5, *p++, &level, 0);
        actor->stat22 = Aya_LookupLevelStats(level + g_StatScaleBase[5])->battleStat22;
        Aya_StatQueryLevelAndSubLevel(6, *p, &level, 0);
        actor->stat26 = Aya_LookupLevelStats(level)->inventoryCapacity;
        Inv_SetAyaSlotCount(actor->stat26);
        actor->field04.fieldId04 = g_AyaSaveState.level + 1;
    }
}