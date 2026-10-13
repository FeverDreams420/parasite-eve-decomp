#ifndef PE1_FIELD_SCRIPT_CONTEXT_H
#define PE1_FIELD_SCRIPT_CONTEXT_H

#include "common.h"

/* The field script engine stores the active context pointer at state+0x70. */
void FieldFx_SetScriptContext(void *context);
void *FieldFx_GetScriptContext(void);

/* Nonzero when the script state word at +4 has 0x0101 in its high half. */
int FieldFx_IsLastSlotHalted(void);

void *FieldFx_StartSlot(void *object, u8 id, u16 *sizes,
                   int (**handlers)(void *, void *, void *));

void *FieldFx_StartSlot(void *object, u8 id, u16 *sizes,
                   int (**handlers)(void *, void *, void *));

#endif /* PE1_FIELD_SCRIPT_CONTEXT_H */
