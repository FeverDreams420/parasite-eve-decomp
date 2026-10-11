/* MASPSX_FLAGS: --expand-div */
#include "pe1/projection_slot.h"
#include "common.h"
#include "pe1/geom_state.h"
#include "pe1/render_camera.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/render_lighting.h"
#include "pe1/render_object.h"
#include "pe1/field_movement.h"
#include "pe1/battle_runtime.h"
#include "pe1/render_prim.h"
#include "pe1/render_tint.h"

/* Resets the view state at the start of a room draw: render flags, the
 * scroll and camera words, the light colour, the mesh group and depth, and
 * points the view at the given matrix and projection distance. */
int Render_InitViewState(void *matrix, void *screen) {
    g_GeomScroll.words.flags = 0x70;
    D_800BCFFC = 0x60;
    D_800BCFFE[0] = 0x180;
    D_800BD025.b = 0xFF;
    D_800BD025.g = 0xFF;
    D_800BD025.r = 0xFF;
    g_GeomScroll.words.position = 0;
    g_GeomScroll.words.saved = 0;
    g_GeomScroll.words.screenOffset = 0;
    g_GeomScroll.words.start = 0;
    g_GeomScroll.words.target = 0;
    g_GeomScroll.words.timer = 0;
    g_GeomScroll.words.viewMatrix = matrix;
    g_GeomScroll.words.projectionDistance = screen;
    g_GeomScroll.words.savedBoundsX = 0;
    g_GeomScroll.words.savedBoundsY = 0;
    g_GeomScroll.words.cameraOffset = 0;
    g_GeomGroupSel = 0;
    D_800BD022 = 0;
    D_800BD020 = 0;
    D_800BD024 = 0;
    g_GeomScroll.words.origin = 0;
    return 0;
}

/* Clamps the camera position into the selected group's viewport. */
s32 Render_ClampCameraPosition(s32 x, s32 y)
{
    u16 cameraX = x;

    if (g_RenderStateFlags & 0x40) {
        GeomStateAddress address, base;
        CameraViewport *view;

        GEOM_STATE_OFFSET(address, base, entry_offset_1C, 0);
        view = &address.viewport[g_GeomGroupSel];
        if ((s16)cameraX < view->minX)
            g_GeomScroll.state.position.x = view->minX;
        else if (view->maxX < (s16)cameraX)
            g_GeomScroll.state.position.x = view->maxX;
        else
            g_GeomScroll.state.position.x = x;
        if ((s16)y < view->minY) {
            g_GeomScroll.state.position.y = view->minY;
            return 0;
        }
        if (view->maxY < (s16)y) {
            g_GeomScroll.state.position.y = view->maxY;
            return 0;
        }
        g_GeomScroll.state.position.y = y;
    }
    return 0;
}
/* Saves the selected viewport's bounds and opens them to the full range. */
int Render_SaveAndOpenBounds(void) {
    GeomStateAddress address, base;
    CameraViewport *view;

    GEOM_STATE_OFFSET(address, base, entry_offset_1C, 0);
    view = &address.viewport[g_GeomGroupSel];
    g_GeomScroll.state.position.boundsMinX = view->minX;
    g_GeomScroll.state.position.boundsMaxX = view->maxX;
    g_GeomScroll.state.position.boundsMinY = view->minY;
    g_GeomScroll.state.position.boundsMaxY = view->maxY;
    view->minX = -0x8000;
    view->maxX = 0x7FFF;
    view->minY = -0x8000;
    view->maxY = 0x7FFF;
    return 0;
}

/* Restores the bounds saved by Render_SaveAndOpenBounds. */
int Render_RestoreBounds(void) {
    GeomStateAddress address, base;
    CameraViewport *view;

    GEOM_STATE_OFFSET(address, base, entry_offset_1C, 0);
    view = &address.viewport[g_GeomGroupSel];
    view->minX = g_GeomScroll.state.position.boundsMinX;
    view->maxX = g_GeomScroll.state.position.boundsMaxX;
    view->minY = g_GeomScroll.state.position.boundsMinY;
    view->maxY = g_GeomScroll.state.position.boundsMaxY;
    return 0;
}
/* Matching debt: register pins, empty scheduling constraints, a $v1 clobber
 * and explicit signed-halving steps; see docs/CODE_ORGANIZATION.md. */


