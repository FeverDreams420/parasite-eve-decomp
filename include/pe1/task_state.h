#ifndef PE1_TASK_STATE_H
#define PE1_TASK_STATE_H

#include "common.h"
#include "pe1/task_node.h"

/* Task script interpreter state and the tables its opcodes read
 * (src/main/task). */

extern int g_SceneDataTable0;
extern short g_EntityCollisionWallParam;
extern u8 D_800944A8[];
extern TaskNode *g_TaskNodeFreeListHead;
/* Sequence number stamped into each allocated task node (a one-element
 * array: small data in the -G8 pool unit). */
extern unsigned short g_TaskNodeSeqCounter[1];
extern int g_TaskScriptOperandTable[16];
extern u16 g_SceneInitLbaTbl0[];
extern u16 g_SceneInitLbaTbl1[];
extern u16 g_SceneAssetLbaTbl[];
extern unsigned char g_BattleEnemyEscapeFlag;
extern int g_CombatModeFlags;
extern int D_800A76CC;

#endif /* PE1_TASK_STATE_H */
