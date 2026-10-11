#include "common.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"
#include "pe1/field_owner_actor.h"
#include "pe1/render_object.h"
#include "pe1/pe_image.h"
#include "pe1/gte.h"
#include "pe1/scene_assets.h"
#include "pe1/asset_find.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_flash_burst.h"


int func_800D3F64(int arg0, int arg1) {
    FieldActor *actor;
    s16 position[3];

    actor = D_800F32D0->actor;
    position[0] = actor->render_object.matrices->translation[0];
    position[1] = actor->render_object.matrices->translation[1];
    position[2] = actor->render_object.matrices->translation[2];
    return Asset_Find08w(arg0, arg1, position[0], position[1], position[2]);
}

int func_800D3FD8(void) {
    FieldAnimObjectPrefix *ctx;
    FieldActorState *node;
    int value;

    ctx = D_800F32D0;
    node = ctx->actor->state;
    value = 0x80;
    if (node != 0) {
        value = node->progress;
        if (value >= 0x41) {
            value = 0x80;
        }
    }
    return value;
}

int func_800D401C(int id)
{
    /* Reused for the callback's additional payload size after the slot scan. */
    int result;
    FieldAnimTaskContext *ctx = D_800E2368;
    FieldAnimTaskSlot *slot = ctx->slots;
    int size;
    char *payload;
    for (result = 0; result < 8; result++, slot++) {
        if (slot->id == 0xFFFF)
            break;
    }
    if (result == 8)
        return -1;
    ctx->count++;
    slot->id = id;
    slot->age = 0;
    size = ctx->table->sizes[id];
    payload = ctx->cursor;
    ctx->used += size;
    if (ctx->used > 0x97C) {
        payload = ctx->arena;
        ctx->used = size;
        ctx->cursor = payload;
    }
    slot->end = payload + size;
    slot->start = payload;
    D_800F33E0 = slot;
    result = ctx->table->callbacks[slot->id](0, payload, ctx->argument);
    if (!result)
        slot->end = 0;
    ctx->cursor += size + result;
    ctx->used += result;
    return size + result;
}

/* Runs the owner's task script until it yields, then steps every live task.
 * Opcodes: 0 stop (end of script), 1 set variable, 2 spawn task,
 * 3 count down a variable and jump while positive, 4 wait on a timer or
 * a variable, 5 toggle the owner's held state. */
int func_800D413C(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot;
    FieldActorState *state;
    unsigned int flags;
    int count;
    int running = 1;
    int result;
    int i;
    s16 op, a, b;

    D_800F32D0 = &owner->prefix;
    D_800E2368 = context;
    while (running) {
        if (context->delay != 0) {
            if (--context->delay != 0)
                break;
            context->delay = 0;
        }
        op = *context->pc.script;
        context->pc.script++;
        a = *context->pc.script;
        context->pc.script++;
        b = *context->pc.script;
        context->pc.script++;
        switch (op) {
        case 0:
            context->pc.script -= 3;
            if (context->count == 0) {
                owner->prefix.reserved00 = 4;
                if (D_800E2368->flags) {
                    if (D_800F32D0->actor->state != 0) {
                        D_800F32D0->actor->state->core_flags &= 0xC0FFFFFF;
                        *D_800F32D0->actor->state->action_state = 4;
                        D_800E2368->flags = 0;
                    }
                }
            }
            running = 0;
            break;
        case 1:
            context->variables[a] = b;
            break;
        case 2:
            func_800D401C(a);
            break;
        case 3:
            if ((s16)--context->variables[b] <= 0) {
                context->variables[b] = 0;
                running = 0;
            } else {
                /* Retail takes the operand as an absolute address. */
                context->pc.address = a - 6;
            }
            break;
        case 4:
            if (b != 1) {
                context->delay = a;
                running = 0;
            } else if (context->variables[a] == 0) {
                running = 0;
                context->pc.script -= 3;
            }
            break;
        case 5:
            if (a == 0) {
                D_800F32D0->actor->state->core_flags =
                    (D_800F32D0->actor->state->core_flags & 0xC0FFFFFF) | 0x1000000;
                D_800E2368->flags = 1;
            } else if (D_800E2368->flags && D_800F32D0->actor && D_800F32D0->actor->state) {
                u8 *mode = D_800F32D0->actor->state->action_state;
                if (*mode == 1)
                    *mode = 2;
            }
            break;
        }
    }
    if (D_800E2368->flags && D_800F32D0->actor && D_800F32D0->actor->state) {
        state = D_800F32D0->actor->state;
        flags = state->core_flags;
        count = (flags >> 24) & 0x3F;
        if (count >= 2)
            state->core_flags = (flags & 0xC0FFFFFF) | (((count - 1) & 0x3F) << 24);
        if ((int)((state->core_flags >> 24) & 0x3F) > 0) {
            if (*state->action_state == 0) {
                if (state->control10.command_value <= 0)
                    owner->prefix.reserved00 = 4;
            }
        }
        if (*state->action_state == 0) {
            if (state->core_flags & 0x180E)
                owner->prefix.reserved00 = 4;
            if (D_800F32D0->actor->mode < 2)
                return 0;
        }
    }
    if (context->flags)
        D_800F3428 = Asset_SearchByKeyType(owner->prefix.asset_type);
    slot = context->slots;
    for (i = 0; i < 8; i++, slot++) {
        if (slot->id != 0xFFFF) {
            D_800F33E0 = slot;
            D_800E27EC = slot->age;
            result = context->table->callbacks[slot->id](1, slot->start, context->argument);
            if (slot->end) {
                if (result == 2) {
                    if (!func_800CE688(slot->end))
                        result = 1;
                } else {
                    func_800CE688(slot->end);
                }
            }
            slot->age++;
            if (result == 1) {
                slot->id = 0xFFFF;
                context->count--;
            }
        }
    }
    return 0;
}

