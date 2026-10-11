/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"

/* MASPSX_FORCE_G0 keeps the battle words absolute, as retail; the COMMON
 * fade level supplies the small-data metadata of the one $gp-relative byte. */
u8 D_8009CE68;

int Battle_ProcessActionSlot(BattleEntity *actor)
{
    EnemyCombatant *state = (EnemyCombatant *)actor->core;
    int phase = state->coreFlags & 0x6000;
    int result = 1;

    if (phase == 0x2000) {
        int index = D_8009D1D4;
        s16 command = D_800BE830[index].command;
        if (command != 0x196 && command != 0x189) {
            if (D_8009D278->action->actionCode.actionId != 6) {
                if (D_8009D23C == 0 && D_800B8A90[0] != 0) {
                    int i = 0;
                    do {
                        D_800B8A90[(u8)i] = 0;
                        i++;
                    } while (D_800B8A90[(u8)i] != 0);
                }
                {
                    u8 count = D_8009D23C;
                    D_8009D23C = count + 1;
                    D_800B8A90[count] = actor;
                }
            } else {
                /* Matching debt: retail sets the room argument before the
                 * test, in the branch delay slot. */
                register int room asm("$4") = 0x5B;
                if (actor == D_800BE830[index].actor) {
                    D_8009D208 = Scene_LoadRoomAssets(room, actor);
                    D_8009D23C = 0;
                }
            }
        }
        result = 0;
        if (state->field04.bytes.kind != 0 && state->hpAlive > 0) {
            D_8009CE68 = 0xFF;
        }
    } else if (phase == 0x4000 && state->field04.bytes.kind != 0) {
        /* Matching debt: retail compares the kind against a fresh 1, where
         * GCC would reuse the result register that already holds 1. */
        register int one asm("$2") = 1;
        u8 color = D_8009CE68 - 8;
        D_8009CE68 = color;
        if (state->field04.bytes.kind == one) {
            if (color <= 0x80) {
                actor->parent->renderObject.flags_9C |= 0x20;
                state->coreFlags &= ~0x6000;
            } else {
                Render_FadeEntityColor(&actor->parent->renderObject, color, color, color);
                result = 0;
            }
        } else if (state->field04.bytes.kind == 4) {
            BattleEntity *iter;
            for (iter = (BattleEntity *)D_8009D20C; iter != 0; iter = iter->next) {
                if (iter != D_8009D254 && iter->core != 0 &&
                    ((EnemyCombatant *)iter->core)->field04.bytes.kind == 4) {
                    int shade = D_8009CE68;
                    if ((u8)shade <= 0x80) {
                        iter->renderObject.flags_9C |= 0x20;
                        state->coreFlags &= ~0x6000;
                    } else {
                        Render_FadeEntityColor(&iter->renderObject, shade, shade, shade);
                        result = 0;
                    }
                }
            }
            return result;
        } else {
            if (state->hpAlive > 0) {
            if (color <= 0x80) {
                actor->renderObject.flags_9C |= 0x20;
                state->coreFlags &= ~0x6000;
            } else {
                Render_FadeEntityColor(&actor->renderObject, color, color, color);
                result = 0;
            }
            }
        }
    }
    return result;
}

