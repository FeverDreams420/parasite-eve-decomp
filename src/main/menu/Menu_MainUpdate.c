#include "pe1/geom_state.h"
#include "common.h"
#include "pe1/scene_transition.h"
#include "pe1/textbox.h"
#include "pe1/menu_widget.h"
#include "pe1/gte_types.h"
#include "pe1/akao.h"
#include "pe1/entity_animation.h"
#include "pe1/menu_flags.h"
#include "pe1/asset_find.h"
#include "pe1/pm.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/battle_reset_stats.h"

/* CC1_FLAGS: -g3 -G1 */
/* MASPSX_FLAGS: --expand-div -G1 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#define NULL ((void *)0)

#include "pe1/battle.h"
#include "pe1/battle_hud_colors.h"
#include <m2c_macros.h>

M2C_UNK Battle_ReturnToIdle();                            /* extern */
s8 Pad_GetMenuPressedBitOrDisabled(void);
void Battle_FlushScriptSounds(void);
struct BattleStatusPanel;
void Battle_DrawStatusPanel(s32, struct BattleStatusPanel *);
M2C_UNK Entity_ResolveDropTable(void *);                      /* static */
s16 Entity_ApplyHitAndSetAnim(void *);                 /* static */
void Entity_UpdateTimers(void);
#include "Menu_MainUpdate_externs.h"
/* Separate declarations preserve the original GCC quantities for each tail. */
/* COMMON supplies small-data metadata to MASPSX. */
s8 D_8009CE30;
unsigned char D_8009CE34;


static inline u8 Battle_GetReactionTimer(void) {
    return D_8009D234[0];
}

static inline void Battle_SetReactionTimer(u8 value) {
    D_8009D234[0] = value;
}

