#ifndef PE1_TASK_STATE_H
#define PE1_TASK_STATE_H

#include "common.h"
#include "pe1/task_node.h"

/* Task script interpreter state and the tables its opcodes read
 * (src/main/script). */

extern int g_SceneDataTable0;
extern short g_EntityCollisionWallParam;
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

/* Sound commands an actor script queued while deferral was on
 * (Akao_ProcessCommand ops 0x196/0x197). */
typedef struct DeferredAkaoCommand {
    u8 actor_type;
    u8 actor_id;
    s8 command;
    s8 arg1;
    s16 arg2;
    s16 arg3;
} DeferredAkaoCommand;

extern DeferredAkaoCommand g_DeferredAkaoCommands[16];

u32 Task_GpuFlushPrimQueue(void);
void Task_SignalExit(void);

#endif /* PE1_TASK_STATE_H */
