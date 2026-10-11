/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_palette.h"
#include "pe1/battle.h"
#include "pe1/battle_start.h"

/* COMMON supplies small-data metadata to MASPSX; the other battle words
 * stay absolute. cc1 runs at -G0 so the story-day flags byte (D_800B0CE6)
 * counts as an expensive address and stays in a register for its
 * read-modify-write, as retail has it. */
u8 D_8009CE74, D_8009CE78, D_8009CE7C;
int D_8009D1D0;
int D_8009D1E8;
u16 D_8009D21C;
u8 D_8009D244;
Combatant *D_8009D278;
u8 D_8009D288;
int D_8009D28C;
s8 D_8009D2A0;
u8 D_8009D2EC;
u32 D_8009D304;


void Battle_ResetEnemyStats(int mode) {
    Combatant *actor;
    Combatant *clear_actor;
    u32 clear_flags;

    actor = D_8009D278;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    D_8009D278->actionMode12 = 4;
    actor = D_8009D278;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    D_8009D1D0 = 0;

    if ((mode & 0xFF) == 1) {
        D_8009D234[0] = 0x5A;
        D_8009D244 = 1;
        actor->stateFlags |= 0x800000;
    } else {
        u32 state = actor->stateFlags;
        D_8009D244 = 0;
        state &= 0xFFBFFFFF;
        state &= 0xFF7FFFFF;
        actor->stateFlags = state;
    }

    clear_actor = D_8009D278;
    clear_flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    clear_flags &= -4;
    clear_flags &= -0xD;
    clear_flags &= -0x31;
    clear_flags &= -0xC1;
    clear_flags &= -0x101;
    clear_flags &= -0x201;
    clear_flags &= -0x401;
    clear_flags &= -0x801;
    clear_flags &= -0x1001;
    clear_flags &= -0x2001;
    clear_flags &= -0x4001;
    clear_flags &= 0xFFFF7FFF;
    clear_flags &= 0xFFF7FFFF;
    clear_flags &= 0xFFFEFFFF;
    clear_flags &= 0xFFF9FFFF;
    clear_flags &= 0xFFEFFFFF;
    clear_flags &= 0xFFDFFFFF;
    clear_flags &= 0xFEFFFFFF;
    clear_flags &= 0xF1FFFFFF;
    clear_flags &= 0xEFFFFFFF;
    clear_flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = clear_flags;

    D_8009D278->panelB_timer = 0;
    D_8009D278->panelAux_timer = 0;
    D_8009D2E8 &= ~0x10;
    Battle_FlushScriptSounds();
    Tbl_ResetAll();
    D_8009D298 = 0;
    D_8009D29A = 0;
    D_8009D29B[0] = 0;
    D_8009D29C[0] = 0;
}




void Battle_SetupPlayerPalette(void) {
    int c2;
    int c3;
    int c4;
    int c5;
    int c6;
    register int c7 asm("$7");
    u8 *flags;

    c6 = 0x82;
    c5 = 0x36;
    c4 = 0x4A;
    c2 = 0xFF;
    asm("" : : "r"(c2));
    c3 = 0x3B;
    c7 = 0x3D;
    asm("" : : "r"(c2), "r"(c7));

    D_800B0135[0] = c6;
    D_800B0145[0] = c6;
    D_800B017D[0] = c6;
    D_800B018D[0] = c6;

    c6 = 0x81;
    D_800B0136[0] = c5;
    D_800B0146[0] = c5;
    D_800B017E[0] = c5;
    D_800B018E[0] = c5;

    c5 = 0x83;
    D_800B013C[0] = c4;
    D_800B014C[0] = c4;
    D_800B0184[0] = c4;
    D_800B0194[0] = c4;

    c4 = 0x13;
    D_800B013E[0] = c3;
    D_800B014E[0] = c3;
    D_800B0186[0] = c3;
    D_800B0196[0] = c3;

    c3 = 1;
    D_800B0134[0] = 0;
    D_800B013D[0] = c2;
    D_800B0144[0] = 0;
    D_800B014D[0] = c2;
    D_800B017C[0] = 0;
    D_800B0185[0] = c2;
    D_800B018C[0] = 0;
    D_800B0195[0] = c2;

    D_800B0158[0] = c2;
    D_800B0159[0] = c7;
    D_800B015A[0] = c6;
    D_800B0160[0] = c5;
    D_800B0161[0] = c4;
    D_800B0162[0] = c3;
    D_800B0168[0] = c2;
    D_800B0169[0] = c7;
    D_800B016A[0] = c6;
    D_800B0170[0] = c5;
    D_800B0171[0] = c4;
    D_800B0172[0] = c3;
    D_800B01A0[0] = c2;
    D_800B01A1[0] = c7;
    D_800B01A2[0] = c6;
    D_800B01A8[0] = c5;
    D_800B01A9[0] = c4;
    D_800B01AA[0] = c3;
    D_800B01B0[0] = c2;
    D_800B01B1[0] = c7;
    D_800B01B2[0] = c6;
    D_800B01B8[0] = c5;
    D_800B01B9[0] = c4;
    D_800B01BA[0] = c3;

    Battle_FlushScriptSounds();
    D_8009D2EC = 0;
    D_8009D2A0 = 0;
    D_8009D254->actionCheckFn = (void *)D_800915E0[0];
    D_8009D1A0 &= -3;
    D_8009D2E8 &= -0x11;
    flags = &D_800B0CE6;
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}


void Battle_StartEncounter(int mode) {
    int i;
    int neg_one;
    BattleEntity *entity;
    Combatant *actor;
    u32 value;
    int mode_reg;
    u32 initial_flags;
    entity = D_8009D254;
    D_8009D278 = (Combatant *)entity->core;
    D_8009D1E8 = 0;
    *(u32 *)&D_8009D290 = 0;
    D_8009D28C = 0;
    D_8009CE7C = 0;
    D_8009CE78 = 0;
    D_8009D288 = 0;
    D_8009CE74 = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    *(u8 *)&D_8009D1AC = 0;
    initial_flags = D_8009D1AC;
    D_8009D1A8.summary = 0;
    D_8009D1CE = 0;
    D_8009D235[0] = 0;
    D_8009D304 = 0;
    D_8009D21C = 0;
    initial_flags &= -0x301;
    D_8009D1AC = initial_flags;

    for (; (u8)i < 0xA; i++) {
        D_800A7FF0[(u8)i].id = 0;
        D_800A7FF0[(u8)i].extra = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        D_800B8A90[(u8)i] = 0;
    }

    srand(D_8009D250);
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = D_8009D278;
    value = actor->exp_or_acc;
    if ((s32)value <= 0) {
        actor->exp_or_acc = 0x10000;
    } else {
        u32 max_value;

        max_value = actor->maxAtk;
        if ((s32)value >= (s32)max_value) {
            actor->exp_or_acc = max_value;
            actor->atbGauge = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        BattleEntity *post_entity;
        RenderMatrix *post_actor;
        u32 callback;
        post_entity = D_8009D254;
        post_actor = post_entity->renderObject.matrices;
        callback = (u32)Entity_CheckActionIdMatch;
        post_entity->actionCheckFn = (void *)callback;
        D_8009D27C = post_actor->translation[1] - 0x64;
    }
    Window_SetBoundsByMode(mode_reg & 0xFF);
    {
        Combatant *final_actor;
        void *final_entity;
        final_actor = D_8009D278;
        final_entity = D_8009D254;
        Entity_SetActionMode(final_entity, final_actor->actionMode12);
    }
}
