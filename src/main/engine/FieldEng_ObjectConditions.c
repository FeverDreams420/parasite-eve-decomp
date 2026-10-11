#include "common.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"

typedef struct {
    char pad0[0x8];
    void *target;
    char padC[0x8];
    s16 counter;
    char pad16[0x2];
    s8 value;
} Func800DFB20State;

typedef struct {
    char pad0[0xF];
    u8 limit;
    char pad10[0x4];
    int value;
    char pad18[0x2];
    u16 cursor;
} Func800DFB20Target;

void func_800DFB20(Func800DFB20State *state)
{
    Func800DFB20Target *target = state->target;

    if (state->counter != 0 && target->cursor >= target->limit) {
        target->value = state->value;
        if (state->counter != -1) {
            state->counter--;
        }
    }
}

/* Wait condition for a field object: nonzero while the object is still busy.
 * A pending one-shot flag (0x80000 or 0x40000 at +0x98, selected by the
 * state bits) is consumed and reported once; otherwise the object waits while
 * its child is mid-animation. */
int func_800DFB78(FieldAnimTaskOwner *arg0)
{
  register FieldAnimTaskContext *state;
  FieldActor *obj;
  register u32 flags;
  register u32 masked;
  register FieldActorState *child;
  u32 word;
  u32 byte;
  int phase;
  byte = arg0->tasks.flags;
  obj = arg0->prefix.actor;
  state = &arg0->tasks;
  word = 4;
  if ((byte & word) && (obj->mode < 2))
  {
    return 1;
  }
  if (state->flags & 2)
  {
    flags = obj->flags;
    if (flags & 0x80000)
    {
      masked = flags & 0xFFF7FFFF;
      obj->flags = masked;
      return 1;
    }
  }
  if (state->flags & 1)
  {
    flags = obj->flags;
    if (flags & 0x40000)
    {
      masked = flags & 0xFFFBFFFF;
 do { obj->flags = masked; return 1; } while (0);
    }
  }
  child = obj->state;
  if (child != 0)
  {
    word = child->core_flags;
    if (word & 0x1800)
    {
      return 1;
    }
    phase = (word >> 1) & 7;
    if (phase > 0)
    {
      return 1;
    }
  }
  return 0;
}
