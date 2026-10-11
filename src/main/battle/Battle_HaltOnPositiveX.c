#include "pe1/battle.h"

/* CC1_FLAGS: -G1 */
/* MASPSX_FLAGS: -G1 */

extern Combatant *g_ActiveActor;
extern FieldActor *g_PlayerEntity;
extern s8 D_8009CE30;

void Entity_SetActionMode(BattleEntity *entity, int mode);
void Battle_FlushScriptSounds(void);

#define COMBATANT_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(Combatant, member)))
#define ENTITY_FIELD(ptr, type, member) \
    (*(type *)((ptr) + PE1_OFFSETOF(BattleEntity, member)))

void Battle_HaltOnPositiveX(void) {
    char *state;
    char *actor;
    char *state2;
    char *actor2;
    int flags;

    state = (char *)g_ActiveActor;
    if (COMBATANT_FIELD(state, s16, curHP) > 0) {
        actor = (char *)g_PlayerEntity;
        ENTITY_FIELD(actor, int, motionX) = 0;
        ENTITY_FIELD(actor, int, motionY) = 0;
        ENTITY_FIELD(actor, int, motionZ) = 0;
        Entity_SetActionMode((BattleEntity *)actor, 0x12);

        state2 = (char *)g_ActiveActor;
        actor2 = (char *)g_PlayerEntity;
        flags = COMBATANT_FIELD(state2, int, stateFlags);
        D_8009CE30 = 0;
        flags |= 0x2000;
        COMBATANT_FIELD(state2, int, stateFlags) = flags;
        ENTITY_FIELD(actor2, int, entityFlags) &= ~0x100;
        Battle_FlushScriptSounds();
    }
}

#undef ENTITY_FIELD
#undef COMBATANT_FIELD