int Render_SetViewport(s16 *position) {
    struct {
        s16 point[4];
        s16 projected[2];
        unsigned reserved[10];
    } local;
    GeomScrollState *camera = &g_GeomScroll.state;
    GeomStateAddress geometry, viewAddress;
    register CameraViewport *view;
    register unsigned int cx asm("$11");
    GeomStateAddress offset_geometry;
    int flags = camera->flags;
    register int screen_x asm("$6");
    register int screen_y asm("$7");
    register int sum_x;
    register int sum_y;
    int view_x;
    register int view_y;
    register int half_width;
    int half_height;
    register int axisValue asm("$2");
    register int viewportValue asm("$3");
    register unsigned raw_width;
    register int target_x asm("$9");
    register int target_y;
    register int y_bound;
    register int selected_x;

    if (flags & 7)
        return -23;
    cx = 160;
    if (!(flags & 64))
        return -24;
    {
        int value = position[1];

        local.point[0] = value;
    }
    {
        int value = position[3];

        local.point[1] = value;
    }
    {
        int value = position[5];

        local.point[2] = value;
    }
    {
        unsigned int cy = 112;
        gte_SetGeomOffset(cx, cy);
    }
    {
        register u32 **address = &camera->position.matrixWords;
        asm("" : "=r"(address) : "0"(address));
        {
            register const GteMatrixWords *words asm("$11");
            words = (const GteMatrixWords *)*address;
            gte_ldrotmatrix(words);
            gte_ldtransmatrix(words);
        }
    }
    {
        register int distance = *D_800BCFA8.distance;
        gte_ctc2_26(distance);
    }
    gte_ldv0(local.point);
    gte_rtps();
    gte_stsxy2(local.projected);
    screen_x = local.projected[0];
    screen_y = local.projected[1];
    if (camera->flags & 128) {
        D_800BCFB4 = screen_x;
        D_800BCFB6 = screen_y;
    }
    geometry.state = g_GeomState;
    sum_x = D_800BCFB4 + screen_x;
    screen_x = sum_x / 2;
    D_800BCFB4 = screen_x;
    sum_y = D_800BCFB6 + screen_y;
    sum_y += (unsigned)sum_y >> 31;
    view_x = 320 - screen_x;

    offset_geometry.state = g_GeomState;

    screen_y = sum_y >> 1;
    D_800BCFB6 = screen_y;
    asm volatile("" : : : "memory");
    viewAddress.word = geometry.word + offset_geometry.state->entry_offset_1C;
    view = viewAddress.viewport;
    view += g_GeomGroupSel;
    asm("" : : "r"(view) : "$3");
    viewportValue = 224;

    raw_width = view->width;
    view_y = viewportValue - screen_y;
    raw_width <<= 16;
    viewportValue = (int)raw_width >> 16;
    viewportValue += raw_width >> 31;
    half_width = viewportValue >> 1;
    axisValue = half_width - 160;

    viewportValue = view->height;
    target_x = axisValue + screen_x;
    viewportValue = (unsigned)viewportValue << 16;
    axisValue = viewportValue >> 16;
    axisValue += (unsigned)viewportValue >> 31;
    half_height = axisValue >> 1;
    target_y = half_height + (screen_y - 112);
    viewportValue = view->minX;
    if (target_x < viewportValue ||
        (viewportValue = view->maxX, viewportValue < target_x)) {
        axisValue = viewportValue - 160;
        view_x = half_width - axisValue;
    }
    y_bound = view->minY;
    if (target_y < y_bound ||
        (y_bound = view->maxY, y_bound < target_y)) {
        axisValue = view->height;
        viewportValue = (s16)axisValue / 2;
        axisValue = y_bound - 112;
        view_y = viewportValue - axisValue;
    }
    D_800BCF94 = view_x;
    D_800BCF96 = view_y;
    selected_x = target_x;
    if (g_GeomScroll.state.flags & 64) {
        register int signed_y;
        s16 chosen_bound;
        register int upper_y;
        geometry.state = g_GeomState;
        viewAddress.word = geometry.word + geometry.state->entry_offset_1C;
        view = viewAddress.viewport;
        view += g_GeomGroupSel;
        viewportValue = (s16)target_x;
        axisValue = view->minX;
        chosen_bound = axisValue;
        if (viewportValue < axisValue) {
            signed_y = (unsigned)target_y << 16;
            D_800BCF8C.x = chosen_bound;
        } else {
            axisValue = view->maxX;
            chosen_bound = axisValue;
            if (axisValue < viewportValue) {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = chosen_bound;
            } else {
                signed_y = (unsigned)target_y << 16;
                D_800BCF8C.x = selected_x;
            }
        }
        signed_y >>= 16;
        axisValue = view->minY;
        chosen_bound = axisValue;
        if (signed_y < axisValue) {
            D_800BCF8E = chosen_bound;
            return 0;
        }
        axisValue = view->maxY;
        upper_y = axisValue;
        asm("" : "=r"(upper_y) : "0"(upper_y));
        if (axisValue < signed_y) {
            D_800BCF8E = upper_y;
            return 0;
        }
        D_800BCF8E = target_y;
    }
    return 0;
}
/* Moves the GTE screen offset to the current screen offset. */
int Render_SetGteScreenOffset(void) {
    gte_SetGeomOffset(g_GeomScroll.state.position.screenOffsetX,
                      g_GeomScroll.state.position.screenOffsetY);
    return 0;
}

