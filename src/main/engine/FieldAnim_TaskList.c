#include "pe1/field_anim.h"
#include "pe1/scene_assets.h"
#include "pe1/psyq_bios.h"
#include "pe1/battle.h"
#include "pe1/field_effect_modules.h"

/* Binds task program `index` to the owner's task context. */
int FieldAnim_BindTaskProgram(FieldAnimTaskOwner *owner, int index)
{
    FieldAnimTaskProgram *program = g_FieldAnimTaskPrograms[index];

    if (program == 0) {
        printf(D_800C2244, index);
        return -1;
    }
    owner->tasks.table = &program->table;
    owner->tasks.pc.script = program->script;
    return 0;
}

/* Effect resource `index`: the two texture pages, then the third resource. */
void *FieldAnim_GetEffectResource(int index) {
    void *ret;

    switch (index) {
    case 0:
        ret = g_FieldEffectTexturePage0;
        return ret;
    case 1:
        ret = g_FieldEffectTexturePage1;
        return ret;
    case 2:
        ret = g_FieldEffectResource2;
        return ret;
    default:
        ret = 0;
        return ret;
    }
}
int func_800CE560(char *out, int stride, int count, FieldAnimTaskCallback callback)
{
  int i;
  register char *entry;
  FieldAnimCallbackList *list = (FieldAnimCallbackList *)out;
  stride += 4;
  entry = out;
  entry = entry + 0xC;
  list->callback = callback;
  list->stride = stride;
  list->count = count;
  for (i = 0; i < count; i++)
  {
    ((FieldAnimCallbackListEntry *)entry)->active = 0;
    entry += stride;
  }

  return (stride * count) + 0xC;
}

int func_800CE5AC(void *arg0, int arg1, int arg2, int arg3, void *arg4) {
    char *header;
    char *entry;
    FieldAnimCallbackList *list;
    int stride;
    int i;

    i = 0;
    stride = arg2 + 4;
    header = D_800F33E0->end + arg1;
    list = (FieldAnimCallbackList *)header;
    entry = header + 0xC;

    *(char **)arg0 = header;
    list->callback = (FieldAnimCallbackListCallback)arg4;
    list->stride = stride;
    list->count = arg3;

    while (i < arg3) {
        ((FieldAnimCallbackListEntry *)entry)->active = 0;
        i++;
        entry += stride;
    }

    return (stride * arg3) + 0xC;
}

void *func_800CE610(char *list) {
    char frame[8];
    FieldAnimCallbackList *base;
    char *entry;
    register int slot_or_count;
    register int count asm("$6");
    register int stride asm("$7");
    int i;
    void *ret;

    (void)frame;
    ret = list;
    asm volatile("" : "=r"(ret) : "0"(ret));
    base = (FieldAnimCallbackList *)list;
    asm volatile("" : "=r"(base) : "0"(base));
    entry = list + 0xC;
        slot_or_count = ((FieldAnimCallbackList *)ret)->count;
    stride = ((FieldAnimCallbackList *)ret)->stride;
    i = 0;
    if (slot_or_count > 0) {
        count = slot_or_count;
        while (1) {
            slot_or_count = (int)entry;
            if (((FieldAnimCallbackListEntry *)entry)->active == 0) {
                break;
            }
            i++;
            entry += stride;
            if (i >= count) {
                break;
            }
        }

        if (i < base->count) {
            ((FieldAnimCallbackListEntry *)slot_or_count)->active = 1;
            ((FieldAnimCallbackListEntry *)slot_or_count)->age = 0;
            return entry + 0x4;
        }
    }

    return 0;
}

int func_800CE688(void *data) {
    FieldAnimCallbackList *list = (FieldAnimCallbackList *)data;
    int saved_age = D_800E27EC;
    char *entry = (char *)list->entries;
    int i;
    int active = 0;
    FieldAnimTaskCallback callback = list->callback;
    int stride = list->stride;

    for (i = 0; i < list->count;) {
        if (((FieldAnimCallbackListEntry *)entry)->active != 0) {
            active++;
            D_800E27EC = ((FieldAnimCallbackListEntry *)entry)->age;
            if (callback(1, ((FieldAnimCallbackListEntry *)entry)->state, D_800E2368->argument) != 0) {
                ((FieldAnimCallbackListEntry *)entry)->active = 0;
            } else {
                ((FieldAnimCallbackListEntry *)entry)->age++;
            }
        }
        i++;
        entry += stride;
    }
    D_800E27EC = saved_age;
    return active;
}

int func_800CE78C(char *data) {
    FieldAnimCallbackList *list = (FieldAnimCallbackList *)data;
    int saved_age = D_800E27EC;
    char *entry = (char *)list->entries;
    int i;
    int active = 0;
    FieldAnimTaskCallback callback = list->callback;
    int stride = list->stride;

    for (i = 0; i < list->count;) {
        if (((FieldAnimCallbackListEntry *)entry)->active != 0) {
            active++;
            D_800E27EC = ((FieldAnimCallbackListEntry *)entry)->age;
            callback(2, ((FieldAnimCallbackListEntry *)entry)->state, D_800E2368->argument);
        }
        i++;
        entry += stride;
    }
    D_800E27EC = saved_age;
    return active;
}

void func_800CE870(char *arg0, int arg1, s16 *arg2) {
    BattleEntity *actor = (BattleEntity *)arg0;

    switch (arg1) {
    case 0:
        arg2[0] = actor->renderObject.matrices[0].translation[0];
        arg2[1] = actor->renderObject.matrices[0].translation[1];
        arg2[2] = actor->renderObject.matrices[0].translation[2];
        break;
    case 1: {
        int value;

        value = actor->posX.parts.integer;
        arg2[0] = value;
        value = actor->posY.parts.integer;
        arg2[1] = value;
        value = actor->posZ.parts.integer;
        arg2[2] = value;
        break;
    }
    }
}
