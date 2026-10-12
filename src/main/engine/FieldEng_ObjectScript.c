#include "common.h"
#include "pe1/field_engine_state.h"
#include "pe1/psyq_bios.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"

/* Field engine objects: clearing an object's script state and dispatch
 * slots, and the two per-frame passes over the 0x40 dispatch slots. */

void **FieldEng_GetSlot(char *obj) {
    char *p;
    unsigned int i;

    p = obj + 0xC;
    do {
        *p++ = 0;
    } while (p < obj + 0xA0C);

    obj[2] = 0;
    obj[3] = 0;

    g_FieldEngineScriptState = &((FieldEngObject *)obj)->state;
    g_FieldEngineSlotTable = ((FieldEngObject *)obj)->slots;
    g_FieldEngineScriptObject = obj;
    g_FieldEngineScriptData = (char *)((FieldEngObject *)obj)->data;

    ((FieldEngObject *)obj)->state.wait = 0;
    ((FieldEngObject *)obj)->state.pc = 0;
    ((FieldEngObject *)obj)->state.data_next = 0;

    i = 0;
    do {
        g_FieldEngineSlotTable[i].handler_id = 0;
        g_FieldEngineSlotTable[i].flag = 0;
        g_FieldEngineSlotTable[i].counter = 0;
        g_FieldEngineSlotTable[i].data_offset = 0;
        i++;
    } while (i < 0x40);

    if (FieldEng_GetStatus(obj) == 3) {
        FieldActorState *inner = ((FieldAnimObjectPrefix *)obj)->actor->state;

        inner->core_flags = (inner->core_flags & 0xC0FFFFFF) | 0x01000000;
    }

    return (void **)&g_FieldEngineScriptState->commands;
}

int FieldEng_Register(char *obj, FieldEngHandler *handlers)
{
  register char *obj_s0 = obj;
  register FieldEngHandler *handlers_s2 = handlers;
  s16 i;
  char *table;
  i = 0;
  table = (char *) ((FieldEngObject *) obj_s0)->state.commands;
  g_FieldEngineScriptState = &((FieldEngObject *)obj_s0)->state;
  g_FieldEngineSlotTable = ((FieldEngObject *)obj_s0)->slots;
  g_FieldEngineScriptData = (char *)((FieldEngObject *)obj_s0)->data;
  g_FieldEngineCommandTable = (int *)table;
  for (; i < 0x40; i++)
  {
    FieldEngSlotAddress entry;
    entry.slot = g_FieldEngineSlotTable;
    entry.word = (s16)i * sizeof(FieldEngSlot) + entry.word;
    if (entry.slot->flag == 1)
    {
      FieldEngHandler handler = handlers_s2[entry.slot->handler_id];
      if (handler != ((void *) (-1)))
      {
        handler(obj, entry.slot, g_FieldEngineScriptData + entry.slot->data_offset);
      }
      else
      {
        printf(g_FieldEngServiceThreadMissingMsg);
      }
    }
  }

  return 0;
}

