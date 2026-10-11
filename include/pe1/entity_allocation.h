#ifndef PE1_ENTITY_ALLOCATION_H
#define PE1_ENTITY_ALLOCATION_H

#include "common.h"

/* Sixteen allocation slots: a run length followed by its scratch-buffer address.
 * A zero address marks a free slot. */
typedef struct EntityAllocationBlock {
    u32 blockCount;
    s32 address;
} EntityAllocationBlock;

PE1_STATIC_ASSERT(sizeof(EntityAllocationBlock) == 8, entity_allocation_block_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(EntityAllocationBlock, address) == 4,
                  entity_allocation_block_address);
extern EntityAllocationBlock D_800A7620[16];

/* Entity pool storage cleared by Entity_ResetAllPools: the 0x800-byte work
 * buffer (also saved in the save tail), the 14 0x280-byte actor records at
 * D_800BEA90 (D_800BEA94 is the free-list link word of the first) and the
 * pool's trailing record. */
extern char g_EntityWorkBuffer[];
extern char D_800BEA90[];
extern char D_800BEA94[];
extern int D_800C0B14[];
extern int g_FieldMoveLock;

#endif
