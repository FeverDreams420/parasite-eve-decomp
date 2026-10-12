#include "common.h"
#include "pe1/render_packets.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/battle.h"
#include "pe1/draw_state.h"
#include "pe1/battle_status.h"
#include "pe1/battle_entity_anim.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
u8 g_BattleSpritePrimCountdown;
u8 g_MenuActiveMode;

void Gpu_SetupSprites(BattleEntity *entity, int unused, int icon) {
    int i;

    g_BattleSpritePrimCountdown = 0x1E;
    i = 0;
    icon = (u8)icon;
    do {
        RenderSpritePacket *marker;

        marker = &g_StatusMarkerPackets[(u8)i].sprite;
        marker->u = g_StatusIconPackets[(u8)i][icon].sprite.u;
        marker->v = g_StatusIconPackets[(u8)i][icon].sprite.v;
        marker->x = entity->renderObject.projected_x - 8;
        marker->y = entity->renderObject.projected_y - 0x10;
        g_StatusMarkerPackets[(u8)i].sprite.clut =
            g_StatusIconPackets[(u8)i][icon].sprite.clut;
        i++;
    } while ((u8)i < 2);
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


void Gpu_QueuePrimitive(void) {
    int slot;

    slot = g_ActiveDrawSlot;
    g_StatusMarkerPackets[slot].sprite.y -= 2;
    AddPrim((u32 *)g_OtBufferTable[slot] + 7, &g_StatusMarkerPackets[slot]);
    g_BattleSpritePrimCountdown--;
}

#include "common.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


/* Queue one status icon at the cursor and advance the cursor by one icon. */
#define INDEX_STRIDE(i) ((((((i) << 1) + (i)) << 3) - (i)) << 2) - (i)

#define EMIT_CODED(code_expr) do { \
    int packet_code; \
    register char *base asm("$6"); \
    register int code_off asm("$3"); \
    int idx; \
    int idx_off; \
    register void *prim asm("$5"); \
    register void *packet asm("$2"); \
    void *ot; \
    packet_code = (code_expr); \
    base = (char *)&g_StatusIconPackets[0][0].sprite; \
    code_off = ((packet_code << 3) - packet_code) << 2; \
    prim = base - 8; \
     \
    idx = g_ActiveDrawSlot; \
    prim = (char *)(code_off + (int)prim); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    prim = (char *)(idx_off + (int)prim); \
    packet = (char *)(idx_off + code_off + (int)base); \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    ot = g_OtBufferTable[idx]; \
    s0 += s3; \
    AddPrim((u32 *)ot + 7, prim); \
} while (0)

#define EMIT_BASE(base_sym) do { \
    char *base; \
    int idx; \
    void *prim; \
    void *packet; \
    base = (base_sym); \
    idx = g_ActiveDrawSlot; \
    prim = base - 8; \
    packet = (char *)(INDEX_STRIDE(idx) << 2); \
    prim = (char *)packet + (int)prim; \
    packet = (char *)packet + (int)base; \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    s0 += s3; \
    AddPrim((u32 *)g_OtBufferTable[idx] + 7, prim); \
} while (0)

#define EMIT_BASE_LAST(base_sym) do { \
    int idx; \
    char *base; \
    int idx_off; \
    register void *packet asm("$3"); \
    idx = g_ActiveDrawSlot; \
    base = (base_sym); \
    idx_off = INDEX_STRIDE(idx) << 2; \
    packet = (char *)(idx_off + (int)base); \
    idx <<= 2; \
    base = base - 8; \
    ((RenderSpritePacket *)packet)->x = s0; \
    ((RenderSpritePacket *)packet)->y = s2; \
    AddPrim(*(char **)((char *)g_OtBufferTable + idx) + 0x1C, (char *)idx_off + (int)base); \
} while (0)

void Gpu_DrawStatusIcons(void) {
    Combatant *s1;
    int s0;
    register int s2 asm("$18");
    register int s3 asm("$19");
    s32 *s4;
    u8 mode;
    int bits;
    register int code asm("$5");

    s1 = g_ActiveActor;
    mode = g_MenuActiveMode;
    s0 = 0xF;
    s4 = &s1->stateFlags;
    if (mode - 1U < 2U) {
        s0 = 0x121;
    }
    if (mode < 2) {
        s2 = 0xC1;
    } else {
        s2 = 0xF;
    }
    if ((short)s0 == 0x121) {
        s3 = -0x10;
    } else {
        s3 = 0x10;
    }

    {
        int bits0;
        bits0 = s1->stateFlags & 0xC;
        if (bits0 != 0) {
            code = bits0 ^ 0xC;
            code = code < 1;
            code <<= 2;
            EMIT_CODED(code);
        }
    }

    {
        int tmp;
        tmp = s1->stateFlags;
        bits = tmp & 0x30;
    }
    if (bits != 0) {
        code = 1;
        if (bits == 0x30) {
            code = 5;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *s4;
        bits = tmp & 0xC0;
    }
    if (bits != 0) {
        code = 2;
        if (bits == 0xC0) {
            code = 6;
        }
        EMIT_CODED(code);
    }

    {
        int tmp;
        tmp = *s4;
        bits = tmp & 3;
    }
    if (bits != 0) {
        code = 3;
        if (bits == 3) {
            code = 7;
        }
        EMIT_CODED(code);
    }

    if (*s4 & 0x1000) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][8].sprite);
    }
    if (*s4 & 0x100) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][9].sprite);
    }
    if (*s4 & 0x400) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][10].sprite);
    }
    if (*s4 & 0x800) {
        EMIT_BASE((char *)&g_StatusIconPackets[0][11].sprite);
    }
    if (*s4 & 0x200) {
        EMIT_BASE_LAST((char *)&g_StatusIconPackets[0][12].sprite);
    }
}