/* Advance enemy charge/status timers, movement, damage panels and death state.
* Matching debt: the statusCore pin and the actor/statusCore barriers keep
* retail's separate copies of the entity and its core.
* curHP is the existing name for the 0..9000 charge gauge in this core view.
*/
void Battle_UpdateEnemy(BattleEntity *entity) {
    BattleEntity *actor = entity;
    register EnemyCombatant *statusCore asm("$18");
    BattleEntity *parent;
    BattleEntity *livingLink;
    BattleEntity *linkedActor;
    EnemyCombatant *linkedCore;
    EnemyCombatant *enemy;
    EnemyCombatant *otherCore;
    s16 angle;
    s32 hpChange;
    s32 actionPhase;
    s32 damageTicks;
    s32 advancedAngle;
    s32 wrappedAngle;
    EnemyActionEffect *expiredEffect;
    EnemyActionEffect *expiredStatusEffect;
    s8 linkedKindValue;
    s8 effectKind;
    s8 deadKind;
    u16 nextCharge;
    u16 charge;
    u32 initialFlags;
    u32 statusFlags;
    u32 damageFlags;
    u32 nextFlags;
    u32 nextStatusFlags;
    u32 actionFlags;

    asm("" : "=r"(actor) : "0"(actor));
    enemy = actor->core;
    {
        s32 hp = enemy->hpAlive;
        s32 maximum = enemy->field88;
        if (maximum < hp) enemy->hpAlive = maximum;
        statusCore = enemy;
        asm("" : "=r"(statusCore) : "0"(statusCore));
    }
    if (!(D_8009D1A0 & 0x100)) {
        if (enemy->curHP == 0) {
            initialFlags = enemy->coreFlags;
            if (initialFlags & 0xE) {
                u32 preserved = initialFlags & ~0xE;
                u32 count = ((((initialFlags >> 1) & 7) - 1) & 7) * 2;
                nextFlags = preserved | count;
                enemy->coreFlags = nextFlags;
                if (!(nextFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredEffect = enemy->effect;
                    if (expiredEffect != 0) {
                        expiredEffect->state = 4;
                        enemy->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
            statusFlags = statusCore->coreFlags;
            if (statusFlags & 0x1800) {
                nextStatusFlags = (statusFlags & ~0x1800) | (((((statusFlags >> 0xB) & 3) - 1) & 3) << 0xB);
                statusCore->coreFlags = nextStatusFlags;
                if (!(nextStatusFlags & 0x180E)) {
                    actor->entityFlags &= ~0x1000;
                    Entity_TickAnimSequences(actor);
                    expiredStatusEffect = enemy->effect;
                    if (expiredStatusEffect != 0) {
                        expiredStatusEffect->state = 4;
                        statusCore->coreFlags &= 0xC0FFFFFF;
                    }
                }
            }
        }
        charge = enemy->curHP;
        if (charge < 0x2328U) {
            nextCharge = charge + enemy->chargeStep;
            enemy->curHP = nextCharge;
            if (statusCore->coreFlags & 1) {
                enemy->curHP = nextCharge - ((enemy->chargeStep * 2) / 5);
            }
        } else {
            enemy->curHP = 0x2328;
            if (actor->entityFlags & 0x1000) {
                enemy->curHP = 0;
            }
        }
        damageFlags = statusCore->coreFlags;
        if (damageFlags & 0x10) {
            damageTicks = (damageFlags >> 5) & 0x1F;
            if (damageTicks >= 0x1E) {
                enemy->hpAlive -= enemy->periodicDamage;
                statusCore->coreFlags &= ~0x3E0;
            } else {
                statusCore->coreFlags = (damageFlags & ~0x3E0) | (((damageTicks + 1) & 0x1F) << 5);
            }
        }
    }
    if (statusCore->coreFlags & 0x6000) {
        Battle_ProcessActionSlot(actor);
        actionFlags = statusCore->coreFlags;
        actionPhase = actionFlags & 0x6000;
        if (actionPhase == 0x2000) {
            Battle_StepEnemyMovement(actor);
            Asset_Find08w(enemy->attackAssetId, 0, actor->renderObject.target_x, actor->renderObject.target_y, actor->renderObject.target_z);
        } else if (actionPhase == 0x4000) {
            s32 kind = enemy->field04.bytes.kind;
            if (kind == 0 && (actionFlags & 0xE) == 0) {
                {
                    if (actor->animLastFrame == actor->animPrev.parts.integer) {
                        if ((enemy->resumePhase == 2) && !(enemy->coreFlags & 0x1800)) {
                            enemy->resumePhase--;
                            Entity_SetActionMode(actor, enemy->savedActionMode);
                            if (enemy->savedInterpolation != 0) {
                                actor->entityFlags |= 0x200;
                            }
                            actor->animFrame = enemy->savedAnimFrame;
                            actor->animPrev.fixed = enemy->savedAnimPrev;
                            actor->animStep = enemy->savedAnimStep;
                        } else {
                            {
                                BattleEntity *callEntity;
                                unsigned mode = enemy->field06.bytes.low;
                                callEntity = actor;

                                Entity_SetActionMode(callEntity, (u16)(s8)mode);
                            }
                        }
                        statusCore->coreFlags &= ~0x6000;
                        if (!(D_8009D278->stateFlags & 0x80000) && !(enemy->coreFlags & 0x1800)) {
                            actor->entityFlags &= ~0x1000;
                            Entity_TickAnimSequences(actor);
                        }
                    } else if (((u8) enemy->motionPhase >= 2U) && (enemy->motionFrames != 0)) {
                        actor->posX.fixed += enemy->motionAmplitude * rsin(enemy->motionAngle) * 0x10;
                        actor->posZ.fixed += enemy->motionAmplitude * rcos(enemy->motionAngle) * 0x10;
                        enemy->motionFrames -= 1;
                    }
                }
            } else {
                if ((actionFlags & 0xE) != 0) {
                    statusCore->coreFlags &= ~0x6000;
                    if ((enemy->field04.bytes.kind != 0) && (enemy->hpAlive > 0)) {
                        effectKind = ((EnemyCombatant *)actor->core)->field04.bytes.kind;
                        if (effectKind == 1) {
                            parent = actor->parent;
                            parent->renderObject.flags_9C |= 0x20;
                        } else if (effectKind == 4) {
                            int linkedKind = 4;
                            BattleEntity *player;
                            linkedActor = (BattleEntity *)D_8009D20C;
                            if (linkedActor != 0) {
                                player = D_8009D254;
                                do {
                                    if (linkedActor != player) {
                                        otherCore = linkedActor->core;
                                        if ((otherCore != 0) && (otherCore->field04.bytes.kind == linkedKind)) {
                                            linkedActor->renderObject.flags_9C |= 0x20;
                                        }
                                    }
                                    linkedActor = linkedActor->next;
                                } while (linkedActor != 0);
                            }
                        } else {
                            actor->renderObject.flags_9C |= 0x20;
                        }
                    }
                }
            }
        }
        if (enemy->field04.bytes.kind != 1) {
            actor->motionX = 0;
            actor->motionY = 0;
            actor->motionZ = 0;
        }
    }
    if ((statusCore->coreFlags & 0xE) && (enemy->hpAlive > 0)) {
        int contextFlags = D_8009D1A0;
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
        if (!(contextFlags & 0x100)) {
            angle = actor->facingAngle;
            advancedAngle = angle + 0x80;
            wrappedAngle = advancedAngle;
            if (advancedAngle < 0) {
                wrappedAngle = angle + 0x107F;
            }
            actor->facingAngle = advancedAngle - ((wrappedAngle >> 0xC) << 0xC);
        }
    }
    if (statusCore->coreFlags & 0x1800) {
        actor->motionX = 0;
        actor->motionY = 0;
        actor->motionZ = 0;
    }
    if (statusCore->coreFlags & 0x400) {
        enemy->hpAlive = -1;
    }
    hpChange = enemy->previousHP - enemy->hpAlive;
    if (hpChange != 0) {
        if (hpChange < 0) {
            enemy->panelC_val = -hpChange;
            enemy->panelC_mode = 1;
        } else {
            enemy->panelC_val = hpChange;
            if ((statusCore->coreFlags & 0x38000) == 0x18000) {
                enemy->panelC_mode = 2;
            } else {
                enemy->panelC_mode = 0;
            }
        }
        enemy->panelC_x = (u16) actor->renderObject.projected_target_x;

        enemy->panelC_y = (u16) actor->renderObject.projected_target_y - 0x14;
        enemy->panelC_timer = 0x1E;
    }
    if (enemy->panelC_timer != 0) {
        Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
        enemy->panelC_timer -= 1;
    }
    if (enemy->hpAlive <= 0) {
        if (!(D_8009D1A0 & 0x100)) {
            if ((enemy->field04.bytes.kind == 1) && !(enemy->coreFlags & 0x6000)) {
                actor->entityFlags |= 0x10;
                Battle_SlotFree(actor);
            }
            if (enemy->field04.bytes.kind == 4) {
                if (!(enemy->coreFlags & 0x6000)) {
                    actor->motionX = 0;
                    actor->motionY = 0;
                    actor->motionZ = 0;
                    Battle_StepEntityAnimState(actor);
                }
            } else if (enemy->field04.bytes.kind != 1 && enemy->field04.bytes.kind != 3) {
                /* Matching debt: retail reaches this step from the kind-3
                 * search below; a copy there is laid out as its own block
                 * (lev 19). */
stepDeathAnimation:
                actor->motionX = 0;
                actor->motionY = 0;
                actor->motionZ = 0;
                Battle_StepEntityAnimState(actor);
            } else {
                livingLink = (BattleEntity *)D_8009D20C;
                for (;;) {
                    if (livingLink == 0) {
                        deadKind = enemy->field04.bytes.kind;
                        if (deadKind != 3) {
                            if (deadKind == 1) {
                                actor->entityFlags |= 0x10;
                                Battle_SlotFree(actor);
                                actor->parent->motionX = 0;
                                actor->parent->motionY = 0;
                                actor->parent->motionZ = 0;
                                Battle_StepEntityAnimState(actor->parent);
                            }
                        } else {
                            goto stepDeathAnimation;
                        }
                        break;
                    } else {
                        linkedCore = livingLink->core;
                        if ((linkedCore == 0) || (livingLink == D_8009D254) || (linkedKindValue = linkedCore->field04.bytes.kind, (linkedKindValue == 0)) || (linkedKindValue == 2) || (linkedKindValue == 4) || (linkedCore->hpAlive <= 0)) {
                            livingLink = livingLink->next;
                            continue;
                        }
                    }
                    break;
                }
            }
        }
    }
    enemy->previousHP = enemy->hpAlive;
}
