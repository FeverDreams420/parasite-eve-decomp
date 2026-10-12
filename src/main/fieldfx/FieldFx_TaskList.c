#include "pe1/field_anim.h"
#include "pe1/scene_assets.h"
#include "pe1/psyq_bios.h"
#include "pe1/battle.h"
#include "pe1/field_effect_modules.h"

/* Binds task program `index` to the owner's task context. */
int FieldFx_BindTaskProgram(FieldAnimTaskOwner *owner, int index)
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
void *FieldFx_GetEffectResource(int index) {
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
    FieldAnimCallbackList *header;
    FieldAnimCallbackList *base;
    FieldAnimCallbackListEntry *slot;
    int count;
    int limit;
    int stride;
    int i;

    (void)frame;
    /* The parameter itself becomes the entry cursor, so the header is read
     * through two copies taken before it advances, as retail does. */
    header = (FieldAnimCallbackList *)list;
    base = (FieldAnimCallbackList *)list;
    list += 0xC;
    count = header->count;
    stride = header->stride;
    i = 0;
    if (count > 0) {
        limit = count;
        while (1) {
            slot = (FieldAnimCallbackListEntry *)list;
            if (slot->active == 0) {
                break;
            }
            i++;
            list += stride;
            if (i >= limit) {
                break;
            }
        }

        if (i < base->count) {
            slot->active = 1;
            slot->age = 0;
            return list + 0x4;
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
