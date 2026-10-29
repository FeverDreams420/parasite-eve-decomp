#include "common.h"
#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/asset_tim.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_services.h"
#include "pe1/cdrom.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/draw_buffers.h"
#include "pe1/font.h"
#include "pe1/game_state.h"
#include "pe1/pe_image.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_animation_frame.h"
#include "pe1/render_buffers.h"
#include "pe1/scene_assets.h"
#include "pe1/script_queue.h"

int DsPosToInt(CdlLOC *position);

typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 animGroup;
    u8 animSet;
    u8 animIndex;
    u8 frameSkip;
    u8 pad_10[0x06];
    u16 frame;
    u8 pad_18[0x02];
    u16 targetFrame;
    int frameStep;
    u8 pad_20[0x0A];
    s16 posXInteger; /* integer halves of the 16.16 position */
    u8 pad_2c[0x02];
    s16 posYInteger;
    u8 pad_30[0x02];
    s16 posZInteger;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankState {
    int flags;
    u8 pad_04[0x0D];
    u8 first_dynamic_entry;
    u8 active_id_slot;
} AkaoVoiceBankState;



/* End-of-frame overlay pass (draw the game-state primitives into the
 * current draw buffer with background clear off and send the queued AKAO
 * commands), soft-reset shutdown and screen vertical-offset update. These
 * contiguous routines share the default compiler profile and display state. */

void Boot_FinishFrame(void) {
    s32 one;
    int flags;

    if (!(g_GameState.flags & 0x200)) {
        flags = g_GameStateFlags;
        if (flags & 0x10) {
            one = 1;
            g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = 0;
            g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = one;
            PutDrawEnv(&g_RenderDrawEnvArray[g_ActiveDrawSlot]);
            DrawPrim(&g_GameState.fade_tpage);
            DrawPrim(&g_GameState.fade_tile);
            g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = one;
            g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = 0;
            Akao_Cmd_99_9B_9D(0);
        } else if (flags & 0x20) {
            Akao_Cmd_98_9A_9C(0);
        }
    }
}