int FieldEng_UpdateSlots(char *obj, FieldEngHandler *handlers) {
    int result;
    s16 i;
    FieldActor *data;
    char *table;

    result = 0;
    table = (char *)((FieldEngObject *)obj)->state.commands;
    data = ((FieldAnimObjectPrefix *)obj)->actor;

    g_FieldEngineScriptState = &((FieldEngObject *)obj)->state;
    g_FieldEngineSlotTable = ((FieldEngObject *)obj)->slots;
    g_FieldEngineScriptObject = obj;
    g_FieldEngineScriptData = (char *)((FieldEngObject *)obj)->data;
    g_FieldEngineCommandTable = (int *)table;

    obj[3] = obj[0x12];

    if (FieldEng_GetStatus(obj) == 3) {
        if ((u8)obj[1] != 0x24) {
            u8 *state = ((FieldAnimObjectPrefix *)obj)->actor->state->action_state;

            if ((u8)state[0] == 1) {
                state[0] = 2;
            }
        }
    }

    for (i = 0; i < 0x40; i++) {
        FieldEngSlotAddress entry;

        entry.slot = g_FieldEngineSlotTable;
        entry.word = i * sizeof(FieldEngSlot) + entry.word;
        if (entry.slot->flag == 1) {
            FieldEngHandler handler = handlers[entry.slot->handler_id];

            if (handler != (void *)-1) {
                handler(obj, entry.slot, g_FieldEngineScriptData + entry.slot->data_offset);
            } else {
                printf(g_FieldEngUpdateThreadMissingMsg);
            }
            g_FieldEngineSlotTable[i].counter += 1;
        }

        if (g_FieldEngineSlotTable[i].flag == 2) {
            result |= FieldEng_FreeSlot((u16)i);
        }
    }

    if (FieldEng_GetStatus(obj) == 3) {
        char *inner = (char *)data->state;
        u32 flags = ((FieldActorState *)inner)->core_flags;

        if (*(u8 *)(inner + ((flags >> 17) & 0x70) + 0x1C) == 0 && (flags & 0x180E) != 0) {
            result = -1;
        }
    }

    return result;
}

#define SCRIPT g_FieldEngineScriptState

/* Runs the field script of obj until a wait or end command. */
int FieldEng_RunScript(FieldEngObject *obj, FieldEngHandler *handlers, u16 *sizes) {
    u8 stopped = 0;
    int result = 0;
    s16 wait;

    g_FieldEngineScriptState = &obj->state;
    g_FieldEngineSlotTable = obj->slots;
    g_FieldEngineScriptObject = (char *)obj;
    g_FieldEngineScriptData = (char *)obj->data;
    g_FieldEngineCommandTable = obj->state.commands;

    wait = obj->state.wait;
    if (wait == 0) {
        do {
            int command = g_FieldEngineCommandTable[SCRIPT->pc];

            if (command == -3) {
                result |= FieldEng_FreeAllSlots();
            }
            if (command == -2) {
                command = 0;
            }
            if (command != -1) {
                int high = (unsigned int)command >> 16;
                u16 op = high;
                    s16 operand = command;
                u8 index;

                if (op == 1) {
                    FieldEng_StartSlot(obj, (u8)operand, sizes, handlers);
                }
                if (op == 2) {
                    stopped = 1;
                    SCRIPT->wait = operand;
                }
                if ((unsigned int)(high - 0x10) < 0x10) {
                    index = op - 0x10;
                    SCRIPT->regs[index] = (s16)command;
                }
                if ((unsigned int)(high - 0x20) < 0x10) {
                    index = op - 0x20;
                    SCRIPT->regs[index] += (s16)command;
                }
                if ((unsigned int)(high - 0x30) < 0x10) {
                    index = op - 0x30;
                    SCRIPT->regs[index] -= (s16)command;
                }
                if ((unsigned int)(high - 0x40) < 0x10) {
                    index = op - 0x40;
                    SCRIPT->regs[index] = SCRIPT->vars[(u16)operand];
                }
                if ((unsigned int)(high - 0x50) < 0x10) {
                    index = op - 0x50;
                    SCRIPT->vars[(u16)operand] = SCRIPT->regs[index];
                }
                if ((unsigned int)(high - 0x1000) < 0x1000) {
                    s8 jump = op;

                    if (SCRIPT->regs[(high & 0xF00) >> 8] == (s16)command) {
                        SCRIPT->pc += jump;
                    }
                }
                if ((unsigned int)(op - 0x2000) < 0x1000) {
                    s8 jump = op;

                    if (SCRIPT->regs[(op & 0xF00) >> 8] != (s16)operand) {
                        SCRIPT->pc += jump;
                    }
                }
                if ((unsigned int)(op - 0x3000) < 0x1000) {
                    SCRIPT->pc += (s8)op;
                }
            } else {
                SCRIPT->halted = 1;
                stopped = 1;
            }

            if (SCRIPT->halted == 0) {
                SCRIPT->pc++;
            } else if (SCRIPT->slot_count == 0) {
                result = -1;
            }
        } while (!stopped);
    } else {
        obj->state.wait = wait - 1;
    }

    return result;
}

