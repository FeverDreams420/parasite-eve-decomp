/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
/* Script opcodes: actor screen position, start-up memory-card dialog,
 * received-item notice, inventory command and the AKAO/CD sound command.
 * Contiguous handlers between the map-selection flow and the ASPSX 2.70
 * script dispatcher; the screen-position opcode touches no data, so -G8
 * leaves it unchanged. */
#include "pe1/scene_transition.h"
#include "pe1/player_entity.h"
#include "pe1/psyq_cd.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_record.h"
#include "common.h"
#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/game_audio_state.h"
#include "pe1/game_state_types.h"
#include "pe1/akao/commands.h"
#include "pe1/akao.h"
#include "pe1/asset_find.h"
#include "pe1/game_state.h"
#include "pe1/battle_level_up.h"
#include "pe1/battle_runtime.h"
#include "pe1/task_state.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/menu_dialog.h"
#include "pe1/cdrom.h"

/* COMMON supplies small-data metadata to MASPSX. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;
TaskNode *D_8009D300;


int Render_SetEntryPosition(int index, int x, int y);

int Task_SetActorScreenPos(int **arg0) {
    Render_SetEntryPosition(*arg0[0], *(short *)arg0[1], *(short *)arg0[2]);
    return 1;
}

int Task_OpenMemCardDialog(void) {
    Pe1GameState *state = &g_GameState;
    TaskNode *node;
    unsigned short *node_flags;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {
        node = g_TaskNodePool;
        node_flags = &node->flags;
        flags = *node_flags;
        if (!(flags & 0x20)) {
            *node_flags = flags | 0x20;
            cursor = g_SceneDataTable0;
            node->active = 1;
            cursor -= 8;
            g_SceneDataTable0 = cursor;
            return 0;
        }
        Render_BeginSceneLoad();
        Menu_OpenStartupMemCardDialog();
        {
            TaskNode *tail_node = g_TaskNodePool;
            int tail_flags;

            state->flags |= 0x9000;
            tail_flags = tail_node->flags;
            g_GameStateFlags |= 4;
            tail_flags &= 0xFFDF;
            tail_node->flags = tail_flags;
        }
    }
    return 1;
}

int Task_ShowReceivedItem(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    TaskNode *node;
    unsigned short *node_flags;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {

    node = g_TaskNodePool;
    node_flags = &node->flags;
    flags = *node_flags;
    if (!(flags & 0x20)) {
    *node_flags = flags | 0x20;
    cursor = g_SceneDataTable0;
    node->active = 1;
    cursor -= 0xC;
    g_SceneDataTable0 = cursor;
    return 0;

    }
    Render_BeginSceneLoad();
    Menu_CreateItemUsePanel(**saved);
    {
        TaskNode *tail_node = g_TaskNodePool;
        int tail_flags;

        state->flags |= 0x9000;
        tail_flags = tail_node->flags;
        g_GameStateFlags |= 4;
        tail_flags &= 0xFFDF;
        tail_node->flags = tail_flags;
    }

    }
    return 1;
}

/* Decoded script operands; the result is written back through its pointer. */
typedef struct TaskInvCommandArgs {
    int *command;
    int *value;
    int *other;
    int *result;
    int *unused;
} TaskInvCommandArgs;

int Task_RunInvCommand(TaskInvCommandArgs *args) {
    TaskNode *node;
    int flags;
    int result;

    if ((g_TaskNodePool->flags & 0x20) == 0) {
        *args->result = Inv_DispatchCommand(*args->command, *args->value, *args->other, args->unused);
    }

    if (MenuWidget_HasActiveNodes() != 0) {
        node = g_TaskNodePool;
        flags = node->flags;
        if ((flags & 0x20) == 0) {
            node->flags = flags | 0x20;
            g_GameState.flags |= 0x9000;
            Render_BeginSceneLoad();
        } else {
            g_GameStateFlags |= 4;
        }
        g_SceneDataTable0 -= 0x1C;
        g_TaskNodePool->active = 1;
        return 0;
    }

    result = D_8009D2A4;
    if (result != 0) {
        *args->result = result;
        g_TaskNodePool->flags &= 0xFFDF;
    }
    return 1;
}