/* Restores the GTE screen offset to the screen centre. */
int Render_ResetGteScreenOffset(void) {
    gte_SetGeomOffset(160, 112);
    return 0;
}

/* Starts a camera scroll from the current position to (x, y) over
 * `duration` frames; mode 8 selects the smoothed step. */
int Render_SetScrollMode(int x, int y, int duration, int mode) {
    int *flags = &g_RenderStateFlags;
    int value = *flags;
    u32 position;
    int bits;

    if (!(value & 0x40))
        return -19;
    position = g_GeomScroll.words.position;
    g_GeomScroll.state.position.elapsed = 1;
    g_GeomScroll.state.position.targetX = x;
    g_GeomScroll.state.position.targetY = y;
    g_GeomScroll.state.position.duration = duration;
    g_GeomScroll.words.start = position;
    position = value & ~0xF;
    if (mode != 8)
        bits = position | 1;
    else
        bits = position | 9;
    *flags = bits;
    return 0;
}

/* Advance the camera transition, then center the selected viewport. */
int Render_StepCameraTransition(void)
{
    unsigned int flags = g_RenderStateFlags;
    int mode, smooth;
    u16 dx, dy, duration;
    if (!(flags & 0x40))
        return -20;
    mode = flags & 7;
    smooth = flags & 8;
    if (mode != 1 && mode != 2 && mode != 3)
        return 0;
    {
        u16 targetX = D_800BCF9C, x = D_800BCF98;
        u16 targetY = D_800BCF9E, y = D_800BCF9A;
        u16 time = D_800BCFA0;
        duration = D_800BCFA2;
        dx = targetX - x;
        dy = targetY - y;
        if (!smooth) {
            int movedX = (short)dx * (short)time / (short)duration;
            int movedY = (short)dy * (short)time / (short)duration;
            D_800BCF8C.x = x + movedX;
            D_800BCF8E = y + movedY;
        } else {
            int weight = rcos((short)time * 2048 / (short)duration + 2048) + 4096;
            int movedX = (short)dx * weight / 8192;
            int movedY = (short)dy * weight / 8192;
            D_800BCF8C.x = D_800BCF98 + movedX;
            D_800BCF8E = D_800BCF9A + movedY;
        }
    }
    {
        GeomStateAddress views, base;
        CameraViewport *view;
        GEOM_STATE_OFFSET(views, base, entry_offset_1C, 0);
        view = &views.viewport[g_GeomGroupSel];
        {
            unsigned int offset = 160 - (D_800BCF8C.x - (short)view->width / 2);
            D_800BCF94 = offset;
        }
        {
            unsigned int offset = 112 - ((short)D_800BCF8E - (short)view->height / 2);
            D_800BCF96 = offset;
        }
    }
    if (mode == 1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 2;
    {
        u16 time = D_800BCFA0 + 1;
        D_800BCFA0 = time;
        if ((short)time > (short)duration) {
            unsigned int completed = g_RenderStateFlags;
            if ((completed & 7) == 2)
                g_RenderStateFlags = (completed & ~7) | 0x84;
            else
                g_RenderStateFlags = (completed & ~7) | 0x80;
        }
    }
    return 0;
}

/* Project a world-space point and start a clamped camera transition. */
/* Starts a camera scroll towards the projection of a world position,
 * clamped to the selected viewport (duration -1 = 30 frames, mode -1 =
 * plain step). */
int Render_UpdateScrollPosition(void *positionArg, int duration, int mode)
{
    int *position = positionArg;
    GteShortVector vector;
    s16 projected[2];
    GeomScrollState *camera = &g_GeomScroll.state;
    CameraViewport *view;
    int x, y;

    if (!(camera->flags & 64))
        return -21;
    D_800BCF98 = D_800BCF8C.x;
    D_800BCF9A = D_800BCF8E;
    vector.x = position[0] >> 16;
    vector.y = (position[1] >> 16) - (u16)D_800BCFFE[0];
    vector.z = position[2] >> 16;
    {
        /* Matching debt: see Render_PrepareFrame; this volatile register
         * input is the only form found that keeps the 24-byte frame. */
        register volatile unsigned short cx asm("$10") = 160;

        gte_SetGeomOffset(cx, 112);
    }
    {
        GteMatrix **slot = &g_GeomScroll.words.viewMatrix;
        register GteMatrix *matrix asm("$10");

        asm("" : "=r"(slot) : "0"(slot));
        matrix = *slot;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
    }
    gte_ldv0(&vector);
    gte_rtps();
    gte_stsxy2(projected);
    {
        GeomState *state = g_GeomState;
        GeomStateAddress table;

        table.state = g_GeomState;
        table.word += state->entry_offset_1C;
        view = &table.viewport[g_GeomGroupSel];
    }
    {
        u16 width = view->width,height = view->height;
        int halfX = (short)width/2-160,halfY = (short)height/2-112;
        x = halfX+projected[0];
        y = halfY+projected[1];
    }
    if (x < view->minX)
        x = view->minX;
    else if (x > view->maxX)
        x = view->maxX;
    if (y < view->minY)
        y = view->minY;
    else if (y > view->maxY)
        y = view->maxY;
    D_800BCF9C = x;
    D_800BCF9E = y;
    if (duration == -1)
        D_800BCFA2 = 30;
    else
        D_800BCFA2 = duration;
    D_800BCFA0 = 1;
    if (mode == -1)
        g_RenderStateFlags = (g_RenderStateFlags & ~7) | 3;
    else {
        unsigned int flags = g_RenderStateFlags & ~15;
        unsigned int bits = mode | 3;
        g_RenderStateFlags = flags | bits;
    }
    return 0;
}

void SetGeomScreen(int h);

int Gpu_LoadGeomState(int index) {
    GeomStateAddress table, base;
    CameraViewport *entry;
    u16 *dst;
    int value;

    GEOM_STATE_OFFSET(table, base, entry_offset_1C, index * 52);
    entry = table.viewport;
    *D_800BCFA8.distance = entry->prefix.gpu.geom_screen;
    SetGeomScreen(entry->prefix.gpu.geom_screen);

    dst = g_GeomVramPacketDst;
    dst[0] = entry->prefix.gpu.half02;
    dst = g_GeomVramPacketDst;
    dst[1] = entry->prefix.gpu.half04;
    dst = g_GeomVramPacketDst;
    dst[2] = entry->prefix.gpu.half06;
    dst = g_GeomVramPacketDst;
    dst[3] = entry->prefix.gpu.half08;
    dst = g_GeomVramPacketDst;
    dst[4] = entry->prefix.gpu.half0A;
    dst = g_GeomVramPacketDst;
    dst[5] = entry->prefix.gpu.half0C;
    dst = g_GeomVramPacketDst;
    dst[6] = entry->prefix.gpu.half0E;
    dst = g_GeomVramPacketDst;
    dst[7] = entry->prefix.gpu.half10;
    dst = g_GeomVramPacketDst;
    dst[8] = entry->prefix.gpu.half12;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[5] = entry->prefix.gpu.word14;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[6] = entry->prefix.gpu.word18;
    dst = g_GeomVramPacketDst;
    ((u32 *)dst)[7] = entry->prefix.gpu.word1C;

    g_GeomGroupSel = index;
    value = g_RenderStateFlags;
    g_RenderStateFlags = value | 0x80;
    return 0;
}

static inline int ClampLight(int value)
{
    if (value < 0) return 0;
    if (value > 255) return 255;
    return value;
}

/* Build two opposite Y lights and upload their color matrix to the GTE. */
int Render_InitRoomPrimState(void *objectArg)
{
    RenderObjectEntity *object = objectArg;
    int intensity, r, g, b;

    D_800BEA40.matrix.m[0][0] = 0;
    D_800BEA42 = 4096;
    D_800BEA44 = 0;
    intensity = object->shade + object->lightPositiveY;
    r = D_800BD025.r;
    g = D_800BD025.g;
    b = D_800BD025.b;
    intensity = ClampLight(intensity) << 4;
    D_800BEA60.matrix.m[0][0] = intensity * r / 256;
    D_800BEA66 = intensity * g / 256;
    D_800BEA6C = intensity * b / 256;

    D_800BEA46 = 0;
    D_800BEA48 = -4096;
    D_800BEA4A = 0;
    intensity = object->shade + object->lightNegativeY;
    intensity = ClampLight(intensity) << 4;
    D_800BEA62 = intensity * r / 256;
    D_800BEA68 = intensity * g / 256;
    D_800BEA6E = intensity * b / 256;
    D_800BEA4C = 0;
    D_800BEA4E = 0;
    D_800BEA50 = 0;
    D_800BEA64 = 0;
    D_800BEA6A = 0;
    D_800BEA70 = 0;
    gte_SetColorMatrix(D_800BEA60.words);
    return 0;
}

int CdRom_SetSeekPos(unsigned int arg0) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    D_800BCFE8 = 0xFF;
    g_ScreenTransitionTargetY = 0xFF;
    D_800BCFEC = 0xFF;
    g_ScreenTransitionState[0] = 2;
    g_ScreenTransitionMode = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    return 0;
}

