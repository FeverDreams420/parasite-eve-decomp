/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_cmd.h"
#include "pe1/pm.h"
#include "pe1/battle_runtime.h"

/* MASPSX_FORCE_G0 keeps the battle pointers and the effect result absolute,
 * as retail; the COMMON asset result supplies the small-data metadata of the
 * one $gp-relative word. */
int D_8009D200;

void Battle_DispatchEntityEffect(void) {
    int action_id;
    BattleAction *action;
    BattleAction *action2;
    int action_id2;
    BattleEntity *next_entity;
    int next_id;
    int result;
    u32 word10;

    action = D_8009D278->action;
    action_id = action->actionCode.actionId;

    if (action_id == 6) {
        if ((action->attackWord & 0x3FF) == 0) {
            BattleCmd_CommitAmmoAndUpdate(action);
        }
        D_8009D200 = Scene_LoadRoomAssets(3, D_8009D254);
        return;
    }

    if (action_id == 8) {
        D_8009D2FC = Scene_LoadRoomAssets(6, D_8009D254);
        return;
    }

    if ((action->attackWord & 0x3FF) == 0) {
        BattleCmd_CommitAmmoAndUpdate(action);
    }

    action2 = D_8009D278->action;
    action_id2 = action2->actionCode.actionId;

    if (action_id2 == 5 || ((word10 = action2->turnWord) & 0x1F00) != 0) {
        result = Scene_LoadRoomAssets(0, D_8009D254);
        next_entity = D_8009D254;
        next_id = 5;
    } else {
        if ((action_id2 == 2) || ((word10 & 0xC0) == 0x80)) {
            result = Scene_LoadRoomAssets(1, D_8009D254);
        } else {
            result = Scene_LoadRoomAssets(2, D_8009D254);
        }
        next_entity = D_8009D254;
        next_id = 4;
    }

    D_8009D200 = result;
    D_8009D2FC = Scene_LoadRoomAssets(next_id, next_entity);
}
