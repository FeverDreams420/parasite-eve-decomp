#ifndef PE1_FIELD_ENGINE_STATE_H
#define PE1_FIELD_ENGINE_STATE_H

#include "pe1/field_script_context.h"
#include "pe1/field_engine_slot.h"

/* Script-state work area (obj+0xC, pointed at by g_FieldEngineScriptState).
 * FieldEng_RunScript interprets the command table: each 32-bit command
 * carries an opcode in the high half and an operand in the low half. The int
 * register file at +0x08 and the variable file at +0x48 are also reached
 * through FieldEng_RegPtr/FieldEng_VarPtr; the active context
 * pointer at +0x70 is shared by FieldEng_SetScriptContext and
 * FieldEng_GetScriptContext. */
typedef struct FieldEngState {
    /* 0x00 */ short wait;              /* frames to sleep before the next command */
    /* 0x02 */ short pc;                /* index into the command table */
    /* 0x04 */ short data_next;         /* next free slot-data offset */
    /* 0x06 */ signed char slot_count;  /* live dispatch slots; a halted script
                                         * reports -1 once it reaches zero */
    /* 0x07 */ signed char halted;      /* set by the -1 end command */
    /* 0x08 */ int regs[16];            /* regs[14] = slot table full (FieldEng_StartSlot) */
    /* 0x48 */ int vars[9];
    /* 0x6C */ int *commands;           /* command table (obj+0x78) */
    /* 0x70 */ void *current_context;   /* active field object or its effect context */
} FieldEngState;

/* Word view of the same work area: the halt tests compare slot_count and
 * halted (the high half of the word at +4) in one load. */
typedef struct FieldEngStateWords {
    /* 0x00 */ u32 timing;  /* wait and pc */
    /* 0x04 */ u32 status;  /* data_next, slot_count, halted */
} FieldEngStateWords;

/* Field engine object: header, script state, dispatch slots, slot data. */
typedef struct FieldEngObject {
    /* 0x000 */ unsigned char header[0xC];
    /* 0x00C */ FieldEngState state;
    /* 0x080 */ FieldEngSlot slots[0x40];
    /* 0x200 */ unsigned char data[1];
} FieldEngObject;

/* One handler of an effect module's lists: called with the object, its
 * dispatch slot and the slot's data. */
typedef int (*FieldEngHandler)(void *obj, void *slot, void *data);

/* The object the engine is running and its parts, set by every pass. */
extern FieldEngState *g_FieldEngineScriptState;
extern char *g_FieldEngineScriptObject;
extern char *g_FieldEngineScriptData;
extern int *g_FieldEngineCommandTable;
extern FieldEngSlot *g_FieldEngineSlotTable;

/* Messages printed for a slot whose handler list entry is -1. */
extern char g_FieldEngServiceThreadMissingMsg[];
extern char g_FieldEngUpdateThreadMissingMsg[];

/* Object passes (FieldEng_ObjectScript.c). FieldEng_Register runs the
 * service (draw) handler of every active slot, FieldEng_UpdateSlots the
 * update handler and frees the slots marked for teardown, FieldEng_RunScript
 * steps the command table and starts slots from the init list with the slot
 * data sizes. FieldEng_Spawn6 stores a script variable; the effect classes
 * forward their six spawn arguments to it unchanged. */
int FieldEng_Register(char *obj, FieldEngHandler *handlers);
int FieldEng_UpdateSlots(char *obj, FieldEngHandler *handlers);
int FieldEng_RunScript(FieldEngObject *obj, FieldEngHandler *handlers, u16 *sizes);
int FieldEng_Spawn6();
int *FieldEng_RegPtr(int index);
int *FieldEng_VarPtr(int index);
void FieldEng_SetScriptContext(void *context);
void *FieldEng_GetScriptContext(void);
int FieldEng_IsLastSlotHalted(void);
void *FieldEng_StartSlot(void *obj, u8 id, u16 *sizes, FieldEngHandler *handlers);
void FieldEng_ActivateSlot(u16 slot, u8 id, u16 size);
int FieldEng_FreeSlot(u16 slot);
int FieldEng_FreeAllSlots(void);
/* Object status (FieldEng_CollisionQueries_3.c); 3 = running. */
int FieldEng_GetStatus(void *obj);

/* Shared XYZ origin consumed by field effect initializers. */
struct FieldAnimPointTriple;
extern struct FieldAnimPointTriple D_800E2290;

#endif /* PE1_FIELD_ENGINE_STATE_H */
