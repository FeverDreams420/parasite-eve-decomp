#ifndef PE1_ROOM_M123_BEACON_H
#define PE1_ROOM_M123_BEACON_H

#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_transform_point.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/render_prim.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"

/* room_m123 joint beacon (func_80193618): follows joint 0x17 of the actor,
 * swells, pulses and fades, tags the player once on contact, and draws a glow
 * sprite pair with star fans, a fading ground ring of three sprites, a
 * turning model while the effect starts and three passes of a second model. */
typedef struct RoomM123Beacon {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector rotation;
    /* 0x18 */ int height;
    /* 0x1C */ int width;
    /* 0x20 */ int depth;
    /* 0x24 */ s16 alpha;
    /* 0x26 */ s16 fade;
    /* 0x28 */ s16 armed;
    /* 0x2A */ s16 phase;
    /* 0x2C */ s16 timer;
    /* 0x2E */ s16 glow;
    /* 0x30 */ s16 shade;
} RoomM123Beacon;

typedef struct RoomM123BeaconNode {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u8 reserved04[0x14];
    /* 0x18 */ u8 *state;
} RoomM123BeaconNode;

typedef struct RoomM123BeaconPool {
    /* 0x000 */ RoomM123BeaconNode *node;
    /* 0x004 */ u8 reserved004[0x234];
    /* 0x238 */ GteMatrix *matrix;
} RoomM123BeaconPool;

typedef struct RoomM123BeaconChannel {
    s32 reserved[2];
    RoomM123BeaconPool *pool;
} RoomM123BeaconChannel;

typedef struct RoomM123BeaconActor {
    u8 reserved[0x4C];
    u32 flags;
} RoomM123BeaconActor;

typedef struct RoomM123BeaconEntity {
    RoomM123BeaconActor *actor;
} RoomM123BeaconEntity;

typedef struct RoomM123BeaconEvent {
    /* 0x00 */ u8 reserved[0xD];
    /* 0x0D */ u8 active;
    /* 0x0E */ u8 reserved0E[4];
    /* 0x12 */ s16 variant;
} RoomM123BeaconEvent;

typedef struct RoomM123BeaconOffset {
    s32 word[2];
} __attribute__((packed)) RoomM123BeaconOffset;

typedef struct RoomM123BeaconArchive {
    void *channel;
} RoomM123BeaconArchive;

typedef struct RoomM123BeaconColor {
    u8 r, g, b, code;
} RoomM123BeaconColor;

extern RoomM123BeaconChannel *D_800F32D0;
extern RoomM123BeaconEvent *D_800E2368;
extern RoomM123BeaconEntity *D_8009D254;
extern RoomM123BeaconArchive D_800B0E64;
extern RoomM123BeaconOffset D_8018F1E8;
extern u8 *D_80195698;
extern u8 *D_8019569C;
extern u8 D_8019550C[];
extern u16 D_800E11FA;
extern u16 D_800E11EA;

u8 *func_8006E498(void *channel, int id);
void func_800C6D5C(u8 *data, u8 x_offset, u8 y_offset);
void func_800C6FA0(u8 *data, u16 factor);
void func_800D3BC8(GteShortVector *position, int scale_x, int scale_y, int texture,
                   int clut, int page, int intensity, int unused, int angle);
void func_800CE8F0(void *pool, int index, void *offset, void *position);
int func_800CEB8C(void *position, GteShortVector *target, int radius);
u16 GetClut(int x, int y);
GteMatrix *MulMatrix0(GteMatrix *first, GteMatrix *second, GteMatrix *out);
int rcos(int angle);
int rsin(int angle);

#endif