int FieldEng_Spawn6(char *base, int unused, int index, int value) {
    base += 0xC;
    g_FieldEngineScriptState = (FieldEngState *)base;
    ((FieldEngState *)base)->vars[index] = value;
    return 0;
}

int *FieldEng_RegPtr(int index) {
    return &g_FieldEngineScriptState->regs[index];
}

int *FieldEng_VarPtr(int index) {
    return &g_FieldEngineScriptState->vars[index];
}

void FieldEng_SetScriptContext(void *context) {
    g_FieldEngineScriptState->current_context = context;
}

void *FieldEng_GetScriptContext(void) {
    return g_FieldEngineScriptState->current_context;
}

int FieldEng_IsLastSlotHalted(void) {
    return ((((FieldEngStateWords *)g_FieldEngineScriptState)->status & 0xFFFF0000U) ^ 0x01010000U) < 1;
}

void *FieldEng_StartSlot(void *obj, u8 id, u16 *sizes, FieldEngHandler *handlers) {
    s16 slot = -1;
    s16 i;
    FieldEngSlot *base;
    void *entry;
    void *data;
    FieldEngHandler handler;

    base = g_FieldEngineSlotTable;
    for (i = 0; i < 0x40; i++) {
        if (base[i].flag == 0) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        g_FieldEngineScriptState->regs[14] = 1;
        return 0;
    }

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4 || FieldEng_GetStatus(obj) == 5) {
        FieldEng_ActivateSlot(slot, id, sizes[id]);

        entry = &g_FieldEngineSlotTable[slot];
        data = g_FieldEngineScriptData + ((FieldEngSlot *)entry)->data_offset;
        handler = handlers[id];
        if (handler != (void *)-1) {
            handler(obj, entry, data);
        }
        return data;
    }

    return 0;
}

/* Activate slot `slot` with handler `id` and reserve `size` bytes of script
 * data for it, wrapping to offset 0 when the 0x80C-byte area would overflow. */
void FieldEng_ActivateSlot(u16 slot, u8 id, u16 size) {
    FieldEngState *state;
    s16 offset;

    g_FieldEngineSlotTable[slot].flag = 1;
    g_FieldEngineSlotTable[slot].handler_id = id;
    g_FieldEngineSlotTable[slot].counter = 0;
    state = g_FieldEngineScriptState;
    offset = state->data_next;
    if ((unsigned int)(offset + (u16)size) >= 0x80C) {
        offset = 0;
    }
    g_FieldEngineSlotTable[slot].data_offset = offset;
    offset += size;
    state->data_next = offset;
    state->slot_count++;
}

int FieldEng_FreeSlot(u16 slot) {
    g_FieldEngineSlotTable[slot].flag = 0;
    g_FieldEngineScriptState->slot_count--;

    return -((((((FieldEngStateWords *)g_FieldEngineScriptState)->status & 0xFFFF0000) ^ 0x01000000) < 1));
}

int FieldEng_FreeAllSlots(void) {
    int i;
    int result;
    u32 andMask;
    u32 xorMask;

    result = 0;
    i = 0;
    andMask = 0xFFFF0000;
    xorMask = 0x01000000;
    for (; i < 0x40; i++) {
        if (g_FieldEngineSlotTable[i].flag != 0) {
            u32 check;
            g_FieldEngineSlotTable[(u16)i].flag = 0;
            g_FieldEngineScriptState->slot_count--;
            check = ((FieldEngStateWords *)g_FieldEngineScriptState)->status & andMask;
            check = check == xorMask;
            result |= -check;
        }
    }

    return result;
}
