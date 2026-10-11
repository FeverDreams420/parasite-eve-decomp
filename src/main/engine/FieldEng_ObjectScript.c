#include "common.h"
#include "pe1/field_engine_state.h"
#include "pe1/psyq_bios.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"

/* Field engine objects: clearing an object's script state and dispatch
 * slots, and the two per-frame passes over the 0x40 dispatch slots. */

int FieldEng_GetStatus(void *obj);
int func_800C2DA0(u16 slot);

extern char *D_800E2248;
extern u8 *D_800F34F4;
extern char *D_800F32A8;
extern char *D_800F3330;
extern char *D_800F33B0;
extern char D_800C20C8[];
extern char D_800C20EC[];

void **FieldEng_GetSlot(char *obj) {
    char *p;
    unsigned int i;
    int offset;

    p = obj + 0xC;
    do {
        *p++ = 0;
    } while (p < obj + 0xA0C);

    obj[2] = 0;
    obj[3] = 0;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;

    ((FieldEngObject *)obj)->state.wait = 0;
    ((FieldEngObject *)obj)->state.pc = 0;
    ((FieldEngObject *)obj)->state.data_next = 0;

    i = 0;
    offset = 0;
    do {
        ((FieldEngSlot *)(offset + (int)D_800F34F4))->handler_id = 0;
        ((FieldEngSlot *)(offset + (int)D_800F34F4))->flag = 0;
        ((FieldEngSlot *)(offset + (int)D_800F34F4))->counter = 0;
        ((FieldEngSlot *)(offset + (int)D_800F34F4))->data_offset = 0;
        i++;
        offset += 6;
    } while (i < 0x40);

    if (FieldEng_GetStatus(obj) == 3) {
        FieldActorState *inner = ((FieldAnimObjectPrefix *)obj)->actor->state;

        inner->core_flags = (inner->core_flags & 0xC0FFFFFF) | 0x01000000;
    }

    return (void **)(D_800E2248 + 0x6C);
}

int FieldEng_Register(char *obj, int (**handlers)(char *obj, void *entry, void *data))
{
  register char *obj_s0 = obj;
  register int (**handlers_s2)(char *obj, void *entry, void *data) = handlers;
  s16 i;
  char *table;
  i = 0;
  table = (char *) ((FieldEngObject *) obj_s0)->state.commands;
  D_800E2248 = obj_s0 + 0xC;
  D_800F34F4 = (u8 *) (obj_s0 + 0x80);
  D_800F3330 = obj_s0 + 0x200;
  D_800F33B0 = table;
  for (; i < 0x40; i++)
  {
    FieldEngSlot *entry = (FieldEngSlot *) ((((s16) i) * 6) + ((int) D_800F34F4));
    if (entry->flag == 1)
    {
      int (*handler)(char *obj, void *entry, void *data) = handlers_s2[entry->handler_id];
      if (handler != ((void *) (-1)))
      {
        handler(obj, entry, D_800F3330 + entry->data_offset);
      }
      else
      {
        printf(D_800C20C8);
      }
    }
  }

  return 0;
}

int func_800C251C(char *obj, int (**handlers)(char *obj, void *entry, void *data)) {
    int result;
    s16 i;
    FieldActor *data;
    char *table;

    result = 0;
    table = (char *)((FieldEngObject *)obj)->state.commands;
    data = ((FieldAnimObjectPrefix *)obj)->actor;

    D_800E2248 = obj + 0xC;
    D_800F34F4 = (u8 *)(obj + 0x80);
    D_800F32A8 = obj;
    D_800F3330 = obj + 0x200;
    D_800F33B0 = table;

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
        FieldEngSlot *entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

        if (entry->flag == 1) {
            int (*handler)(char *obj, void *entry, void *data) = handlers[entry->handler_id];

            if (handler != (void *)-1) {
                handler(obj, entry, D_800F3330 + entry->data_offset);
            } else {
                printf(D_800C20EC);
            }
            {
                FieldEngSlot *counter_entry = (FieldEngSlot *)(i * 6 + (int)D_800F34F4);

                counter_entry->counter += 1;
            }
        }

        {
            int status_offset = i * 6;
            u8 *status_base = D_800F34F4;

            if (((FieldEngSlot *)(status_offset + (int)status_base))->flag == 2) {
                result |= func_800C2DA0((u16)i);
            }
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

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *));
int func_800C2E08(void);

