#include "pe1/field_sprite_params.h"
#include "room_m123_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte_types.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_owner_actor.h"

extern GteShortVector D_8018F1F8;
extern GteShortVector D_8018F1CC;
extern char D_80195624[];
extern int rcos(int);
extern u16 GetClut(int, int);
extern u16 D_800E11E8;
extern void func_800CE8F0(void *, int, void *, void *);
extern int func_80071A54(void);

int func_80194F68(int mode, RoomM123Wave *wave) {
    int color[2];
    GteShortVector position = D_8018F1F8;
    int result, scale, kind, palette;
    u16 clut;
    switch (mode) {
    case 1:
        if (wave->state != 0) break;
        wave->frame++;
        wave->y -= 3;
        if ((s16)wave->frame >= 24) return 1;
        break;
    case 2:
        if (wave->state != 0) break;
        func_800CF3AC(D_80195624, color, (s16)wave->frame);
        result = rcos((D_800E27EC << 10) / 24);
        position.z = D_800E27EC << 5;
        scale = result / 2 + 2048;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20((GteShortVector *)wave, (GteRotation *)&position, scale, scale,
                      (s16)D_800F3368.parameter02 + 220, clut, 1, 128, (struct RenderColor *)color);
        break;
    }
    return 0;
}

typedef struct RoomM123Burst {
    u16 frame;
    u16 phase;
    u16 x, y, z;
    u16 pad;
} RoomM123Burst;

int func_80195114(int mode, RoomM123Burst *burst, int *choice) {
    GteShortVector position = D_8018F1CC;
    RoomM123Wave *child;
    int value;

    switch (mode) {
    case 0:
        value = *choice ? 34 : 40;
        func_800CE8F0(ANIM_OWNER_AS(RoomM123Pool)->pool, value, &position, &burst->x);
        burst->frame = 0;
        burst->phase = 0;
        func_800D3F64(0x581, func_800D3FD8());
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 12, 24,
                             (FieldAnimCallbackListCallback)func_80194F68);
    case 1:
        if (D_800E27EC < 32) {
            child = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (child != 0) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                {
                    int sample = func_80071A54();
                    int previous = child->x;
                    child->x = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->y;
                    child->y = (previous - 128) + (sample & 255);
                }
                {
                    int sample = func_80071A54();
                    int previous = child->z;
                    child->z = (previous - 128) + (sample & 255);
                }
                child->state = 0;
                child->frame = 0;
            }
        }
        if (D_800E27EC >= 8) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        palette = D_800E2850[paletteIndex];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
    }
    return 0;
}
