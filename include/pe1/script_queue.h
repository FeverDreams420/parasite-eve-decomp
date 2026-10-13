#ifndef PE1_SCRIPT_QUEUE_H
#define PE1_SCRIPT_QUEUE_H

#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/script_node.h"

extern u32 *D_8009CE00;
extern u8 *D_8009D2F0[];
extern struct BattleEntity *D_8009D254;
extern u32 g_GameStateFlags;
extern u32 g_ScriptVariableBank[];
extern int (*g_ScriptQueueOpcodeHandlers[])(u32 **);

void Script_RunQueue(void);

#endif
