#include "common.h"
#include "../room_lib/room_lib.h"
#include "pe1/pe_image.h"
#include "pe1/field_glow_sprite.h"

typedef struct SceneE08MatrixWords {
    int words[8];
} SceneE08MatrixWords;

typedef struct SceneE08State {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
    u16 timer;
    u16 counter;
    u16 flag;
} SceneE08State;

#include "func_80190060_externs.h"

void func_80190060(RoomEnt *ent, void *unused, SceneE08State *state) {
    SceneE08MatrixWords *matrix;

    func_800C2B40(state);
    state->link = ent->link;
    matrix = (SceneE08MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    state->asset = Asset_SearchByKeyType(0x2E);
    state->timer = 30;
    state->counter = 0;
    state->flag = 0;

    D_801994B8.cell = 0x46;
    D_801994B8.clut = 3;
    D_801994B8.offset = -400;
    D_801994B8.depth = 128;
    D_801994B8.r = 240;
    D_801994B8.g = 64;
    D_801994B8.b = 64;
    D_801994B8.flip = 0;

    D_801995A8.cell = 0xA4;
    D_801995A8.clut = 10;
    D_801995A8.offset = -30;
    D_801995A8.depth = 128;
    D_801995A8.r = 160;
    D_801995A8.g = 128;
    D_801995A8.b = 240;
    D_801995A8.flip = 0;

    D_801995B8.cell = 0x44;
    D_801995B8.clut = 2;
    D_801995B8.offset = -30;
    D_801995B8.depth = 128;
    D_801995B8.r = 160;
    D_801995B8.g = 128;
    D_801995B8.b = 240;
    D_801995B8.flip = 0;

    D_80199690.cell = 0x4C;
    D_80199690.clut = 5;
    D_80199690.offset = -30;
    D_80199690.depth = 128;
    D_80199690.r = 160;
    D_80199690.g = 128;
    D_80199690.b = 240;
    D_80199690.flip = 0;
}
