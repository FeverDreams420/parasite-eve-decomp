#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_anim_pool.h"
#include "pe1/asset_find.h"
typedef RoomM350TableShadeEntry TableEntry;
typedef RoomM350SoundOwner Owner;
typedef struct {
    Owner *owner;
    char reserved04[10];
    unsigned char id;
    char reserved0F[7];
    unsigned short frame;
    char reserved18[2];
    unsigned short previous;
} Animation;
typedef struct { int reserved[2]; Animation *animation; } Actor;

typedef struct { short remaining, delay; } State;

extern int D_800966EC[], D_8019A3C8[], D_8019A4DC[], D_8019A4E0[];
extern short D_8019A7A0[];

int func_801944F0(int event, void **object)
{
    short vector[4];
    if (event == 1) {
        if (D_800E27EC >= 8) {
            return 1;
        }
    } else if (event == 2) {
        int *base = D_800966EC;
        int counter = D_800E27EC;
        TableEntry *entry = (TableEntry *)((char *)base +
            (((unsigned int)(counter - 1) << 9) & 0x3E00));
        int size = entry->size;

        vector[0] = 0;
        vector[1] = 0;
        vector[2] = counter * 128;
        vector[3] = 0;
        func_800D004C(*object, 320, 320, 16, (struct GteRotation *)vector, size, size,
            (struct RenderColor *)D_8019A4DC, (struct RenderColor *)D_8019A3C8, entry->shade >> 5, 1);
        {
            int nextCounter = D_800E27EC;
            size = nextCounter * 512 + 2048;
            func_800D0728(*object, 256, 512, 16, (struct GteRotation *)vector, size, size,
                (struct RenderColor *)D_8019A3C8, (struct RenderColor *)D_8019A4E0,
                (short)base[((unsigned int)(nextCounter - 1) << 8) & 0xF00] >> 5, 1);
        }
    }
    return 0;
}

int func_80194654(int event, State *state)
{
    switch (event) {
    case 0:
        state->remaining = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool, 4, 4,
                             (FieldAnimCallbackListCallback)func_801944F0);
    case 1:
        {
            Animation *animation = ANIM_OWNER_AS(Actor)->animation;
            if (animation->id == 13) {
                int frame = animation->frame;
                if (frame >= 26) {
                    return 2;
                }
                {
                    int previous = animation->previous;
                    if (frame >= 8 && previous < 8) {
                        state->remaining = 4;
                        state->delay = 0;
                    }
                }
            }
            if (state->remaining != 0 && state->delay-- <= 0) {
                short **effect = func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
                if (effect == 0) {
                    return 0;
                }
                *effect = D_8019A7A0;
                state->delay = 1;
                state->remaining--;
                Asset_Find08w(1479, ANIM_OWNER_AS(Actor)->animation->owner->soundMode,
                    (*effect)[0], (*effect)[1], (*effect)[2]);
            }
        }
        break;
    case 2:
        break;
    }
    return 0;
}