#define SCRIPT ((FieldEngState *)g_FieldEngineScriptState)

/* Runs the field script of obj until a wait or end command. */
int func_800C2758(FieldEngObject *obj, int (**handlers)(void *, void *, void *), u16 *sizes) {
    u8 stopped = 0;
    int result = 0;
    s16 wait;

    g_FieldEngineScriptState = (char *)&obj->state;
    g_FieldEngineSlotTable = (u8 *)obj->slots;
    g_FieldEngineScriptObject = (char *)obj;
    g_FieldEngineScriptData = (char *)obj->data;
    g_FieldEngineCommandTable = obj->state.commands;

    wait = obj->state.wait;
    if (wait == 0) {
        do {
            int command = g_FieldEngineCommandTable[SCRIPT->pc];

            if (command == -3) {
                result |= func_800C2E08();
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
                    func_800C2B90(obj, (u8)operand, sizes, handlers);
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
            } else if (SCRIPT->keep_alive == 0) {
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
    g_FieldEngineScriptState = base;
    ((FieldEngState *)base)->vars[index] = value;
    return 0;
}

void *func_800C2B10(int index) {
    return &((FieldEngState *)D_800E2248)->regs[index];
}

void *func_800C2B28(int index) {
    return &((FieldEngState *)D_800E2248)->vars[index];
}

void func_800C2B40(void *context) {
    ((FieldEngState *)g_FieldEngineScriptState)->current_context = context;
}

void *func_800C2B50(void) {
    return ((FieldEngState *)g_FieldEngineScriptState)->current_context;
}

int func_800C2B68(void) {
    return ((((FieldEngStateWords *)g_FieldEngineScriptState)->status & 0xFFFF0000U) ^ 0x01010000U) < 1;
}

void func_800C2D0C(u16 slot, u8 id, u16 size);

void *func_800C2B90(void *obj, u8 id, u16 *sizes, int (**handlers)(void *, void *, void *)) {
    s16 slot = -1;
    s16 i;
    FieldEngSlot *base;
    void *entry;
    void *data;
    int (*handler)(void *, void *, void *);

    base = (FieldEngSlot *)D_800F34F4;
    for (i = 0; i < 0x40; i++) {
        if (base[i].flag == 0) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        ((FieldEngState *)D_800E2248)->regs[14] = 1;
        return 0;
    }

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4 || FieldEng_GetStatus(obj) == 5) {
        func_800C2D0C(slot, id, sizes[id]);

        entry = D_800F34F4 + slot * 6;
        data = D_800F3330 + ((FieldEngSlot *)entry)->data_offset;
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
void func_800C2D0C(u16 slot, u8 id, u16 size) {
    FieldEngDataState *state;
    s16 offset;

    g_FieldEngineSlots[slot].flag = 1;
    g_FieldEngineSlots[slot].handler_id = id;
    g_FieldEngineSlots[slot].counter = 0;
    state = g_FieldEngineState;
    offset = state->data_next;
    if ((unsigned int)(offset + (u16)size) >= 0x80C) {
        offset = 0;
    }
    g_FieldEngineSlots[slot].data_offset = offset;
    offset += size;
    state->data_next = offset;
    state->slot_count++;
}

int func_800C2DA0(u16 slot) {
    ((FieldEngSlot *)D_800F34F4)[slot].flag = 0;
    D_800E2248[6]--;

    return -((((((FieldEngStateWords *)D_800E2248)->status & 0xFFFF0000) ^ 0x01000000) < 1));
}

int func_800C2E08(void) {
    int i;
    int offset;
    int result;
    u32 andMask;
    u32 xorMask;
    FieldEngSlot *entry;

    result = 0;
    i = 0;
    andMask = 0xFFFF0000;
    xorMask = 0x01000000;
    offset = 0;
    for (; i < 0x40; i++, offset += 6) {
        if (((FieldEngSlot *)(offset + (int)D_800F34F4))->flag != 0) {
            u32 check;
            entry = (FieldEngSlot *)((u16)i * 6 + (int)D_800F34F4);
            entry->flag = 0;
            D_800E2248[6]--;
            check = ((FieldEngStateWords *)D_800E2248)->status & andMask;
            check = check == xorMask;
            result |= -check;
        }
    }

    return result;
}
