#include "common.h"
#include "../room_lib/room_lib.h"
#include "pe1/pe_image.h"
#include "pe1/field_glow_sprite.h"

typedef struct {
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 w5;
    s32 w6;
    s32 w7;
} Overlay178MatrixWords;

typedef struct {
    RoomLink *link;
    Overlay178MatrixWords matrix;
    void *asset;
} Overlay178InitState;


#include "func_8018F3C4_externs.h"

void func_800C2B40(void *arg0);

void func_8018F3C4(RoomEnt *ent, void *arg1, Overlay178InitState *state) {
    Overlay178MatrixWords *matrix;

    func_800C2B40(state);

    state->link = ent->link;
    matrix = (Overlay178MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->asset = Asset_SearchByKeyType(0x2E);

    D_80199670.cell = 0x46;
    D_80199670.clut = 3;
    D_80199670.offset = -0x1E;
    D_80199670.depth = 0x80;
    D_80199670.r = 0xA0;
    D_80199670.g = 0x80;
    D_80199670.b = 0xF0;
    D_80199670.flip = 0;
    D_801994A8.cell = 0x46;
    D_801994A8.clut = 3;
    D_801994A8.offset = -0x190;
    D_801994A8.depth = 0x80;
    D_801994A8.r = 0xA0;
    D_801994A8.g = 0x80;
    D_801994A8.b = 0xF0;
    D_801994A8.flip = 0;
}