void Menu_MainUpdate(s32 arg0) {
    char unused_stack[0x2C0];
    s16 temp_v0_7;
    s16 temp_v0_8;
    s16 temp_v1_10;
    s16 temp_v1_11;
    s16 temp_v1_13;
    s16 temp_v1_8;
    s32 *temp_s2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 temp_v1_4;
    s32 temp_v1_6;
    s32 temp_v1_7;
    s32 temp_v1_9;
    register s32 var_a2;
    register s32 var_a1;
    register s32 var_a3;
    register s32 var_t0;
    register s32 var_t1;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    register s32 var_v1;
    register u32 var_a0;
    register u8 var_a0_2;
    u16 temp_a1;
    register u32 var_v0;
    u32 temp_a2;
    EnemyActionEffect *temp_a0_2;
    u8 temp_a0;
    u8 temp_v0_6;
    u8 temp_v1_12;
    register Combatant *state0;
    register Combatant *temp_a1_2;
    EnemyCombatant *temp_v1_5;
    BattleEntity *var_s0;

    state0 = D_8009D278;
    temp_s2 = &state0->stateFlags;
    if (arg0 & 0xFF) {
        if (!(D_8009D1A0 & 0x100)) {
            temp_a2 = state0->stateFlags;
            temp_a1 = state0->hpAlive + state0->scaledOffense;
            temp_v1 = temp_a2 & 0xC0;
            state0->hpAlive = temp_a1;
            if ((temp_v1 == 0x40) || (temp_v1 == 0x80)) {
                state0->hpAlive = temp_a1 - ((state0->scaledOffense * 2) / 5);
            } else if (temp_a2 & 0x100) {
                state0->hpAlive = temp_a1 + ((u16) state0->scaledOffense >> 1);
            }
            temp_a1_2 = D_8009D278;
            if ((temp_a1_2->exp_or_acc < temp_a1_2->maxAtk) && !(*temp_s2 & 0x2600)) {
                temp_v0 = temp_a1_2->atbRate - 3;
                temp_a1_2->atbRate = temp_v0;
                if (temp_v0 <= 0) {
                    temp_a1_2->atbRate = 1;
                }
                temp_v0_2 = temp_a1_2->atbStep - (temp_a1_2->maxAtk / (temp_a1_2->atbRate * 0x64));
                temp_a1_2->atbStep = temp_v0_2;
                if (temp_v0_2 < 0x1999) {
                    temp_a1_2->atbStep = 0x1999;
                }
                temp_v0_3 = D_8009D278->exp_or_acc + temp_a1_2->atbStep;
                D_8009D278->exp_or_acc = temp_v0_3;
                if (temp_v0_3 >= temp_a1_2->maxAtk) {
                    volatile s32 *callback;

                    temp_a1_2->atbGauge = 0xF0;
                    callback = (volatile s32 *)D_800B0E08;
                    if (*callback != 0) {
                        Akao_SendTableCommand((void *)*callback, 0x455, 0, 0x80, 0x7F);
                    }
                }
            }
        }
        if ((arg0 & 0xFF) == 1) {
            temp_a0 = D_8009D254->actionMode;
            if (((temp_a0 == D_8009D278->actionMode12) || (temp_a0 == 5)) &&
                !((D_8009D278->attributes->effectFlags & 0x4000) && (D_8009D278->stateFlags & 0x4000) &&
                  (Pad_GetMenuPressedBitOrDisabled() << 0x18) > 0)) {
                D_8009D2E8 &= ~1;
            } else if ((u8) D_8009D254->actionMode >= 0xEU) {
                D_8009D2E8 &= ~1;
            } else {
                D_8009D2E8 |= 1;
            }
        }
    } else {
        D_8009D2E8 |= 1;
    }
    if (D_8009D278->exp_or_acc < 0) {
        D_8009D278->exp_or_acc = 0;
    }
    if (*temp_s2 & 0x2000) {
        var_v1 = D_8009D1E8 & 3;
        if (var_v1 == 0) {
            var_t1 = 0xFF;
            var_t0 = 0x3D;
            var_a3 = 0x81;
            var_a2 = 0x83;
            var_a1 = 0x13;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 1;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_t1;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_t1;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_status0;
            *(&D_800B0172[0] + ((var_v0_3 * 9) * 8)) = var_a0;
        }
        else if (var_v1 == 1) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x41;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_a2;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_status1;
            *(&D_800B0172[0] + ((var_v0_3 * 9) * 8)) = var_a0;
        }
        else if (var_v1 == 2) {
            var_t1 = 0x83;
            var_t0 = 0x13;
            var_a3 = 1;
            var_a2 = 0xFF;
            var_a1 = 0x3D;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x81;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_t1;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_t1;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_t0;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a3;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = draw_slot_status2;
            *(&D_800B0172[0] + ((var_v0_3 * 9) * 8)) = var_a0;
        }
        else {
        var_a2 = 0xC1;
        if (var_v1 == 3) {
            var_a2 = 0xC1;
            var_a1 = 0x28;
            {
            do { } while (0);
            var_v1 = D_8009CDDC;
            do { } while (0);
            var_a0 = 0x41;
            *(&D_800B0158[0] + (var_v1 * 0x48)) = var_a2;
            }
            *(&D_800B0159[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B015A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0160[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0161[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B0162[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0168[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0169[0] + (D_8009CDDC * 0x48)) = var_a1;
            *(&D_800B016A[0] + (D_8009CDDC * 0x48)) = var_a0;
            *(&D_800B0170[0] + (D_8009CDDC * 0x48)) = var_a2;
            *(&D_800B0171[0] + (D_8009CDDC * 0x48)) = var_a1;
            var_v0_3 = D_8009CDDC;
            *(&D_800B0172[0] + ((var_v0_3 * 9) * 8)) = var_a0;
        }
        }
        if (D_8009D254->actionMode != 0x12) {
            ((void (*)(BattleEntity *, int, int))Entity_SetActionMode)(D_8009D254, 0x12U, var_a2);
        }
        if (D_8009CE30 == 0x5A) {
            s32 reset_flags;
            u32 reset_operand;

            Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
            reset_flags = *temp_s2;
            reset_operand = ~0x2000U;
            reset_flags &= reset_operand;
            reset_operand = (u32) D_8009D278;
            D_8009CE30 = 0;
            *temp_s2 = reset_flags;
            ((Combatant *)reset_operand)->exp_or_acc = 0x10000;
            D_800B0158[0] = 0xFF;
            D_800B0159[0] = 0x3D;
            D_800B015A[0] = 0x81;
            D_800B0160[0] = 0x83;
            D_800B0161[0] = 0x13;
            D_800B0162[0] = 1;
            D_800B0168[0] = 0xFF;
            D_800B0169[0] = 0x3D;
            D_800B016A[0] = 0x81;
            D_800B0170[0] = 0x83;
            D_800B0171[0] = 0x13;
            D_800B0172[0] = 1;
            D_800B01A0[0] = 0xFF;
            D_800B01A1[0] = 0x3D;
            D_800B01A2[0] = 0x81;
            D_800B01A8[0] = 0x83;
            D_800B01A9[0] = 0x13;
            D_800B01AA[0] = 1;
            D_800B01B0[0] = 0xFF;
            D_800B01B1[0] = 0x3D;
            D_800B01B2[0] = 0x81;
            D_800B01B8[0] = 0x83;
            D_800B01B9[0] = 0x13;
            D_800B01BA[0] = 1;
        } else {
            D_8009CE30 += 1;
            D_8009D254->motionX = 0;
            D_8009D254->motionY = 0;
            D_8009D254->motionZ = 0;
        }
    }
    if (!(D_8009D1A0 & 0x100)) {
        register Combatant *timer_state;

        timer_state = D_8009D278;
        temp_v0_4 = timer_state->atbGauge;
        if (temp_v0_4 > 0) {
            temp_v0_4 -= 1;
            timer_state->atbGauge = temp_v0_4;
            if (temp_v0_4 != 0) {
                var_v1 = D_8009D1E8 & 3;
                if (var_v1 == 0) {
                    var_t0 = 0x82;
                    var_a3 = 0x36;
                    var_a2 = 0x4A;
                    var_a1 = 0xFF;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x3B;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = 0;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = 0x3B;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status0;
                    *(&D_800B014E[0] + ((var_v0_4 * 9) * 8)) = var_a0_2;
                }
                else if (var_v1 == 1) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_a2;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status1;
                    *(&D_800B014E[0] + ((var_v0_4 * 9) * 8)) = var_a0_2;
                }
                else if (var_v1 == 2) {
                    var_t0 = 0x4A;
                    var_a3 = 0xFF;
                    var_a2 = 0x3B;
                    var_a1 = 0x82;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x36;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_t0;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_t0;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a3;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = 0;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = draw_slot_status2;
                    *(&D_800B014E[0] + ((var_v0_4 * 9) * 8)) = var_a0_2;
                }
                else if (var_v1 == 3) {
                    var_a2 = 0x25;
                    var_a1 = 0xC1;
                    {
                    do { } while (0);
                    var_v1 = D_8009CDDC;
                    do { } while (0);
                    var_a0_2 = 0x39;
                    *(&D_800B0134[0] + (var_v1 * 0x48)) = var_a2;
                    }
                    *(&D_800B0135[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0136[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B013C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B013D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B013E[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B0144[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B0145[0] + (D_8009CDDC * 0x48)) = var_a1;
                    *(&D_800B0146[0] + (D_8009CDDC * 0x48)) = var_a0_2;
                    *(&D_800B014C[0] + (D_8009CDDC * 0x48)) = var_a2;
                    *(&D_800B014D[0] + (D_8009CDDC * 0x48)) = var_a1;
                    var_v0_4 = D_8009CDDC;
                    *(&D_800B014E[0] + ((var_v0_4 * 9) * 8)) = var_a0_2;
                }
                if (D_8009D278->exp_or_acc < D_8009D278->maxAtk) {
                    D_8009D278->atbGauge = 0;
                }
                if (D_8009D278->atbGauge == 0) {
                D_800B0134[0] = 0;
                D_800B0135[0] = 0x82;
                D_800B0136[0] = 0x36;
                D_800B013C[0] = 0x4A;
                D_800B013D[0] = 0xFF;
                D_800B013E[0] = 0x3B;
                D_800B0144[0] = 0;
                D_800B0145[0] = 0x82;
                D_800B0146[0] = 0x36;
                D_800B014C[0] = 0x4A;
                D_800B014D[0] = 0xFF;
                D_800B014E[0] = 0x3B;
                D_800B017C[0] = 0;
                D_800B017D[0] = 0x82;
                D_800B017E[0] = 0x36;
                D_800B0184[0] = 0x4A;
                D_800B0185[0] = 0xFF;
                D_800B0186[0] = 0x3B;
                D_800B018C[0] = 0;
                D_800B018D[0] = 0x82;
                D_800B018E[0] = 0x36;
                D_800B0194[0] = 0x4A;
                D_800B0195[0] = 0xFF;
                D_800B0196[0] = 0x3B;
                }
            } else {
                D_800B0134[0] = 0;
                D_800B0135[0] = 0x82;
                D_800B0136[0] = 0x36;
                D_800B013C[0] = 0x4A;
                D_800B013D[0] = 0xFF;
                D_800B013E[0] = 0x3B;
                D_800B0144[0] = 0;
                D_800B0145[0] = 0x82;
                D_800B0146[0] = 0x36;
                D_800B014C[0] = 0x4A;
                D_800B014D[0] = 0xFF;
                D_800B014E[0] = 0x3B;
                D_800B017C[0] = 0;
                D_800B017D[0] = 0x82;
                D_800B017E[0] = 0x36;
                D_800B0184[0] = 0x4A;
                D_800B0185[0] = 0xFF;
                D_800B0186[0] = 0x3B;
                D_800B018C[0] = 0;
                D_800B018D[0] = 0x82;
                D_800B018E[0] = 0x36;
                D_800B0194[0] = 0x4A;
                D_800B0195[0] = 0xFF;
                D_800B0196[0] = 0x3B;
            }
        }
        temp_v1_4 = *temp_s2;
        if (temp_v1_4 & 0x800000) {
            temp_v0_6 = Battle_GetReactionTimer() - 1;
            Battle_SetReactionTimer(temp_v0_6);
            if ((temp_v0_6 << 0x18) <= 0) {
                *temp_s2 &= 0xFF7FFFFF;
            }
        } else if (!(temp_v1_4 & 0x180000)) {
            var_s0 = (BattleEntity *)D_8009D20C;
            if (var_s0 != NULL) {
                do {
                    if (var_s0 != D_8009D254) {
                        {
                        register void *entity0;

                        entity0 = var_s0->core;
                        if ((entity0 != NULL) && !(var_s0->entityFlags & 0x10)) {
                            temp_v1_5 = entity0;
                            temp_a0_2 = temp_v1_5->effect;
                            if ((temp_a0_2 != NULL) && (*temp_s2 & 0x4000)) {
                                temp_v1_6 = temp_v1_5->coreFlags;
                                if ((temp_v1_6 < 0) && ((temp_a0_2->state - 1) < 2U)) {
                                    temp_v1_7 = ((unsigned int)temp_v1_6 >> 0x15) & 7;
                                    if (temp_v1_7 < 3) {
                                        Asset_Find08w(temp_v1_5->contactAssetIds[temp_v1_7], 0, var_s0->renderObject.target_x, var_s0->renderObject.target_y, var_s0->renderObject.target_z);
                                    }
                                    *temp_s2 |= 0x10000000;
                                    Entity_ResolveDropTable(var_s0);
                                    D_8009D254->motionX = 0;
                                    D_8009D254->motionY = 0;
                                    D_8009D254->motionZ = 0;
                                    {
                                    u16 v9c;
                                    s32 vflags;

                                    v9c = temp_v1_5->rewardFactorB;
                                    vflags = temp_v1_5->coreFlags;
                                    v9c = (u16) (v9c + 1);
                                    vflags &= 0x7FFFFFFF;
                                    temp_v1_5->coreFlags = vflags;
                                    temp_v1_5->rewardFactorB = v9c;
                                    }
                                    {
                                    s32 s2_flags;

                                    s2_flags = *temp_s2;
                                    D_8009CE34 = 0x5A;
                                    *temp_s2 = s2_flags | 0x01000000;
                                    }
                                }
                                {
                                register Combatant *desc_state;

                                desc_state = D_8009D278;
                                temp_v1_8 = (s16) desc_state->curHP;
                                if ((s16) desc_state->maxHP >= temp_v1_8) {
                                    register BattleEntity *d254;

                                    d254 = D_8009D254;
                                    desc_state->panelA_val = (s16) (desc_state->hpMirror - temp_v1_8);
                                    {
                                    u16 copy210;

                                    copy210 = (u16)d254->renderObject.projected_x;
                                    D_8009D278->panelA_x = copy210;
                                    }
                                    {
                                    u16 copy212;
                                    register Combatant *desc_tail;

                                    copy212 = (u16)d254->renderObject.projected_y;
                                    D_8009D278->panelA_timer = 0x1EU;
                                    desc_tail = D_8009D278;
                                    desc_state->panelA_y = copy212;
                                    if (desc_tail->panelA_val == 0) {
                                        desc_tail->panelA_scale = 0;
                                    } else {
                                        desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                                    }
                                    }
                                    D_8009D278->stateFlags = D_8009D278->stateFlags & 0xFFFF7FFF;
                                }
                                }
                            }
                            if (var_s0->entityFlags & 0x02000000) {
                                temp_v1_9 = *temp_s2;
                                if (!(temp_v1_9 & 0x01000000) && (temp_v1_5->hpAlive > 0)) {
                                    if (!(temp_v1_9 & 0x200)) {
                                        D_8009D278->curHP = (s16) ((u16) D_8009D278->curHP - temp_v1_5->contactDamage);
                                    }
                                    {
                                    s32 s2_flags2;
                                    register Combatant *desc_state;

                                    D_8009CE34 = 0x5A;
                                    s2_flags2 = *temp_s2;
                                    desc_state = D_8009D278;
                                    *temp_s2 = s2_flags2 | 0x01000000;
                                    temp_v1_10 = (s16) desc_state->curHP;
                                    if ((s16) desc_state->maxHP >= temp_v1_10) {
                                        register BattleEntity *d254;

                                        d254 = D_8009D254;
                                        desc_state->panelA_val = (s16) (desc_state->hpMirror - temp_v1_10);
                                        {
                                        u16 copy210;

                                        copy210 = (u16)d254->renderObject.projected_x;
                                        desc_state->panelA_x = copy210;
                                        }
                                        {
                                        u16 copy212;
                                        register Combatant *desc_tail;

                                        copy212 = (u16)d254->renderObject.projected_y;
                                        desc_state->panelA_timer = 0x1EU;
                                        desc_tail = D_8009D278;
                                        desc_state->panelA_y = copy212;
                                        if (desc_tail->panelA_val == 0) {
                                            desc_tail->panelA_scale = 0;
                                        } else {
                                            desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                                        }
                                        }
                                        D_8009D278->stateFlags = D_8009D278->stateFlags & 0xFFFF7FFF;
                                    }
                                    }
                                    {
                                    register s32 clear_flags;

                                    clear_flags = 0xFFF3FFFF;
                                    D_8009D254->motionX = 0;
                                    D_8009D254->motionY = 0;
                                    D_8009D254->motionZ = 0;
                                    D_8009D254->entityFlags = D_8009D254->entityFlags & clear_flags;
                                    D_8009D278->knockbackAngle = Entity_ApplyHitAndSetAnim(var_s0);
                                    }
                                    D_8009D278->knockbackDistance = (u8) temp_v1_5->contactKnockback;
                                    D_8009D278->knockbackFrames = 6;
                                }
                            }
                        }
                        }
                    }
                    var_s0 = var_s0->next;
                } while (var_s0 != NULL);
            }
            {
            register Combatant *desc_state;

            desc_state = D_8009D278;
            temp_v1_11 = (s16) desc_state->curHP;
            temp_v0_7 = (s16) desc_state->hpMirror;
            if (temp_v1_11 < temp_v0_7) {
                if ((s16) desc_state->maxHP >= temp_v1_11) {
                    register BattleEntity *d254;

                    d254 = D_8009D254;
                    desc_state->panelA_val = (s16) (temp_v0_7 - temp_v1_11);
                    {
                    u16 copy210;

                    copy210 = (u16)d254->renderObject.projected_x;
                    D_8009D278->panelA_x = copy210;
                    }
                    {
                    u16 copy212;
                    register Combatant *desc_tail;

                    copy212 = (u16)d254->renderObject.projected_y;
                    D_8009D278->panelA_timer = 0x1EU;
                    desc_tail = D_8009D278;
                    desc_state->panelA_y = copy212;
                    if (desc_tail->panelA_val == 0) {
                        desc_tail->panelA_scale = 0;
                    } else {
                        desc_tail->panelA_scale = (s8) (((unsigned int)desc_tail->stateFlags >> 0xE) & 2);
                    }
                    }
                    D_8009D278->stateFlags = D_8009D278->stateFlags & 0xFFFF7FFF;
                }
                D_8009D278->hpMirror = (u16) D_8009D278->curHP;
            }
            }
        }
        *temp_s2 &= ~0x4000;
        if (D_8009D278->knockbackFrames != 0) {
            temp_a0_3 = D_8009D254->entityFlags;
            if ((temp_a0_3 & 0xC0000) || (D_8009D278->knockbackDistance == 0)) {
                D_8009D254->entityFlags = temp_a0_3 & 0xFFF3FFFF;
                D_8009D278->knockbackFrames = 0;
                D_8009D278->knockbackDistance = 0U;
            } else {
                D_8009D254->posX.fixed = D_8009D254->baseX + (D_8009D278->knockbackDistance * rsin(D_8009D278->knockbackAngle) * 0x10);
                D_8009D254->posZ.fixed = D_8009D254->baseZ + (D_8009D278->knockbackDistance * rcos(D_8009D278->knockbackAngle) * 0x10);
                temp_v1_12 = D_8009D278->knockbackDistance;
                D_8009D278->knockbackDistance = (u8) (temp_v1_12 - (temp_v1_12 / D_8009D278->knockbackFrames));
                D_8009D278->knockbackFrames = (s8) ((u8) D_8009D278->knockbackFrames - 1);
            }
        }
        Entity_UpdateTimers();
    }
    if (D_8009D278->panelA_timer != 0) {
        Battle_DrawStatusPanel(0,
                               (struct BattleStatusPanel *)&D_8009D278->panelA_val);
        D_8009D278->panelA_timer = (u8) (D_8009D278->panelA_timer - 1);
    }
    {
    state0 = D_8009D278;
    temp_v0_8 = (s16) state0->hpMirror;
    temp_v1_13 = (s16) state0->curHP;
    if (temp_v0_8 < temp_v1_13) {
        register BattleEntity *d254;

        d254 = D_8009D254;
        state0->panelB_val = (s16) (temp_v1_13 - temp_v0_8);
        {
        u16 copy210;

        copy210 = (u16)d254->renderObject.projected_x;
        D_8009D278->panelB_x = copy210;
        }
        {
        var_v0 = (u16)d254->renderObject.projected_y;
        var_v1 = 0x1E;
        D_8009D278->panelB_timer = var_v1;
        { register Combatant *post_state; post_state = D_8009D278;
        state0->panelB_y = (s16) (var_v0 - 8);
        post_state->panelB_flag = 1; }
        }
        D_8009D278->hpMirror = (u16) D_8009D278->curHP;
    }
    }
    temp_a1_2 = D_8009D278;
    if (temp_a1_2->panelB_timer != 0) {
        Battle_DrawStatusPanel(0, (struct BattleStatusPanel *)&temp_a1_2->panelB_val);
        D_8009D278->panelB_timer = (u8) (D_8009D278->panelB_timer - 1);
        temp_a1_2 = D_8009D278;
    }
    if (temp_a1_2->panelAux_timer != 0) {
        Battle_DrawStatusPanel(0, (struct BattleStatusPanel *)&temp_a1_2->panelAux_val);
        D_8009D278->panelAux_timer = (u8) (D_8009D278->panelAux_timer - 1);
    }
    temp_a0_4 = D_8009D278->exp_or_acc;
    D_8009D278->hpMirror = (u16) D_8009D278->curHP;
    if (temp_a0_4 <= 0) {
        *temp_s2 |= 0x2000;
        if (Pad_GetMenuPressedBitOrDisabled() != D_8009D1D4) {
            if (D_8009D200 != -1) {
                Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
                D_8009D200 = -1;
            }
            if (D_8009D2FC != -1) {
                Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
                D_8009D2FC = -1;
            }
        }
        Battle_FlushScriptSounds();
    }
    if (D_8009D28C == 1) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v0 = 0xFF;
        var_v1 = 0xF9;
        D_800B00ED[0] = var_a2;
        D_800B00FD[0] = var_a2;
        D_800B0111[0] = var_a2;
        D_800B0121[0] = var_a2;
        var_a2 = 0x36;
        D_800B00F4[0] = var_a0;
        D_800B0104[0] = var_a0;
        D_800B692C[0] = var_a0;
        D_800B0118[0] = var_a0;
        D_800B0128[0] = var_a0;
        D_800B6948[0] = var_a0;
        var_a0 = 0x4A;
        D_800B00F6[0] = var_v1;
        D_800B0106[0] = var_v1;
        D_800B692E[0] = var_v1;
        D_800B011A[0] = var_v1;
        D_800B012A[0] = var_v1;
        D_800B694A[0] = var_v1;
        var_v1 = 0x3B;
        D_800B00EC[0] = 0;
        D_800B00EE[0] = var_a1;
        D_800B00F5[0] = var_v0;
        D_800B00FC[0] = 0;
        D_800B00FE[0] = var_a1;
        D_800B0105[0] = var_v0;
        D_800B692D[0] = var_v0;
        D_800B0110[0] = 0;
        D_800B0112[0] = var_a1;
        D_800B0119[0] = var_v0;
        D_800B0120[0] = 0;
        D_800B0122[0] = var_a1;
        D_800B0129[0] = var_v0;
        D_800B6949[0] = var_v0;
        D_800B0134[0] = 0;
        D_800B0135[0] = var_a1;
        D_800B0136[0] = var_a2;
        D_800B013C[0] = var_a0;
        D_800B013D[0] = var_v0;
        D_800B013E[0] = var_v1;
        D_800B0144[0] = 0;
        D_800B0145[0] = var_a1;
        D_800B0146[0] = var_a2;
        D_800B014C[0] = var_a0;
        D_800B014D[0] = var_v0;
        D_800B014E[0] = var_v1;
        D_800B017C[0] = 0;
        D_800B017D[0] = var_a1;
        D_800B017E[0] = var_a2;
        D_800B0184[0] = var_a0;
        D_800B0185[0] = var_v0;
        D_800B0186[0] = var_v1;
        D_800B018C[0] = 0;
        D_800B018D[0] = var_a1;
        D_800B018E[0] = var_a2;
        D_800B0194[0] = var_a0;
        D_800B0195[0] = var_v0;
        D_800B0196[0] = var_v1;
    }
    if ((s16) D_8009D278->curHP <= 0) {
        var_a2 = 0x46;
        var_a1 = 0x82;
        var_a0 = 0x9F;
        var_v1 = 0xF9;
        var_a3 = 0x3D;
        {
        var_v0 = (u32) D_8009D254;
        ((BattleEntity *)var_v0)->motionX = 0;
        ((BattleEntity *)var_v0)->motionY = 0;
        ((BattleEntity *)var_v0)->motionZ = 0;
        }
        var_v0 = 0xFF;
        D_800B00ED[0] = var_a2;
        D_800B00FD[0] = var_a2;
        D_800B0111[0] = var_a2;
        D_800B0121[0] = var_a2;
        D_800B00F4[0] = var_a0;
        D_800B0104[0] = var_a0;
        D_800B692C[0] = var_a0;
        D_800B0118[0] = var_a0;
        D_800B0128[0] = var_a0;
        D_800B6948[0] = var_a0;
        D_800B00EC[0] = 0;
        D_800B00EE[0] = var_a1;
        D_800B00F5[0] = var_v0;
        D_800B00F6[0] = var_v1;
        D_800B00FC[0] = 0;
        D_800B00FE[0] = var_a1;
        D_800B0105[0] = var_v0;
        D_800B0106[0] = var_v1;
        D_800B692D[0] = var_v0;
        D_800B692E[0] = var_v1;
        D_800B0110[0] = 0;
        D_800B0112[0] = var_a1;
        D_800B0119[0] = var_v0;
        D_800B011A[0] = var_v1;
        D_800B0120[0] = 0;
        D_800B0122[0] = var_a1;
        D_800B0129[0] = var_v0;
        D_800B012A[0] = var_v1;
        D_800B6949[0] = var_v0;
        D_800B694A[0] = var_v1;
        {
        var_a2 = 0x36;
        var_a0 = 0x4A;
        var_v1 = 0x3B;
        D_800B0136[0] = var_a2;
        D_800B0146[0] = var_a2;
        D_800B017E[0] = var_a2;
        D_800B018E[0] = var_a2;
        var_a2 = 0x81;
        D_800B0135[0] = var_a1;
        D_800B0145[0] = var_a1;
        D_800B017D[0] = var_a1;
        D_800B018D[0] = var_a1;
        var_a1 = 0x83;
        D_800B013C[0] = var_a0;
        D_800B014C[0] = var_a0;
        D_800B0184[0] = var_a0;
        D_800B0194[0] = var_a0;
        var_a0 = 0x13;
        }
        D_800B013E[0] = var_v1;
        D_800B014E[0] = var_v1;
        D_800B0186[0] = var_v1;
        D_800B0196[0] = var_v1;
        var_v1 = 1;
        D_800B0134[0] = 0;
        D_800B013D[0] = var_v0;
        D_800B0144[0] = 0;
        D_800B014D[0] = var_v0;
        D_800B017C[0] = 0;
        D_800B0185[0] = var_v0;
        D_800B018C[0] = 0;
        D_800B0195[0] = var_v0;
        {
        D_800B0158[0] = var_v0;
        D_800B0159[0] = var_a3;
        D_800B015A[0] = var_a2;
        D_800B0160[0] = var_a1;
        D_800B0161[0] = var_a0;
        D_800B0162[0] = var_v1;
        D_800B0168[0] = var_v0;
        D_800B0169[0] = var_a3;
        D_800B016A[0] = var_a2;
        D_800B0170[0] = var_a1;
        D_800B0171[0] = var_a0;
        D_800B0172[0] = var_v1;
        D_800B01A0[0] = var_v0;
        D_800B01A1[0] = var_a3;
        D_800B01A2[0] = var_a2;
        D_800B01A8[0] = var_a1;
        D_800B01A9[0] = var_a0;
        D_800B01AA[0] = var_v1;
        D_800B01B0[0] = var_v0;
        D_800B01B1[0] = var_a3;
        D_800B01B2[0] = var_a2;
        D_800B01B8[0] = var_a1;
        D_800B01B9[0] = var_a0;
        D_800B01BA[0] = var_v1;
        }
        if (*temp_s2 & 0x10000) {
            Battle_ReturnToIdle();
        }
        Tbl_ResetAll();
        if (Pad_GetMenuPressedBitOrDisabled() != D_8009D1D4) {
            if (D_8009D200 != -1) {
                Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
                D_8009D200 = -1;
            }
            if (D_8009D2FC != -1) {
                Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
                D_8009D2FC = -1;
            }
        }
        Battle_FlushScriptSounds();
        {
        register s32 tail_mask;
        register s32 tail_d1ac;
        register s32 tail_a0;

        tail_a0 = 0x46B;
        tail_mask = ~0x300;
        tail_d1ac = D_8009D1AC;
        tail_d1ac &= tail_mask;
        D_8009D1AC = tail_d1ac;
        {
            register s32 tail_a1;
            register s32 tail_a2;
            register s32 tail_a3;
            register s32 tail_sp;

            var_t0 = (s32) D_8009D254;
            /* posX/posY/posZ integer halves. Struct member reads let the
             * scheduler hoist the D_8009D28C store above them. */
            tail_a2 =M2C_FIELD((void *) var_t0, s16 *, 0x2A);
            tail_a3 = M2C_FIELD((void *) var_t0, s16 *, 0x2E);
            D_8009D28C = 3;
            tail_sp = M2C_FIELD((void *) var_t0, s16 *, 0x32);
            tail_a1 = 0;
            D_8009D1CE = 0;
            Asset_Find08Alt(tail_a0, tail_a1, tail_a2, tail_a3, tail_sp);
        }
        }
        D_8009D244 = 0;
        MenuWidget_InitPool();
        {
        register s32 minus5;

        if (g_GeomScroll.state.flags & 0x2000) {
            Render_BeginSceneLoad();
            minus5 = -5;
        } else {
            minus5 = -5;
        }
        D_8009D1A0 &= minus5;
        D_8009D2E8 |= 1;
        ((void (*)(BattleEntity *, int, int))Entity_SetActionMode)(D_8009D254, 0x13U, minus5);
        }
            D_8009D278->curHP = 0;
    }
}
