#include "pe1/battle.h"
#include "pe1/textbox.h"
#include "pe1/battle_runtime.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* COMMON supplies the small-data sizes of the battle queue words to
 * MASPSX; every other word stays absolute. */
u8 D_8009CE38[4];
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3B;
u8 D_8009CE40;
u8 D_8009CE44;
u8 D_8009CE60;
u8 D_8009D1DC;
int D_8009D200;
s32 D_8009D208;
s32 D_8009D258;
u8 D_8009D294;
u8 D_8009D2D8;

void Pm_StopAll(void);
void Battle_InitFadeVars(void);

void Battle_Init(void)
{
    u8 index;
    BattleInitSlot *records;
    BattleAction **action;
    BattleAction *config0;
    BattleAction *config1;
    BattleAction *config2;
    BattleAction *config3;
    BattleAction *config4;
    BattleAction *config5;

    Pm_StopAll();
    index = 0;
    records = D_800BE830;
    D_8009D200 = -1;
    D_8009D2FC = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;

    do {
        D_800BE830[index].actor = 0;
        records[index].actionGeneration = 0;
        D_800BE830[index].command = 0;
        index++;
    } while (index < 45);

    action = &D_8009D278->action;
    config0 = *action;
    D_8009CE44 = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;
    config1 = *action;
    D_8009D2D8 = (config0->turnWord >> 4) & 3;
    config2 = *action;
    D_8009D1DC = config1->turnWord & 0xF;
    D_8009CE38[0] = config2->animMode[0];
    config3 = *action;
    D_8009CE39 = config3->animMode[1];
    config4 = *action;
    D_8009CE3A = config4->animMode[2];
    config5 = *action;
    D_8009CE3B = config5->animMode[3];

    Tbl_ResetAll();
    D_8009D1CE = 0;
    D_8009D1AC &= ~0x300;
    Battle_InitFadeVars();
    D_8009CE60 = 0;
}
