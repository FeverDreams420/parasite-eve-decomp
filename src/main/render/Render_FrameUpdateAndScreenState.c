/* MASPSX_FLAGS: --expand-div */
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_nop.h"
#include "pe1/render_object.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
#include "pe1/render_camera.h"
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_prim.h"
#include "pe1/render_buffers.h"

/* Initialize camera projection and save the active actor screen position. */
int Render_PrepareFrame(void) {
    struct {
        short vector[4];
        void *result;
        union { int word; short xy[2]; } projected;
    } local;
    unsigned int *state = D_800B89F8;
    Render_InitViewState(state, state + 8);
    if (Geo_RenderMeshList((void *)D_800B0E40, &local.result)) return -2;
    SetGeomScreen(state[8]);
    Obj_ResetAllEntries();
    if (D_8009D254) {
        int x = D_8009D254->posX.parts.integer;
        int y, z;
        local.vector[0] = x;
        y = D_8009D254->posY.parts.integer;
        local.vector[1] = y;
        z = D_8009D254->posZ.parts.integer;
        local.vector[2] = z;
    } else local.vector[0] = local.vector[1] = local.vector[2] = 0;
    {
        /* Matching debt: a volatile register input is the only form found
         * that keeps the retail 48-byte frame (the 8 bytes above the
         * projected point are never accessed). */
        register volatile unsigned short cx asm("$6") = 160;

        gte_SetGeomOffset(cx, 112);
    }
    {
        GteMatrix **slot = &D_800BCFA4.value;
        register GteMatrix *view asm("$6");

        asm("" : "=r"(slot) : "0"(slot));
        view = *slot;
        gte_ldrotmatrix(view);
        gte_ldtransmatrix(view);
    }
    gte_ldv0(local.vector);
    gte_rtps();
    gte_stsxy2(&local.projected);
    {
        int ret = 0;
        int word = local.projected.word;
        int x = local.projected.xy[0];

        D_800BCFB4 = x;
        D_800BCFB6 = word >> 16;
        return ret;
    }
}

int Scene_IsBattleMode(void);
int Geo_BuildMeshList(void);
int Render_StepFade(void);
int Render_ApplyScreenTint(void);

int Render_Update(void) {
    Render_DrawSprite();
    Geom_StepGroupAnimations();
    Scene_IsBattleMode();
    Geo_BuildMeshList();
    Render_StepFade();
    Render_ApplyScreenTint();
    return 0;
}

/* Reset the screen tint state embedded in the scroll state: white target,
 * black start colour, two full-screen tiles and their draw-mode packets. */
s32 CdRom_InitScreenState(void) {
    GeomScrollState *state = &g_GeomScroll.state;
    s32 i;

    g_GeomScroll.state.position.tint.target_b = 0xFF;
    g_GeomScroll.state.position.tint.target_g = 0xFF;
    g_GeomScroll.state.position.tint.target_r = 0xFF;
    g_GeomScroll.state.position.tint.fade_mode = 1;
    g_GeomScroll.state.position.tint.start_b = 0;
    g_GeomScroll.state.position.tint.start_g = 0;
    g_GeomScroll.state.position.tint.start_r = 0;
    g_GeomScroll.state.position.tint.blend_mode = 2;
    for (i = 0; i < 2; i++) {
        state->position.tint.tiles[i].tag.length = 3;
        state->position.tint.tiles[i].command = 0x60;
        state->position.tint.tiles[i].r = state->position.tint.target_r;
        state->position.tint.tiles[i].g = state->position.tint.target_g;
        state->position.tint.tiles[i].b = state->position.tint.target_b;
        state->position.tint.tiles[i].x = 0;
        state->position.tint.tiles[i].y = 0;
        state->position.tint.tiles[i].width = 320;
        state->position.tint.tiles[i].height = 224;
        state->position.tint.tiles[i].command |= 2;
        state->position.tint.modes[i].tag.length = 1;
        state->position.tint.modes[i].command = 0xE1000400 | ((state->position.tint.blend_mode & 3) << 5);
    }
    state->position.tint.duration = 0;
    state->position.tint.frame = 0;
    return 0;
}

/* A packet tag seen as a pointer and as the address word the DMA chain
 * links through. */
typedef union RenderTagAddress {
    RenderGpuTag *tag;
    u32 word;
} RenderTagAddress;

/* PSY-Q addPrim; the ordering pointer is re-evaluated after the tag write. */
#define linkPrimitive(ordering, packet) { \
    RenderTagAddress link; \
    (packet)->address = ((RenderGpuTag *)(ordering))->address; \
    link.tag = (packet); \
    ((RenderGpuTag *)(ordering))->address = link.word; \
}

int Render_SetCDDCSlot(void) {
    GeomScrollState *state = &g_GeomScroll.state;
    int mode = D_800BCFEE & 3;
    int stop = D_800BCFEE & 4;
    int divisor, frame;
    int slot;
    int modeSlot;
    RenderBufferPrefix *buffers;

    if (!mode) {
        return 0;
    }
    if (mode == 2) {
        divisor = D_800BCFF6 - 1;
        if (divisor <= 0) {
            divisor = 1;
        }
        frame = D_800BCFF8;
        state->position.tint.tiles[g_ActiveDrawSlot].r =
            D_800BCFF0 + (D_800BCFE8 - D_800BCFF0) * frame / divisor;
        state->position.tint.tiles[g_ActiveDrawSlot].g =
            D_800BCFF2 + (D_800BCFEA - D_800BCFF2) * frame / divisor;
        state->position.tint.tiles[g_ActiveDrawSlot].b =
            D_800BCFF4 + (D_800BCFEC - D_800BCFF4) * frame / divisor;
    } else {
        state->position.tint.tiles[g_ActiveDrawSlot].r = D_800BCFE8;
        state->position.tint.tiles[g_ActiveDrawSlot].g = D_800BCFEA;
        state->position.tint.tiles[g_ActiveDrawSlot].b = D_800BCFEC;
    }
    buffers = &D_800B0E38;
    slot = g_ActiveDrawSlot;
    linkPrimitive(&buffers->ordering[slot][3],
                  &state->position.tint.tiles[slot].tag);
    (slot + state->position.tint.modes)->tag.length = 1;
    modeSlot = g_ActiveDrawSlot;
    (modeSlot + state->position.tint.modes)->command =
        0xe1000400 | ((state->position.tint.blend_mode & 3) << 5);
    linkPrimitive(&buffers->ordering[modeSlot][3],
                  &state->position.tint.modes[modeSlot].tag);
    if (mode == 2) {
        state->position.tint.frame++;
        if (state->position.tint.frame >= state->position.tint.duration) {
            if (stop) {
                state->position.tint.fade_mode = 0;
            } else {
                state->position.tint.fade_mode = 1;
            }
        }
    }
    return 0;
}
