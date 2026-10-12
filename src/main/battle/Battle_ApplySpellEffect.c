/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/battle.h"
#include "pe1/akao.h"
#include "pe1/entity_animation.h"
#include "pe1/battle_main.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_word_records.h"
#include "pe1/battle_ailment.h"

s32 rand(void);
s16 g_ActorEffectFlag100Timer;
void *g_BattleActiveEntity;

/* The acting combatant, reloaded after every structure store as retail does. */
#define ACTOR g_ActiveActorRecord.actor

/* Applies Parasite Energy technique idx (command - PE_SPELL_COMMAND_BASE) cast
 * by the player on target ent, then charges its PE cost. */
void Battle_ApplySpellEffect(u32 idx, BattleEntity *ent) {
    BattleActionSoundTable soundTable;
    s32 dmg;

    dmg = 0;
    if (!(g_GameStateFlagsRecord.flags & 2)) {
        soundTable = D_80010760;
        Akao_SendPositionalCmdStereo(soundTable.soundId[idx], 1, g_PlayerEntityRecord.entity->posX.parts.integer, g_PlayerEntityRecord.entity->posY.parts.integer, g_PlayerEntityRecord.entity->posZ.parts.integer);
        ACTOR = g_PlayerEntityRecord.entity->core;
    }
    switch (idx) {
    case PE_SPELL_HEAL_1:
        dmg = 0x3C0000;
        ACTOR->curHP += 0x1E;
        break;
    case PE_SPELL_HEAL_2:
        dmg = 0x780000;
        ACTOR->curHP += 0x3C;
        break;
    case PE_SPELL_HEAL_3:
        dmg = 0x01F40000;
        ACTOR->curHP += 0x118;
        break;
    case PE_SPELL_DETOX:
        if ((ACTOR->stateFlags & AYA_POISON_MASK) != AYA_POISON_MASK) {
            ACTOR->stateFlags &= ~AYA_POISON_MASK;
        }
        dmg = 0x640000;
        break;
    case PE_SPELL_MEDIC:
        /* Medic: cures every ailment and the 0x1000 state. */
        if ((ACTOR->stateFlags & AYA_POISON_MASK) != AYA_POISON_MASK) {
            ACTOR->stateFlags &= ~AYA_POISON_MASK;
        }
        if ((ACTOR->stateFlags & AYA_CONFUSION_MASK) != AYA_CONFUSION_MASK) {
            ACTOR->stateFlags &= ~AYA_CONFUSION_MASK;
            g_FieldMoveLockRecord.flags &= ~FIELD_MOVE_CONFUSED;
        }
        if ((ACTOR->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_DARKNESS_MASK;
        }
        if ((ACTOR->stateFlags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_STIFFNESS_MASK;
        }
        dmg = 0x02580000;
        ACTOR->stateFlags &= ~0x1000;
        break;
    case PE_SPELL_BARRIER:
        {
            Combatant *p = ACTOR;
            p->stateFlags |= AYA_BARRIER;
            p->exp_or_acc -= p->maxAtk / 3;
        }
        break;
    case PE_SPELL_SCAN:
        {
            /* Scan: note which of the target's status pairs read 2. */
            Combatant *e = ent->core;
            g_BattleResultFlagsRecord.bits.scanPair3 = (*(u32 *)&e->statusFlags2 & 3) == 2;
            g_BattleResultFlagsRecord.bits.scanPairC = (*(u32 *)&e->statusFlags2 & 0xC) == 8;
            g_BattleResultFlagsRecord.bits.scanPair30 = (*(u32 *)&e->statusFlags2 & 0x30) == 0x20;
            g_BattleResultFlagsRecord.bits.scanPairC000 = (*(u32 *)&e->statusFlags2 & 0xC000) == 0x8000;
            g_BattleResultFlagsRecord.bits.scanPair30000 = (*(u32 *)&e->statusFlags2 & 0x30000) == 0x20000;
            g_BattleResultFlagsRecord.bits.scanPair3000 = (*(u32 *)&e->statusFlags2 & 0x3000) == 0x2000;
            g_BattleResultFlagsRecord.bits.scanPairC00 = (*(u32 *)&e->statusFlags2 & 0xC00) == 0x800;
            if (((*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 6) & 3) == 2) {
                g_BattleResultFlagsRecord.bits.scanPairC0 = 1;
            } else {
                g_BattleResultFlagsRecord.bits.scanPairC0 = 0;
            }
            g_BattleResultFlagsRecord.bits.messageId = 0x4B;
            g_BattleActiveEntity = ent;
            g_BattleResultFlagsRecord.bits.infoPanel = 1;
            dmg = 0x320000;
        }
        break;
    case PE_SPELL_SLOW:
        /* Slow: certain when the target's pair reads 2, a coin flip at 0. */
        {
            s32 pair = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 8) & 3;
            if (pair != 0 ? pair == 2 : !(rand() & 1)) {
                Combatant *e = ent->core;
                *(s32 *)&e->coreFlags |= 1;
            }
            dmg = 0x5A0000;
        }
        break;
    case PE_SPELL_HASTE:
        /* Haste: also cures a stiffness affliction. */
        {
            Combatant *p = ACTOR;
            COMBATANT_STATE_BITS(p).haste = 1;
            g_ActorEffectFlag100Timer = 0x1C2;
            if (COMBATANT_STATE_BITS(p).stiffness == AYA_STIFFNESS_SLOW_VALUE ||
                COMBATANT_STATE_BITS(p).stiffness == AYA_STIFFNESS_LOCK_VALUE) {
                COMBATANT_STATE_BITS(p).stiffness = 0;
            }
            dmg = 0xC80000;
        }
        break;
    case PE_SPELL_CONFUSE:
        /* Confuse: certain when the target's pair reads 2, a coin flip at 0. */
        {
            s32 pair = (*(u32 *)&((Combatant *)ent->core)->statusFlags2 >> 0xA) & 3;
            if (pair != 0 ? pair == 2 : !(rand() & 1)) {
                s32 r = rand();
                Combatant *e = ent->core;
                s32 flags = *(s32 *)&e->coreFlags & ~0xE;
                flags |= (((r % 3) + 3) & 7) * 2;
                *(s32 *)&e->coreFlags = flags;
                Entity_SetActionMode(ent, 4);
                ent->entityFlags |= 0x1000;
            }
            dmg = 0x960000;
        }
        break;
    case PE_SPELL_GENE_HEAL:
        dmg = 0x01900000;
        ACTOR->stateFlags |= AYA_GENE_HEAL;
        break;
    case PE_SPELL_PRERAISE:
        dmg = 0x03E80000;
        ACTOR->stateFlags |= AYA_PRERAISE;
        break;
    case PE_SPELL_FULL_RECOVER:
        /* Full recovery: HP to maximum and every ailment cured. */
        {
            Combatant *actor = ACTOR;
            s32 flags = actor->stateFlags;
            actor->curHP = actor->maxHP;
            if ((flags & AYA_POISON_MASK) != AYA_POISON_MASK) {
                actor->stateFlags = flags & ~AYA_POISON_MASK;
            }
        }
        if ((ACTOR->stateFlags & AYA_CONFUSION_MASK) != AYA_CONFUSION_MASK) {
            ACTOR->stateFlags &= ~AYA_CONFUSION_MASK;
            g_FieldMoveLockRecord.flags &= ~FIELD_MOVE_CONFUSED;
        }
        if ((ACTOR->stateFlags & AYA_DARKNESS_MASK) != AYA_DARKNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_DARKNESS_MASK;
        }
        if ((ACTOR->stateFlags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_MASK) {
            ACTOR->stateFlags &= ~AYA_STIFFNESS_MASK;
        }
        dmg = 0x05140000;
        ACTOR->stateFlags &= ~0x1000;
        break;
    case PE_SPELL_LIBERATE:
        ACTOR->stateFlags = (ACTOR->stateFlags | 0x80000) & ~0x200000;
        break;
    }
    if ((s16)ACTOR->maxHP < (s16)ACTOR->curHP) {
        ACTOR->curHP = ACTOR->maxHP;
    }
    /* Armor attribute 0x200 cuts the PE cost by a third. */
    if (!(ACTOR->attributes->effectFlags & 0x200)) {
        ACTOR->exp_or_acc -= dmg;
        return;
    }
    ACTOR->exp_or_acc -= (dmg * 2) / 3;
}