int CdRom_SetScreenPos(int arg0, int arg1, int arg2, int arg3, unsigned short arg4) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;
    int mode;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    D_800BCFE8 = arg1;
    g_ScreenTransitionTargetY = arg2;
    D_800BCFEC = arg3;
    g_ScreenTransitionState[0] = 2;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    mode = arg4;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    if (mode >= 4) {
        g_ScreenTransitionMode = 1;
        return 0;
    }
    if (mode < 0) {
        g_ScreenTransitionMode = 1;
        return 0;
    }

    g_ScreenTransitionMode = arg4;
    return 0;
}

int Render_SetFadeColour(unsigned int arg0) {
    unsigned short old0;
    unsigned short old1;
    unsigned short old2;

    old0 = (u16)D_800BCFE8;
    old1 = g_ScreenTransitionTargetY;
    old2 = (u16)D_800BCFEC;

    g_ScreenTransitionState[0] = 6;
    D_800BCFE8 = 0;
    g_ScreenTransitionTargetY = 0;
    D_800BCFEC = 0;
    g_ScreenTransitionFadeColor = arg0;
    D_800BCFF8 = 0;
    D_800BCFF0 = old0;
    g_ScreenTransitionStartY = old1;
    D_800BCFF4 = old2;

    return 0;
}

