#include "common.h"
#include "pe1/battle.h"
#include "pe1/inventory.h"
#define NULL ((void *)0)

#define COMBATANT_ATTRIBUTES(base) \
    ((Combatant *)(base))->attributes
#define ATTRIBUTE_EFFECT_FLAGS(base) \
    ((BattleAttributes *)(base))->effectFlags

#define D278_0 (*(u8 **)&D278_o16)
#define D278_1 (*(u8 **)&D278_o16)
#define D278_2 (*(u8 **)&D278_o17)
#define D278_3 ((u8 *)g_ActiveActor)
#define D278_4 (*(u8 **)&D278_o16)
#define D278_5 (*(u8 **)&D278_o17)
#define D278_6 ((u8 *)g_ActiveActor)
#define D278_7 (*(u8 **)&D278_o16)
#define D278_8 (*(u8 **)&D278_o17)
#define D278_9 ((u8 *)g_ActiveActor)
#define D278_10 (*(u8 **)&D278_o16)
#define D278_11 (*(u8 **)&D278_o17)
#define D278_12 ((u8 *)g_ActiveActor)
#define D278_13 (*(u8 **)&D278_o16)
#define D278_14 (*(u8 **)&D278_o17)
#include "Battle_ApplyEnemyAttack_externs.h"
#define D278_15 ((u8 *)g_ActiveActor)
#define D278_16 (*(u8 **)&D278_o16)
#define D278_17 (*(u8 **)&D278_o17)

#define g_ActorEffectFlag100Timer (*(s16 *)&D228_o)
#define g_BattleSaveOverlayActive (*(s8 *)&D1CE_o)
#define D2E8A (*(s32 *)&D2E8_oa)
#define D2E8B (*(s32 *)&D2E8_ob)

