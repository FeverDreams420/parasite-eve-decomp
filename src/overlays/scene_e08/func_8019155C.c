#include "common.h"
#include "pe1/field_glow_sprite.h"
#include "../room_lib/room_lib.h"
#include "pe1/scene_archive.h"
#include "pe1/akao_send.h"
#include "pe1/asset_find.h"
#include "pe1/pe_image.h"

typedef struct {
    s32 words[8];
} SceneE08MatrixWords;

typedef struct {
    RoomLink *link;
    SceneE08MatrixWords matrix;
    void *asset;
} SceneE08InitState;

extern void func_800C2B40(void *);
extern struct FieldActor *g_PlayerEntity;
extern int D_8019956C, D_8019957C;
extern u8 *D_80199528;
extern u8 D_80199680, D_80199681, D_80199682, D_80199684, D_80199685, D_80199686;
extern s16 D_80199688, D_8019968A;
extern u8 D_801996A0[];
extern u8 D_801996A1, D_801996A2, D_801996A4, D_801996A5, D_801996A6;
extern s16 D_801996A8, D_801996AA;
extern FieldGlowSprite D_801996B0;

void func_8019155C(RoomEnt *ent, void *unused, SceneE08InitState *state) {
    SceneE08MatrixWords *matrix;
    void *asset;
    int half;
    RoomLink *player;
    register int intensity asm("$3");
    register int mode asm("$2");
    register int offset asm("$5");
    void *volatile *sound = D_800B0E64;
    void *handle;
    int playerX;
    register int playerZ asm("$3");

    func_800C2B40(state);
    D_80199528 = Asset_FindTable08ByU32Key(*sound, 0x10D8704);
    state->link = ent->link;
    matrix = (SceneE08MatrixWords *)state->link->p238;
    state->matrix = *matrix;
    asset = Asset_SearchByKeyType(0x2E);
    intensity = 0x80;
    state->asset = asset;
    mode = 8;
    offset = -400;

    D_80199688 = offset;
    D_801996A8 = offset;
    /* Empty compiler barriers retain the retail register lifetimes. */
    asm volatile ("" : "=r"(player) : "0"((RoomLink *)g_PlayerEntity));
    asm volatile ("" : "=r"(half) : "0"(0x80));
    D_80199685 = mode;
    D_801996A4 = 0x44;
    D_801996A5 = 2;
    D_801996B0.cell = 0x60;
    D_801996B0.clut = 0x40;
    D_80199684 = intensity;
    D_8019968A = half;
    D_80199680 = intensity;
    D_80199681 = intensity;
    D_80199682 = intensity;
    D_80199686 = 0;
    D_801996AA = half;
    D_801996A0[0] = intensity;
    D_801996A1 = intensity;
    D_801996A2 = intensity;
    D_801996A6 = 0;
    D_801996B0.offset = -500;
    D_801996B0.depth = half;
    D_801996B0.r = intensity;
    D_801996B0.g = intensity;
    D_801996B0.b = intensity;
    D_801996B0.flip = 0;

    handle = *sound;
    /* Keep the handle load ahead of the two player-coordinate loads. */
    asm volatile("" : : "r"(handle));
    playerZ = ((s16 *)player->pos)[5];
    playerX = ((s16 *)player->pos)[1];
    D_8019956C = playerX;
    D_8019957C = playerZ;
    if (handle != 0) {
        Akao_SendTableCommand(*sound, 0x603, 0, 0x80, 0x7F);
    }
}
