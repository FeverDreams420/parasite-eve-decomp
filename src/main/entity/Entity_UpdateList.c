#include "pe1/player_entity.h"
#include "pe1/field_collision.h"

#include "Entity_UpdateList_externs.h"


void Entity_UpdateList(void) {
    FieldActor *node;

    if (g_FieldMoveLock & 4) {
        Entity_ApplyCollisionResponse(g_PlayerEntity);
    }

    node = g_FieldActorListHead;
    while (node != 0) {
        if ((node->flags & 0x80) == 0) {
            Entity_UpdateAndRender((struct BattleEntity *)node);
        }
        node = node->next;
    }
}
