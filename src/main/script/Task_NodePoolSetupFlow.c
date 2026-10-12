/* CC1_FLAGS: -G8 -fno-strength-reduce */
/* MASPSX_FLAGS: -G8 */

/* Task-node pool and scene relocation-block setup. Pool initialization,
 * scene data relocation, the scene-map flush and free-list construction are
 * contiguous and share the node pool and g_SceneDataTable1 state. */

#include "pe1/reloc_block.h"
#include "pe1/task_node.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_actor.h"
#include "pe1/task_state.h"



void Task_InitNodePool(void) {
    unsigned int i;
    unsigned int j;
    unsigned int rowOffset;
    TaskNodeAddress base;
    int *ptr;

    i = 0;
    base.node = D_8009D310;
    rowOffset = 0;

    g_TaskNodePool = 0;
    g_TaskNodeSeqCounter[0] = 0;
    g_TaskNodeFreeListHead = 0;

    for (; i < 72; i++, rowOffset += sizeof(TaskNode)) {
        register unsigned int colOffset;
        TaskNodeAddress cell;

        j = 0;
        colOffset = rowOffset;
        for (; j < 11; j++, colOffset += 4) {
            cell.word = colOffset + base.word;
            *cell.words = 0;
        }
    }

    g_SceneDataTable0 = 0;

    i = 0;
    ptr = g_TaskScriptOperandTable;
    while (i < 16) {
        *ptr++ = 0;
        i++;
    }

    g_SceneDataTable1 = 0;
}

/* Turn a loaded block's self-relative base and offsets into addresses;
 * a base already above 0x80000000 means the block was relocated before. */
RelocBlock *Task_RelocBlock(RelocBlock *block) {
    RelocAddress address;
    RelocU32 offset;
    RelocU32 i;
    RelocU32 count;
    RelocU32 *ptr;
    char frame[8];

    address.block = block;
    offset = address.block->u0.baseOffset;
    g_SceneDataTable1 = address.block;
    if (0x80000000U < offset) {
        return address.block;
    }

    i = 0;
    count = address.block->count;
    address.block->u0.baseOffset = address.word + offset;
    if (count != 0) {
        ptr = &address.block->u0.baseOffset;
        do {
            ptr[2] += address.word;
            i++;
            ptr++;
        } while (i < address.block->count);
    }

    return g_SceneDataTable1;
}

void Task_DrawSyncAndFlush(void) {
    unsigned int i;
    int offset;
    RelocBlock *block;
    char frame[8];

    DrawSync(0);
    block = g_SceneDataTable1;
    i = (unsigned char)block->u0.base[0] != 0;
    if (i != 0) {
        i = 0;
        offset = 1;
        do {
            Scene_LoadMap(block->u0.base + offset, 0, 1);
            block = g_SceneDataTable1;
            i++;
            offset += 2;
        } while (i < (unsigned char)block->u0.base[0]);
    }
}

void Task_InitNodeFreeList(void) {
    unsigned short i;

    i = 0;
    g_TaskNodePool = 0;
    g_TaskNodeFreeListHead = D_8009D310;

    do {
        D_8009D310[i].next = &D_8009D310[i + 1];
        i++;
    } while (i < 0x47);

    D_8009D310[71].next = 0;

    i = 0;
    do {
        g_TaskNodeActiveFlags[i] = 0;
        i++;
    } while (i < 0x40);
}
