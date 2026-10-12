/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/render_animation_frame.h"

/* COMMON supplies small-data metadata to MASPSX; the extern words stay
 * absolute. */
int g_BattleAttackAnimFrame;

void Render_AnimationFrame(void) {
    int size;
    int width;
    int frame;
    RenderAnimActor *early_actor;
    RenderAnimActor *late_actor;
    RenderAnimPlayer *player;
    int draw_index;
    int delta;
    int scaled_frame;
    /* Preserves the original GCC 2.7.2 stack frame. */
    char frame_work[12];
    int animation_id;
    int zero_arg;
    int x;
    int y;
    int z;

    frame = g_BattleAttackAnimFrame;
    if (frame < 10U) {
        early_actor = (RenderAnimActor *)g_ActiveActor;
        if ((early_actor->modeFlags & 0x30) == 0x10) {
            scaled_frame = (frame << 2) + frame;
            size = scaled_frame << 18;
        } else {
            size = ((early_actor->frameData->frameCount << 16) * frame) / 10U;
        }
        width = g_BattleAttackAnimFrame << 6;
    } else {
        late_actor = (RenderAnimActor *)g_ActiveActor;
        size = 0xC80000;
        if ((late_actor->modeFlags & 0x30) != 0x10) {
            size = late_actor->frameData->frameCount << 16;
        }
        frame = 10;
        width = (g_BattleAttackAnimFrame << 4) + frame;
    }

    animation_id = 0x457;
    if (g_BattleAttackAnimFrame == 0) {
        player = (RenderAnimPlayer *)g_PlayerEntity;
        x = player->x;
        y = player->y;
        z = player->z;
        zero_arg = 0;
        Asset_Find08Alt(animation_id, zero_arg, x, y, z);
    }

    Render_SetGteScreenOffset();

    draw_index = g_ActiveDrawSlot;
    delta = Render_DrawWireframeModel(D_800B0DF8[0],
                                      ((RenderAnimPlayer *)g_PlayerEntity)->decoderData,
                                      size, width & 0xFFFE,
                                      g_OtBufferTable[draw_index] + 8,
                                      g_RenderAnimBufferBase[draw_index] +
                                           D_8009CDD8);
    D_8009CDD8 += delta;

    Render_ResetGteScreenOffset();
    g_BattleAttackAnimFrame++;
}
