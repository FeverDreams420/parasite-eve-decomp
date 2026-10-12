#include "pe1/room_m350_effects.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"

#include "RoomEffect_SineTransformSequence_externs.h"

int func_801996BC(int event, RoomM350SineParticle *object)
{
    GteShortVector position;
    GteRotation texture;
    if (event == 1) {
        int wave = rsin((unsigned int)D_800E27EC << 4);
        object->size = wave * object->amplitude >> 12;
        if (D_800E27EC >= 8) {
            object->brightness -= 8;
            if (object->brightness < 16) return 1;
        }
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        unsigned short handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 7 : palette + 3);
        func_800CEE20(&object->position, (GteRotation *)object->controls, object->size, object->size,
            128, handle, 1, object->brightness, 0);
        position.x = object->position.x;
        position.y = ANIM_OWNER_AS(RoomM350EffectActor)->instance->transform.t[1];
        position.z = object->position.z;
        texture.x = 1024;
        texture.y = D_800E27EC * 192;
        texture.z = 0;
        texture.flags = 1;
        func_800D004C(&position, 384, 384, 10, &texture, object->size,
            object->size, &D_8019A738, &D_8019A734, object->brightness, 1);
    }
    return 0;
}

typedef struct { short count, delay; } EmissionState;
#include "RoomEffect_SineTransformSequence_externs_1.h"

int func_8019988C(int event, EmissionState *state)
{
    switch (event) {
    case 0:

        state->count = 0;
        state->delay = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool, 24, 8,
                             (FieldAnimCallbackListCallback)func_801996BC);
        case 1:
    if (state->count < 24) {
        RoomM350SineParticle *effect;
        int random;
        int index;
        RoomM350EffectActor *actor;
        FieldActor *instance;
        state->delay--;
        if (state->delay != -1) return 0;
        effect = func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if (!effect) return 0;
        random = Inv_ScrambleGrid();
        actor = ANIM_OWNER_AS(RoomM350EffectActor);
        instance = (FieldActor *)actor->instance;
        index = (unsigned char)random;
        {
            register int byte asm("$4") = index;
            int count = instance->render_object.header->part_count;
            register int range asm("$3") = count - 1;
            register int product asm("$2");
            asm("" : "=r"(range) : "0"(range), "r"(count));
            product = byte * range;
            product /= 256;
            index = product + 1;
        }
        effect->position.x = instance->render_object.matrices[index].translation[0];
        effect->position.y = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[1];
        effect->position.z = ((FieldActor *)actor->instance)->render_object.matrices[index].translation[2];
        {
            int count = state->count;
            effect->brightness = 128;
            effect->amplitude = count * 2048 + 2048;
        }
        /* Keep the position/amplitude stores before the delay/count update. */
        asm volatile("" : : "m"(effect->amplitude));
        {
            int delay = random & 7;
            unsigned short count = state->count;
            delay += 12;
            state->delay = delay;
            state->count = count + 1;
        }
        {
            register int random asm("$4") = rand();
            int high;
            int y;
            int rawHigh;
            int bit;
            effect->controls[0] = (random & 255) + 512;
            asm volatile("" : :);
            rawHigh = random >> 8;
            high = rawHigh & 255;
            asm("" : : "r"(high));
            bit = random & 2;
            asm("" : : "r"(bit), "r"(random));
            if (bit) y = high + 768;
            else y = high - 1024;
            effect->controls[1] = y;
        }
        effect->controls[2] = 0;
        effect->controls[3] = 0;
    } else {
        D_8019A8C4 = 1;
        return 2;
    }
        break;
    case 2:

    D_800F3368.parameter00 = 64;
    D_800F3368.parameter02 = 4;
    D_800F3368.extent_x = 64;
    D_800F3368.extent_y = 64;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 5;
    D_800F3368.depth = 0;
    }
    return 0;
}
