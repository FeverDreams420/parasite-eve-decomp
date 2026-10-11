#include "common.h"
#include "pe1/game_state.h"
#include "pe1/entity_allocation.h"
#include "pe1/player_entity.h"
#include "pe1/task_node.h"
/* CC1_FLAGS: -fno-strength-reduce */
/* MASPSX_FLAGS: -G8 --use-comm-section */

/* Entity pool reset and free-list construction share the pool globals and the
 * 14-entry 0x280-byte entity record block at D_800BEA90. */

int g_FieldMoveLock;
FieldActor *g_EntityFreeListHead;
FieldActor *g_FieldActorListHead;
FieldActor *g_CurrentEntity;
u16 g_EntityFreePoolCount;
FieldActor *g_PlayerEntity;
u32 D_8009D224;


void Entity_ResetAllPools(void)
{
    unsigned int i;
    unsigned int row;
    char *ptr;
    unsigned int base;
    unsigned int col;
    register unsigned int offset;

    g_FieldMoveLock = 0;

    i = 0;
    ptr = g_EntityWorkBuffer;
    do {
        *(int *)ptr = 0;
        i++;
        ptr += 4;
    } while (i < 0x200);

    i = 0;
    ptr = (char *)g_TaskNodeActiveFlags;
    do {
        i++;
        *(int *)ptr = 0;
    } while (i < 0x40);

    i = 0;
    base = (unsigned int)D_800BEA90;
    row = 0;
    do {
        col = 0;
        offset = row;
        do {
            *(int *)(offset + base) = 0;
            col++;
            offset += 4;
        } while (col < 0xA0);
        i++;
        row += 0x280;
    } while (i < 0xE);

    g_EntityFreeListHead = 0;
    g_FieldActorListHead = 0;
    g_CurrentEntity = 0;
    g_EntityFreePoolCount = 0;
    g_PlayerEntity = 0;
    D_8009D224 = 0;
    g_GameState.flags &= 0xFFFFCFFF;
}

void Entity_InitFreePool(void)
{
    int i;
    int offset;
    char *base;
    char *next;

    i = 0;
    base = D_800BEA90;
    next = base + 0x280;
    offset = 0;
    g_EntityFreePoolCount = 0;
    g_FieldActorListHead = 0;
    g_EntityFreeListHead = (FieldActor *)base;

    do {
        *(void **)(D_800BEA94 + offset) = next;
        next += 0x280;
        i++;
        offset += 0x280;
    } while ((unsigned int)i < 13);

    D_800C0B14[0] = 0;
    offset = 0;
    do {
        ((EntityAllocationBlock *)((u8 *)D_800A7620 + offset))->address = 0;
        offset += 8;
    } while ((unsigned int)offset < 0x80);

    g_PlayerEntity = 0;
}
