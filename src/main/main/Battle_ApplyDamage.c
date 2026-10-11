#include "pe1/battle_runtime.h"
#include "pe1/field_movement.h"
#include "pe1/item_ids.h"

void Aya_DeriveStats(int *first, int *second);
void Gpu_SetupSprites(BattleEntity *actor, int unused, int kind);

void Battle_ApplyDamage(int damageKind) {
    int effect;
    s8 result = -1;
    Combatant *actor;
    int flags;

    if ((D_8009D1A0 & 2) == 0) {
        BattleEntity *localActor = D_8009D254;
        D_8009D278 = localActor->core;
        effect = damageKind < ITEM_CURE_P ? 0x4B4 : 0x4B5;
        Asset_Find08Alt(effect, 1, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    } else {
        BattleEntity *localActor;
        effect = damageKind < ITEM_CURE_P ? 0x4B4 : 0x4B5;
        localActor = D_8009D254;
        Asset_Find08Alt(effect, 0, localActor->posX.parts.integer,
                        localActor->posY.parts.integer,
                        localActor->posZ.parts.integer);
    }

    switch (damageKind) {
    case ITEM_MEDICINE_1:
        D_8009D278->curHP += 0x2D;
        break;
    case ITEM_MEDICINE_2:
        D_8009D278->curHP += 0x5A;
        break;
    case ITEM_MEDICINE_3:
        D_8009D278->curHP += 0xB4;
        break;
    case ITEM_MEDICINE_4:
        D_8009D278->curHP += 0x190;
        break;
    case ITEM_FULL_RECOVER:
        D_8009D278->curHP = D_8009D278->maxHP;
        break;
    case ITEM_SUPER_JUNK:
        D_8009D278->exp_or_acc += D_8009D278->maxAtk / 4;
        break;
    case ITEM_DUPER_JUNK:
        D_8009D278->exp_or_acc += D_8009D278->maxAtk / 2;
        break;
    case ITEM_CURE_P: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & AYA_POISON_MASK) == 0) {
            target->stateFlags = field | AYA_POISON_IMMUNE;
        } else if ((field & AYA_POISON_MASK) != AYA_POISON_IMMUNE) {
            target->stateFlags = field & ~AYA_POISON_MASK;
        }
        result = 7;
        break;
    }
    case ITEM_CURE_D: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & AYA_DARKNESS_MASK) == 0) {
            target->stateFlags = field | AYA_DARKNESS_IMMUNE;
        } else if ((field & AYA_DARKNESS_MASK) != AYA_DARKNESS_IMMUNE) {
            target->stateFlags = field & ~AYA_DARKNESS_MASK;
        }
        result = 5;
        break;
    }
    case ITEM_CURE_C: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & AYA_CONFUSION_MASK) == 0) {
            target->stateFlags = field | AYA_CONFUSION_IMMUNE;
        } else if ((field & AYA_CONFUSION_MASK) != AYA_CONFUSION_IMMUNE) {
            target->stateFlags = field & ~AYA_CONFUSION_MASK;
            D_8009D2E8 &= ~FIELD_MOVE_CONFUSED;
        }
        result = 4;
        break;
    }
    case ITEM_CURE_M: {
        Combatant *target = D_8009D278;
        int field = target->stateFlags;
        if ((field & AYA_STIFFNESS_MASK) == 0) {
            target->stateFlags = field | AYA_STIFFNESS_IMMUNE;
        } else if ((field & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_IMMUNE) {
            target->stateFlags = field & ~AYA_STIFFNESS_MASK;
        }
        result = 6;
        break;
    }
    case ITEM_FULL_CURE:
        actor = D_8009D278;
        Aya_DeriveStats(&actor->atbStep, &actor->atbRate);
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & AYA_POISON_MASK) != AYA_POISON_IMMUNE) {
            actor->stateFlags = flags & ~AYA_POISON_MASK;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & AYA_CONFUSION_MASK) != AYA_CONFUSION_IMMUNE) {
            actor->stateFlags = flags & ~AYA_CONFUSION_MASK;
            D_8009D2E8 &= ~FIELD_MOVE_CONFUSED;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & AYA_DARKNESS_MASK) != AYA_DARKNESS_IMMUNE) {
            actor->stateFlags = flags & ~AYA_DARKNESS_MASK;
        }
        actor = D_8009D278;
        flags = actor->stateFlags;
        if ((flags & AYA_STIFFNESS_MASK) != AYA_STIFFNESS_IMMUNE) {
            actor->stateFlags = flags & ~AYA_STIFFNESS_MASK;
        }
        D_8009D278->stateFlags &= ~0x1000;
        break;
    }

    {
        Combatant *endActor = D_8009D278;
        if ((s16)endActor->maxHP < (s16)endActor->curHP) {
            endActor->curHP = (s16)endActor->maxHP;
        }
    }
    if (result != -1 && (D_8009D1A0 & 2)) {
        Gpu_SetupSprites(D_8009D254, 0, (u8)result);
    }
}