/* Build the camera basis matrix from the camera yaw (plus the player's
 * facing quadrant while aiming): forward = R * Z, right = Y x forward,
 * up = forward x right. */
int Render_DrawSprite(void)
{
    GteMatrixStorage rotation;
    GteShortVector axis;
    GteVector up, right, newUp, forward;
    int angle, cosine, sine;
    if ((D_800BE9A0 & 0xF000) == 0x7000)
        angle = D_800BD022;
    else
        angle = D_800BD020;
    {
        int masked = angle & 4095;
        if (D_8009D254 && (D_8009D2E8 & 16)) {
            angle += 2048;
            angle += (((unsigned int)((Combatant *)D_8009D254->core)->stateFlags) >> 7) & 0xC00;
            masked = angle & 4095;
        }
        cosine = rcos(masked);
        sine = rsin(masked);
    }
    rotation.matrix.m[0][2] = sine;
    rotation.matrix.m[2][0] = -sine;
    rotation.matrix.m[1][1] = 4096;
    axis.z = 4096;
    rotation.matrix.m[0][0] = cosine;
    rotation.matrix.m[2][2] = cosine;
    rotation.matrix.t[0] = rotation.matrix.t[1] = rotation.matrix.t[2] = 0;
    rotation.matrix.m[0][1] = rotation.matrix.m[1][0] = rotation.matrix.m[1][2] = rotation.matrix.m[2][1] = 0;
    axis.x = 0;
    axis.y = 0;
    gte_ldrotmatrix(&rotation);
    gte_ldtransmatrix(&rotation);
    gte_ldv0(&axis);
    gte_rt();
    gte_stlvnl(&forward);
    up.y = 4096;
    up.x = 0;
    up.z = 0;
    gte_ldopv1_psyq(&up);
    gte_ldopv2(&forward);
    gte_op12_psyq();
    gte_stlvnl(&right);
    gte_ldopv1_psyq(&forward);
    gte_ldopv2(&right);
    gte_op12_psyq();
    gte_stlvnl(&newUp);
    D_800BD000.t[0] = 0;
    D_800BD000.t[1] = 0;
    D_800BD000.t[2] = 0;
    D_800BD000.m[0][0] = right.x;
    D_800BD000.m[0][1] = newUp.x;
    D_800BD000.m[0][2] = forward.x;
    D_800BD000.m[1][0] = right.y;
    D_800BD000.m[1][1] = newUp.y;
    D_800BD000.m[1][2] = forward.y;
    D_800BD000.m[2][0] = right.z;
    D_800BD000.m[2][1] = newUp.z;
    D_800BD000.m[2][2] = forward.z;
    return 0;
}
