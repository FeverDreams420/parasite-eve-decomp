/* CC1_FLAGS: -fno-force-mem -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/battle_runtime.h"
#include "pe1/gte_types.h"
#include "pe1/battle_main.h"

/* COMMON supplies the small-data sizes of the turn queue words to MASPSX,
 * which then keeps the load delay before each $gp store; every other word
 * stays absolute. */
u8 D_8009CE38[4];
u8 D_8009CE39;
u8 D_8009CE3A;
u8 D_8009CE3C;
u8 D_8009D1D4;
u8 D_8009D1DC;
int D_8009D200;
u8 D_8009D274;
s16 D_8009D27C;
u8 D_8009D294;

void Battle_FinalizeAttackResult(void)
{
    Combatant *combatant;
    BattleAction *action;
    u32 attack_word;

    combatant = D_8009D278;
    action = combatant->action;

    if (action->actionCode.actionId == 8) {
        BattleEntity *player;

        D_8009D294 = 1;
        player = D_8009D254;
        Asset_Find08Alt(0x46C, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
        return;
    }

    attack_word = action->attackWord;
    if ((attack_word & 0x3FF) != 0) {
        D_8009D294 = 1;
        if (!(combatant->stateFlags & 0x100000)) {
            BattleEntity *player;

            Pm_SendCmd(D_8009D200, 0, 0, 1, 0, 0);
            player = D_8009D254;
            Akao_SetPos3D(0, 0, player->posX.parts.integer,
                          player->posY.parts.integer, player->posZ.parts.integer);
        }

        {
            Combatant *next_combatant;
            BattleAction *next_action;
            u32 next_attack_word;

            next_combatant = D_8009D278;
            next_action = next_combatant->action;
            next_attack_word = next_action->attackWord;
            next_action->attackWord = (next_attack_word & ~0x3FF) |
                                      (((next_attack_word & 0x3FF) - 1) & 0x3FF);
        }
        return;
    }

    {
        BattleEntity *player;

        D_8009D294 = 0;
        player = D_8009D254;
        Asset_Find08Alt(0x46E, 0, player->posX.parts.integer,
                        player->posY.parts.integer, player->posZ.parts.integer);
    }
}



int BattleCmd_CommitAmmoAndUpdate(); /* no prototype: retail calls it with and without the action */


int Battle_StepCharacterAction(BattleInitSlot *slot)
{
    BattleInitSlot * turnSlot = slot;
    int result = 0;
    register BattleAction *activeAction asm("$4");
    Combatant * active;
    BattleEntity * caseEntity;
    BattleEntity *idleEntity;
    Combatant *idleActor;
    int retryCount;
    int indexB;
    register int nextIndexB asm("$3");
    int indexAlt;
    int indexDefault;
    int angle;
    int direction;
    register int predicate asm("$2");
    register int facingAngle asm("$4");
    register int height asm("$16");

    switch (D_8009D254->actionMode) {
    case 6:
    case 8:
    case 10: {
        BattleEntity *current = D_8009D254;
        if (current->animLastFrame != ((u16 *)&current->animFrame)[1])
            break;
        if (D_8009CE38[0] != 0) {
            D_8009CE38[0]--;
        } else if (D_8009CE39 != 0) {
            D_8009CE39--;
        } else {
            goto ready;
        }
        current->entityFlags |= 0x100;
        break;
    ready:
        }
        D_8009D274 = 0;
        { u8 animMode = D_8009D278->action->animMode[1]; D_8009CE39 = animMode; }
        asm volatile("");
        { height = turnSlot->actor->renderObject.target_y; angle = height - D_8009D27C; }
        { facingAngle = ratan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009D254));
        if (facingAngle < -0xAB) direction = 0; else { predicate = facingAngle < 0xE4; if (predicate) direction = 1; else direction = 2; } }
        Entity_SetActionMode(D_8009D254, *(direction + D_8009D278->followUpAngleActionModes));
        D_8009D254->entityFlags &= ~0x100;
        D_8009D1DC = ((BattleTurnBits *)&D_8009D278->action->turnWord)->actions;
        Battle_FinalizeAttackResult();
        break;

    case 7:
    case 9:
    case 11:
        caseEntity = D_8009D254;
        if (caseEntity->animLastFrame != caseEntity->animPrev.parts.integer)
            break;
        indexB = D_8009D1D4;
        if (D_800BE830[indexB].command == 0x189) {
            idleActor = D_8009D278;
            idleEntity = D_8009D254;
        }
        else
        {
        { active = D_8009D278;
        activeAction = active->action;
        if ((activeAction->turnWord & 0xC0) == 0xC0) {
            int pending = D_8009D1DC;
            int narrow;
            predicate = 1;
            asm volatile("" : "=r"(pending) : "0"(pending));
            narrow = (u8)pending;
            retryCount = pending;
            if (narrow != predicate) goto alternate;
        }
        nextIndexB = indexB + 1;
        { register int offset asm("$5") = nextIndexB << 3;
        { int nextCode = D_800BE830[nextIndexB].command;
        D_8009D1DC = 0;
        if (nextCode >= 3 || nextIndexB == D_8009CE3C) {
            Entity_SetActionMode(D_8009D254, active->actionMode12);
            D_8009D278->stateFlags |= 0x200000;
            { u8 animMode = D_8009D278->action->animMode[0]; D_8009CE38[0] = animMode; }
            result = 1; break;
        }
        }
        { EnemyCombatant *enemy = (EnemyCombatant *)((BattleInitSlot *)((u8 *)D_800BE830 + offset))->actor->core; if (!(enemy->hpAlive > 0 && enemy)) {
            Entity_SetActionMode(D_8009D254, active->actionMode12);
            D_8009D1D4++;
            D_8009D278->stateFlags |= 0x200000;
            goto action_done;
        }
        }
        }
        }
        { register BattleAction *attackAction asm("$4") = active->action;
        if (attackAction->attackWord & 0x3FF) {
            { height = turnSlot->actor->renderObject.target_y; angle = height - D_8009D27C; }
            { facingAngle = ratan2(angle, Battle_CalcDistToPlayer(turnSlot->actor, D_8009D254));
        if (facingAngle < -0xAB) direction = 0; else { predicate = facingAngle < 0xE4; if (predicate) direction = 1; else direction = 2; } }
            Entity_SetActionMode(D_8009D254, *(direction + D_8009D278->targetAngleActionModes));
            result = 1;
            { register Combatant *postActor = D_8009D278;
              { register BattleEntity *postEntity = D_8009D254;
                postEntity->animFrame = (u32)D_8009D254->animLastFrame << 16;
                D_8009D254->entityFlags |= 0x100; }
              { register BattleAction *postAction = postActor->action;
                { u32 turnWord = postAction->turnWord;
                D_8009CE39 += D_8009CE3A;
                D_8009D1DC = turnWord & 0xF; } } }
            break;
        }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode(D_8009D254, 0xC);
            Akao_SetPos3D(1, 0, D_8009D254->posX.parts.integer,
                          D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
        action_done:
            result = 1; asm volatile(""); break;
        }
        result = 1; break;

    alternate:
        if (activeAction->attackWord & 0x3FF) {
            indexAlt = D_8009D1D4;
            {
                EnemyCombatant *enemy = (EnemyCombatant *)D_800BE830[indexAlt].actor->core;
                int nextRetry;
                if (enemy->hpAlive > 0) {
asm volatile("" : : "r"(enemy));
                nextRetry = retryCount - 1;
                if (enemy) goto target_alive;
                }
            target_dead:
            Entity_SetActionMode(caseEntity, active->actionMode12);
            D_8009D278->stateFlags |= 0x200000;
            D_8009D1DC = ((BattleTurnBits *)&D_8009D278->action->turnWord)->actions;
            result = 1; break;
            target_alive:
            D_8009D1DC = nextRetry;
            Battle_FinalizeAttackResult();
            break;
            }
        }
        if (BattleCmd_CommitAmmoAndUpdate()) {
            Entity_SetActionMode(D_8009D254, 0xC);
            Akao_SetPos3D(1, 0, D_8009D254->posX.parts.integer,
                          D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
            break;
        }
        idleActor = D_8009D278;
        idleEntity = D_8009D254;
        }
    set_idle:
        result = 1;
        Entity_SetActionMode(idleEntity, idleActor->actionMode12);
        break;
    default:
        indexDefault = D_8009D1D4;
        if (D_800BE830[indexDefault].command == 0x189)
            break;
        D_8009D1D4 = indexDefault + 1;
        if ((u8)(indexDefault + 1) != D_8009CE3C)
            break;
        if (D_8009D200 < 0)
            break;
        if (D_8009D278->action->actionCode.actionId != 8)
            Pm_SendCmd(D_8009D200, 0, 0, 2, 0, 0);
        Pm_SendCmd(D_8009D2FC, 0, 0, 2, 0, 0);
        break;
    }
    return result;
}