void Battle_ApplyEnemyAttack(EnemyCombatant *ent) {
    s32 *ps = (s32 *)(D278_0 + 0x4C);

    switch (ent->effect->effectType) {
    case ENEMY_EFFECT_POISON:
        if ((*ps & AYA_POISON_MASK) == AYA_POISON) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_1)) & EFFECT_RESIST_POISON) {
            if ((rand() % 100) < AYA_POISON_RESIST_CHANCE) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & AYA_POISON_MASK) == AYA_POISON_IMMUNE) {
                *ps = f & ~AYA_POISON_MASK;
                break;
            }
            {
                Combatant *pD = (Combatant *)D278_2;
                u8 lvl;
                *ps = (f & ~AYA_POISON_MASK) | AYA_POISON;
                pD->statusTimer40 = 0x2328;
                lvl = ent->effectLevel;
                pD->subActionCounter = 0;
                pD->subActionStep = lvl;
            }
        }
        {
            Combatant *pD = (Combatant *)D278_3;
            pD->subActionPeriod = ent->effectDuration;
        }
        break;
    case ENEMY_EFFECT_CLEAR_BUFFS:
        {
            Combatant *pD = (Combatant *)D278_4;
            s32 flags;
            flags = pD->stateFlags;
            g_ActorEffectFlag100Timer = 0;
            pD->stateFlags = flags & ~AYA_HASTE;
            {
                s32 remainingFlags = ((volatile Combatant *)pD)->stateFlags;
                remainingFlags &= ~AYA_BARRIER;
                remainingFlags &= ~AYA_GENE_HEAL;
                remainingFlags &= ~AYA_PRERAISE;
                pD->stateFlags = remainingFlags;
            }
        }
        break;
    case ENEMY_EFFECT_CONFUSION:
        if ((*ps & AYA_CONFUSION_MASK) == AYA_CONFUSION) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_5)) & EFFECT_RESIST_CONFUSION) {
            if ((rand() % 100) < AYA_CONFUSION_RESIST_CHANCE) {
                break;
            }
        }
        {
            s32 f = *ps;
            s32 r;
            s32 a3v;
            s32 d;
            Combatant *pD;
            if ((f & AYA_CONFUSION_MASK) == AYA_CONFUSION_IMMUNE) {
                *ps = f & ~AYA_CONFUSION_MASK;
                break;
            }
            d = D2E8A;
            *ps = (f & ~AYA_CONFUSION_MASK) | AYA_CONFUSION;
            D2E8B = d | FIELD_MOVE_CONFUSED;
            r = rand();
            pD = (Combatant *)D278_6;
            a3v = r;
            if (r < 0) {
                a3v = r + 3;
            }
            pD->statusTimer42 = 0x2328;
            pD->stateFlags = (pD->stateFlags & ~AYA_CONFUSION_ANGLE_MASK) | (((r - ((a3v >> 2) << 2)) & 3) << 17);
        }
        break;
    case ENEMY_EFFECT_DARKNESS:
        if ((*ps & AYA_DARKNESS_MASK) == AYA_DARKNESS) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_7)) & EFFECT_RESIST_DARKNESS) {
            if ((rand() % 100) < AYA_DARKNESS_RESIST_CHANCE) {
                break;
            }
        }
        {
            s32 f = *ps;
            Combatant *pD;
            if ((f & AYA_DARKNESS_MASK) == AYA_DARKNESS_IMMUNE) {
                *ps = f & ~AYA_DARKNESS_MASK;
                break;
            }
            pD = (Combatant *)D278_8;
            *ps = (f & ~AYA_DARKNESS_MASK) | AYA_DARKNESS;
            pD->statusTimer44 = 0x2328;
        }
        break;
    case ENEMY_EFFECT_STIFFNESS_LOCK:
        if ((*ps & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_LOCK) {
            break;
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_9)) & EFFECT_RESIST_STIFFNESS) {
            if ((rand() % 100) < AYA_STIFFNESS_LOCK_RESIST_CHANCE) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_IMMUNE) {
                *ps = f & ~AYA_STIFFNESS_MASK;
                break;
            }
            if (f & AYA_HASTE) {
                *ps = f & ~AYA_HASTE;
            }
            {
                s32 nf = (*ps & ~AYA_STIFFNESS_MASK) | AYA_STIFFNESS_LOCK;
                Combatant *pD = (Combatant *)D278_10;
                *ps = nf;
                pD->statusTimer46 = 0x2328;
            }
        }
        break;
    case ENEMY_EFFECT_STIFFNESS_SLOW:
        {
            s32 m = *ps & AYA_STIFFNESS_MASK;
            if (m == AYA_STIFFNESS_LOCK) {
                break;
            }
            if (m == AYA_STIFFNESS_SLOW) {
                break;
            }
        }
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_11)) & EFFECT_RESIST_STIFFNESS) {
            if ((rand() % 100) < AYA_STIFFNESS_SLOW_RESIST_CHANCE) {
                break;
            }
        }
        {
            s32 f = *ps;
            if ((f & AYA_STIFFNESS_MASK) == AYA_STIFFNESS_IMMUNE) {
                *ps = f & ~AYA_STIFFNESS_MASK;
                break;
            }
            if (f & AYA_HASTE) {
                *ps = f & ~AYA_HASTE;
            }
            {
                s32 nf = (*ps & ~AYA_STIFFNESS_MASK) | AYA_STIFFNESS_SLOW;
                Combatant *pD = (Combatant *)D278_12;
                *ps = nf;
                pD->statusTimer46 = 0x2328;
            }
        }
        break;
    case 7:
        *ps = *ps | 0x1000;
        break;
    case ENEMY_EFFECT_HALVE_HP:
        if (!(*ps & AYA_BARRIER)) {
            Combatant *pD = (Combatant *)D278_13;
            s32 tmp = pD->curHP << 16;
            if ((tmp >> 16) >= 2) {
                *(s16 *)&pD->curHP = tmp >> 17;
            }
        }
        break;
    case ENEMY_EFFECT_REDUCE_HP_TO_1:
        {
            Combatant *pD = (Combatant *)D278_14;
            s32 hp;
            s32 f;
            g_ActorEffectFlag100Timer = 0;
            hp = *(s16 *)&pD->curHP;
            pD->stateFlags = pD->stateFlags & ~AYA_HASTE;
            f = ((volatile Combatant *)pD)->stateFlags;
            f &= ~AYA_BARRIER;
            f &= ~AYA_GENE_HEAL;
            f &= ~AYA_PRERAISE;
            pD->stateFlags = f;
            if (hp >= 2) {
                *(s16 *)&pD->curHP = 1;
            } else if (hp == 1) {
                *(s16 *)&pD->curHP = -1;
            }
        }
        break;
    case ENEMY_EFFECT_STEAL_ITEM:
    case ENEMY_EFFECT_STEAL_ITEM_ALT:
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_15)) & EFFECT_RESIST_STEAL) {
            s32 r = rand();
            if ((r % 100) >= ENEMY_STEAL_RESIST_CHANCE) {
                ent->lootItemId = Inv_PickRandomItem(r / 100);
                ent->effect->effectType = 0;
                Inv_FindItemById(ent->lootItemId);
                {
                    s32 it = ent->lootItemId;
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case ENEMY_EFFECT_STEAL_AMMO:
    case ENEMY_EFFECT_STEAL_AMMO_ALT:
        if (ATTRIBUTE_EFFECT_FLAGS(COMBATANT_ATTRIBUTES(D278_16)) & EFFECT_RESIST_STEAL) {
            if ((rand() % 100) >= ENEMY_STEAL_RESIST_CHANCE) {
                Inv_RollRandomItemType(&ent->lootItemId,
                                       &ent->lootItemAux);
                ent->effect->effectType = 0;
                {
                    s32 it = ent->lootItemId;
                    g_BattleSaveOverlayActive = 1;
                    g_CurItemEffectData = Inv_GetItemEffectData(it, 0);
                }
            }
        }
        break;
    case ENEMY_EFFECT_HP_TO_THREE_QUARTERS:
        if (!(*ps & AYA_BARRIER)) {
            Combatant *pD = (Combatant *)D278_17;
            s16 t = *(s16 *)&pD->curHP;
            if (t >= 2) {
                *(s16 *)&pD->curHP = (t * 3) / 4;
            }
        }
        break;
    }
}

#undef ATTRIBUTE_EFFECT_FLAGS
#undef COMBATANT_ATTRIBUTES