/* The sound archive's second word locates a section whose asset-table
 * word packs the entry count (bits 22-31) and the table's offset from the
 * archive (bits 0-21). */
typedef struct AkaoArchiveHeader {
    u32 reserved;
    u32 section_offset;
} AkaoArchiveHeader;

typedef struct AkaoArchiveSection {
    u8 reserved[0x30];
    u32 asset_table;
} AkaoArchiveSection;

typedef struct AkaoCommandArgs {
    int *opcode;
    int *arg1;
    int *arg2;
    int *arg3;
    int *arg4;
    int *result;
} AkaoCommandArgs;

typedef struct AkaoAssetEntry {
    u8 pad0[3];
    u8 flags;
    u32 data_offset;
    u8 value8;
    u8 pad9;
    unsigned int id : 16;
} AkaoAssetEntry;

typedef struct AkaoStackLocals {
    int slot;
    u8 pad4[4];
    s8 volumes[4];
    u8 pad12[8];
} AkaoStackLocals;

u32 *D_8009CE00;

int Akao_Cmd_19_Then_C0(u8 *data, int value);

int Akao_ProcessCommand(AkaoCommandArgs *args) {
    AkaoStackLocals locals;
    int status;
    int handle;
    int wait_one;
    int slot_index0;
    int slot_index1;
    int slot_index2;
    int count;
    u32 entry_count;
    u32 entry_limit;
    int volume;
    u32 table_info;
    FieldActor *actor;
    u8 *base;
    FieldActor *player;
    AkaoAssetEntry *current;
    AkaoAssetEntry *entries;
    DeferredAkaoCommand *deferred;
    u8 *deferred_count;

    switch (*args->opcode) {
    case 0x64:
        Akao_Cmd_99_9B_9D(*args->arg1);
        break;
    case 0x65:
        Akao_Cmd_98_9A_9C(*args->arg1);
        break;
    case 0xC8:
    case 0x198:
        status = CD_SeekToTrack(*args->arg1, 1, 0, &locals.slot,
                                *args->arg2 == 0);
        if (status == 1) {
            D_8009CE00 -= 8;
            D_8009D300->active = status;
            return 0;
        }
        if (locals.slot + 2U < 2U) {
            break;
        }
        *args->result = Akao_Cmd_10(D_800B0CE8.voice_banks[locals.slot]);
        Akao_Cmd_C0_WithSlot(0, 0x7F);
        Overlay_RegisterAudioSlot(locals.slot, *args->arg1, *args->result, 0x7F);
        break;
    case 0xCB:
        status = CD_SeekToTrack(*args->arg1, 1, 1, &locals.slot,
                                *args->arg3 == 0);
        if (status == 1) {
            D_8009CE00 -= 8;
            D_8009D300->active = status;
            return 0;
        }
        if (locals.slot + 2U < 2U) {
            break;
        }
        *args->result = Akao_Cmd_19_Then_C0(
            D_800B0CE8.voice_banks[locals.slot], *args->arg2);
        Overlay_RegisterAudioSlot(locals.slot, *args->arg1, *args->result,
                                  *args->arg2);
        break;
    case 0xC9:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            break;
        }
        Akao_Cmd_11(locals.slot);
        wait_one = 1;
        status = CD_SeekToTrack(*args->arg1, 0, 0, &locals.slot, wait_one);
        if (status != wait_one) {
            break;
        }
        D_8009CE00 -= 8;
        D_8009D300->active = status;
        return 0;
    case 0xCC:
        Akao_Cmd_90(*args->arg1);
        break;
    case 0xCD:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            break;
        }
        Akao_Cmd_C0_WithSlot(locals.slot, *args->arg2);
        slot_index0 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index0, *args->arg1, handle,
                                  *args->arg2);
        break;
    case 0xCE:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            break;
        }
        Akao_Cmd_C1_WithSlot(locals.slot, *args->arg2 * 2, *args->arg3);
        slot_index1 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index1, *args->arg1, handle,
                                  *args->arg3);
        break;
    case 0xCF:
        locals.slot = Overlay_GetAudioSlotByKey(*args->arg1);
        if (locals.slot == -1) {
            break;
        }
        Akao_Cmd_C2_WithSlot(locals.slot, *args->arg2 * 2, *args->arg3,
                             *args->arg4);
        slot_index2 = Overlay_FindAudioSlotIndex(*args->arg1);
        handle = Overlay_GetAudioSlotByKey(*args->arg1);
        Overlay_RegisterAudioSlot(slot_index2, *args->arg1, handle,
                                  *args->arg4);
        break;
    case 0xD0:
        Akao_Cmd_D0(*args->arg1);
        break;
    case 0xD1:
        Akao_Cmd_D1(*args->arg1 * 2, *args->arg2);
        break;
    case 0xD2:
        Akao_Cmd_D2(*args->arg1 * 2, *args->arg2, *args->arg3);
        break;
    case 0xD3:
        Akao_Cmd_D4(*args->arg1);
        break;
    case 0xD4:
        Akao_Cmd_D5(*args->arg1 * 2, *args->arg2);
        break;
    case 0xD5:
        Akao_Cmd_D6(*args->arg1 * 2, *args->arg2, *args->arg3);
        break;
    case 0xD6:
        Akao_Cmd_D8(*args->arg1);
        break;
    case 0xD7:
        Akao_Cmd_D9(*args->arg1 * 2, *args->arg2);
        break;
    case 0xD8:
        Akao_Cmd_DA(*args->arg1 * 2, *args->arg2, *args->arg3);
        break;
    case 0xD9:
        Akao_ResetPositionalState();
        break;
    case 0x12C:
        *args->result = Akao_SendTableCommand(
            D_800B0CE8.archive, *args->arg1, *args->arg2, *args->arg3,
            *args->arg4);
        break;
    case 0x12D:
        Akao_Cmd_21(*args->arg1, *args->arg2);
        break;
    case 0x12E:
        Akao_Cmd_A0(*args->arg1, *args->arg2, *args->arg3);
        break;
    case 0x12F:
        Akao_Cmd_A1(*args->arg1, *args->arg2, *args->arg3 * 2,
                    *args->arg4);
        break;
    case 0x130:
        Akao_Cmd_A8(*args->arg1);
        break;
    case 0x131:
        Akao_Cmd_A9(*args->arg1 * 2, *args->arg2);
        break;
    case 0x132:
        Akao_Cmd_A2(*args->arg1, *args->arg2, *args->arg3);
        break;
    case 0x133:
        Akao_Cmd_A3(*args->arg1, *args->arg2, *args->arg3 * 8,
                    *args->arg4);
        break;
    case 0x134:
        Akao_Cmd_AA(*args->arg1);
        break;
    case 0x135:
        Akao_Cmd_AB(*args->arg1 * 2, *args->arg2);
        break;
    case 0x136:
        Akao_Cmd_A4(*args->arg1, *args->arg2, *args->arg3);
        break;
    case 0x137:
        Akao_Cmd_A5(*args->arg1, *args->arg2, *args->arg3 * 2,
                    *args->arg4);
        break;
    case 0x138:
        Akao_Cmd_AC(*args->arg1);
        break;
    case 0x139:
        Akao_Cmd_AD(*args->arg1 * 2, *args->arg2);
        break;
    case 0x13A:
        Akao_Cmd_F1();
        break;
    case 0x15E:
        D_800B0CE8.position_y = *args->arg1;
        D_800B0CE8.position_x = *args->arg2;
        D_800B0CE8.position_w = *args->arg3;
        D_800B0CE8.position_z = *args->arg4;
        break;
    case 0x15F:
        *args->result = Asset_Find08w(
            *args->arg1, 0, Pe1Fixed_Integer(&g_CurrentEntity->pos_x),
            Pe1Fixed_Integer(&g_CurrentEntity->pos_y),
            Pe1Fixed_Integer(&g_CurrentEntity->pos_z));
        break;
    case 0x160:
        if (*args->arg2 == 0) {
            player = g_PlayerEntity;
            if (player == 0) {
                break;
            }
            actor = player;
        } else {
            actor = D_8009D20C;
            while (actor != 0) {
                if (actor->type_id == *args->arg2 &&
                    actor->sub_id == *args->arg3 &&
                    !(actor->flags & 0x10)) {
                    break;
                }
                actor = actor->next;
            }
            if (actor == 0) {
                break;
            }
        }
        *args->result = Asset_Find08w(
            *args->arg1, 0, ((Pe1Fixed16_16 *)&actor->pos_x)->parts.integer,
            ((Pe1Fixed16_16 *)&actor->pos_y)->parts.integer,
            ((Pe1Fixed16_16 *)&actor->pos_z)->parts.integer);
        break;
    case 0x161:
        *args->result = Asset_Find08w(*args->arg1, 0,
                                     ((Pe1Fixed16_16 *)args->arg2)->parts.integer,
                                     ((Pe1Fixed16_16 *)args->arg3)->parts.integer,
                                     ((Pe1Fixed16_16 *)args->arg4)->parts.integer);
        break;
    case 0x190:
        base = D_800B0CE8.archive;
        table_info = ((AkaoArchiveSection *)(base +
            ((AkaoArchiveHeader *)D_800B0CE8.archive)->section_offset))->asset_table;
        base += table_info & 0x3FFFFF;
        count = 0;
        entry_count = table_info >> 22;
        if (entry_count == 0) {
            break;
        }
        entry_limit = entry_count;
        entries = (AkaoAssetEntry *)base;
        do {
            current = &entries[count];
            if (current->flags & 0x10) {
                int entry_id = current->id;
                if (entry_id == *args->arg1) {
                    goto archive_match;
                }
            }
        } while (++count < entry_limit);
        break;
    case 0x191:
        status = CD_SeekToTrack(*args->arg1, 1, 0, &locals.slot,
                               *args->arg2 == 0);
        if (status != 1) {
            *args->result = locals.slot;
            break;
    case 0x192:
        status = CD_SeekToTrack(*args->arg1, 1, 1, &locals.slot,
                               *args->arg2 == 0);
        if (status == 1) {
            D_8009CE00 -= 8;
            D_8009D300->active = status;
            return 0;
        }
        }
        else
        {
        D_8009CE00 -= 8;
        D_8009D300->active = status;
        return 0;
        }
        *args->result = locals.slot;
        break;
    case 0x195:
        D_800B0CE8.defer_enabled = 1;
        break;
    case 0x196:
    case 0x197:
        if (D_800B0CE8.deferred_count < 0x10) {
            deferred = &g_DeferredAkaoCommands[D_800B0CE8.deferred_count];
            deferred->actor_type = g_CurrentEntity->type_id;
            deferred->actor_id = g_CurrentEntity->sub_id;
            deferred->command = *args->arg1;
            deferred->arg1 = *args->arg2;
            deferred->arg2 = *args->arg3;
            if (*args->opcode == 0x196) {
                deferred->arg3 = *args->arg3;
            } else {
                deferred->arg3 = *args->arg4;
            }
            deferred_count = &D_800B0CE8.deferred_count;
            (*deferred_count)++;
        }
        break;
    case 0x199:
        volume = *args->arg1;
        locals.volumes[3] = 0;
        locals.volumes[1] = 0;
        D_800B0CE8.cd_volume = volume;
        locals.volumes[2] = volume;
        locals.volumes[0] = volume;
        DsMix((CdlATV *)locals.volumes);
        break;
archive_match:
        D_800B0CE8.selected_id = current->id;
        D_800B0CE8.selected_value = current->value8;
        D_800B0CE8.selected_data = D_800B0CE8.archive +
                                    (current->data_offset & 0xFFFFFF);
        break;
    case 0x19A:
        Akao_Cmd_92(*args->arg1);
        break;
    }
    return 1;
}
