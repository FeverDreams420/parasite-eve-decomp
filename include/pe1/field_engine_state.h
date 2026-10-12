#ifndef PE1_FIELD_ENGINE_STATE_H
#define PE1_FIELD_ENGINE_STATE_H

#include "pe1/field_script_context.h"
#include "pe1/field_engine_slot.h"
#include "pe1/field_anim_pool.h"

/* Script-state work area (obj+0xC, pointed at by g_FieldEngineScriptState).
 * FieldFx_RunScript interprets the command table: each 32-bit command
 * carries an opcode in the high half and an operand in the low half. The int
 * register file at +0x08 and the variable file at +0x48 are also reached
 * through FieldFx_RegPtr/FieldFx_VarPtr; the active context
 * pointer at +0x70 is shared by FieldFx_SetScriptContext and
 * FieldFx_GetScriptContext. */
typedef struct FieldEngState {
    /* 0x00 */ short wait;              /* frames to sleep before the next command */
    /* 0x02 */ short pc;                /* index into the command table */
    /* 0x04 */ short data_next;         /* next free slot-data offset */
    /* 0x06 */ signed char slot_count;  /* live dispatch slots; a halted script
                                         * reports -1 once it reaches zero */
    /* 0x07 */ signed char halted;      /* set by the -1 end command */
    /* 0x08 */ int regs[16];            /* regs[14] = slot table full (FieldFx_StartSlot) */
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
    /* 0x000 */ FieldAnimObjectPrefix prefix;
    /* 0x00C */ FieldEngState state;
    /* 0x080 */ FieldEngSlot slots[0x40];
    /* 0x200 */ char data[1];
} FieldEngObject;

/* Bytes of slot data after the dispatch slots (object size 0xA0C). */
#define FIELD_ENG_DATA_SIZE 0x80C

/* One handler of an effect module's lists: called with the object, its
 * dispatch slot and the slot's data. */
typedef int (*FieldEngHandler)(void *obj, void *slot, void *data);

/* The object the engine is running and its parts, set by every pass. */
extern FieldEngState *g_FieldEngineScriptState;
extern FieldEngObject *g_FieldEngineScriptObject;
extern char *g_FieldEngineScriptData;
extern int *g_FieldEngineCommandTable;
extern FieldEngSlot *g_FieldEngineSlotTable;

/* Messages printed for a slot whose handler list entry is -1. */
extern char g_FieldEngServiceThreadMissingMsg[];
extern char g_FieldEngUpdateThreadMissingMsg[];

/* Object passes (FieldFx_ObjectScript.c). FieldFx_Register runs the
 * service (draw) handler of every active slot, FieldFx_UpdateSlots the
 * update handler and frees the slots marked for teardown, FieldFx_RunScript
 * steps the command table and starts slots from the init list with the slot
 * data sizes. FieldFx_Spawn6 stores a script variable; the effect classes
 * forward their six spawn arguments to it unchanged. */
int FieldFx_Register(FieldEngObject *obj, FieldEngHandler *handlers);
int FieldFx_UpdateSlots(FieldEngObject *obj, FieldEngHandler *handlers);
int FieldFx_RunScript(FieldEngObject *obj, FieldEngHandler *handlers, u16 *sizes);
int FieldFx_Spawn6();
int *FieldFx_RegPtr(int index);
int *FieldFx_VarPtr(int index);
void FieldFx_SetScriptContext(void *context);
void *FieldFx_GetScriptContext(void);
int FieldFx_IsLastSlotHalted(void);
void *FieldFx_StartSlot(void *obj, u8 id, u16 *sizes, FieldEngHandler *handlers);
void FieldFx_ActivateSlot(u16 slot, u8 id, u16 size);
int FieldFx_FreeSlot(u16 slot);
int FieldFx_FreeAllSlots(void);
/* Object status (FieldFx_CollisionQueries.c); 3 = running. */
int FieldFx_GetStatus(void *obj);

/* Shared XYZ origin consumed by field effect initializers. */
struct FieldAnimPointTriple;
extern struct FieldAnimPointTriple D_800E2290;

/* Shape helpers (FieldFx_GeometryAndShapes.c): the two CLUT pixel blocks
 * func_800CED3C uploads and the index of the loaded one, the colour-key table
 * and a word of the direction helpers. */
extern void *D_800B0E18;
extern void *D_800B0E1C;
extern s16 D_800F34E4;
extern u16 D_800E21A8[];
extern u16 D_800E21C8;

#endif /* PE1_FIELD_ENGINE_STATE_H */