int func_800D4620(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot;
    int i;

    owner->prefix.reserved02[0] = 1;
    owner->prefix.reserved02[1] = 0;
    context->delay = 0;
    context->used = 0;
    context->cursor = context->arena;
    context->count = 0;
    context->flags = 0;
    for (i = 6; i >= 0; i--) {
        context->variables[i] = 0;
    }

    slot = context->slots;
    for (i = 0; i < 8; i++, slot++) {
        slot->id = 0xFFFF;
        slot->age = 0;
        slot->start = 0;
    }

    return 0;
}

int func_800D4698(FieldAnimTaskOwner *owner, int skip, int arg2, int arg3, int arg4, int arg5) {
    FieldAnimTaskContext *ctx;
    int (*callback)(int, int, int, int);

    ctx = &owner->tasks;
    if (skip == 0) {
        callback = ((FieldAnimTaskProgram *)owner->tasks.table)->initialize;
        D_800F32D0 = &owner->prefix;
        D_800E2368 = ctx;
        if (callback != 0) {
            owner->tasks.argument = callback(arg2, arg3, arg4, arg5);
        }
    }
    return 0;
}

/* Matching debt: four register pins for the matrix pointer and GTE words.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D4704(FieldAnimTaskOwner *owner)
{
    FieldAnimTaskContext *context = &owner->tasks;
    FieldAnimTaskSlot *slot = context->slots;
    int i;

    D_800F32D0 = &owner->prefix;
    D_800E2368 = context;
    if (context->flags)
        D_800F3428 = Asset_SearchByKeyType(owner->prefix.asset_type);
    for (i = 0; i < 8; i++, slot++) {
        if (slot->id != 0xFFFF) {
            gte_ldrotmatrix((const GteMatrixWords *)D_800BCFA4.value);
            gte_ldtransmatrix((const GteMatrixWords *)D_800BCFA4.value);
            D_800F33E0 = slot;
            D_800E27EC = slot->age;
            context->table->callbacks[slot->id](2, slot->start, context->argument);
            if (slot->end)
                func_800CE78C(slot->end);
        }
    }
    return 0;
}

int func_800D4850(u8 *state) {
    state[0] = 4;
    return 0;
}

#include "FieldEng_TaskSpawn_externs.h"

void func_800CECAC(void);
void func_800CED3C(int arg0);

int func_800D4860(void) {
    void **table;

    func_800CECAC();

    table = &D_800B0E20;
    D_800F32D4 = Asset_FindTable08ByU32Key(*table, 0x6DEAE684);
    D_800F32D8 = Asset_FindTable08ByU32Key(*table, 0x8AB5B684);
    D_800F3418 = Asset_FindTable08ByU32Key(*table, 0x5AB32504);
    D_800F342C = Asset_FindTable08ByU32Key(*table, 0x5AB32104);
    D_800F3474 = Asset_FindTable08ByU32Key(*table, 0x83ACD504);
    D_800E22D4 = Asset_FindTable08ByU32Key(*table, 0x57E8EB04);
    func_800CED3C(0);

    return 0;
}
