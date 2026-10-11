#include "pe1/battle.h"
#include "pe1/textbox.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */

typedef struct GlobalWordObject {
    s32 value;
    u8 pad[12];
} GlobalWordObject;

typedef struct GlobalByteObject {
    u8 value;
    u8 pad[15];
} GlobalByteObject;

typedef struct BattleRootObject {
    u8 *root;
    u8 pad[12];
} BattleRootObject;

#include "Battle_Init_externs.h"
u8 D_8009CE38[4];
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3B;
#include "Battle_Init_externs_1.h"

void Pm_StopAll(void);
void Battle_InitFadeVars(void);

void Battle_Init(void)
{
    u8 index;
    BattleInitSlot *records;
    BattleAction **action;
    BattleAction *config0;
    BattleAction *config1;
    volatile u8 *config2;
    volatile u8 *config3;
    volatile u8 *config4;
    volatile u8 *config5;

    Pm_StopAll();
    index = 0;
    records = D_800BE830;
    D_8009D200 = -1;
    D_8009D2FC_o.value = -1;
    D_8009D258 = -1;
    D_8009D208 = -1;

    do {
        D_800BE830[index].actor = 0;
        records[index].actionGeneration = 0;
        D_800BE830[index].command = 0;
        index++;
    } while (index < 45);

    action = &((Combatant *)D_8009D278_o.root)->action;
    config0 = *action;
    D_8009CE44 = 0;
    D_8009CE40 = 0;
    D_8009D294 = 0;
    config1 = *action;
    D_8009D2D8 = (config0->turnWord >> 4) & 3;
    config2 = (u8 *)*action;
    D_8009D1DC = config1->turnWord & 0xF;
    D_8009CE38[0] = config2[0x14];
    config3 = (u8 *)*action;
    D_8009CE39 = config3[0x15];
    config4 = (u8 *)*action;
    D_8009CE3A = config4[0x16];
    config5 = (u8 *)*action;
    D_8009CE3B = config5[0x17];

    Tbl_ResetAll();
    D_8009D1CE_o.value = 0;
    D_8009D1AC_write.value = D_8009D1AC_read.value & ~0x300;
    Battle_InitFadeVars();
    D_8009CE60 = 0;
}
