/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/akao.h"
#include "pe1/game_state.h"
#include "pe1/psyq_nop.h"
#include "pe1/scene_archive.h"
#include "pe1/boot.h"
#include "pe1/psyq_bios.h"
#include "pe1/akao/commands.h"

/* Matching debt: register pins and empty barriers retain the index,
* zero extension and callback state. The duplicated ready branch preserves
* GCC's block ordering. No ASM construct emits an instruction. */
int CD_ReadSectors(unsigned kind, unsigned int index, int channel, void *buffer, u32 maximum, int blocking)
{
    register Pe1GameState *state asm("$18") = &g_GameState;
    int result = -1;
    u32 *archiveLba;
    u32 transferValue;
    int busy = 0;
    unsigned int isSequence;
    register int finished asm("$16") = 0;
    register u32 scaledIndex asm("$5") = index * 2;
    u32 byteOffset;
    SceneSectorDirectory *root = (SceneSectorDirectory *)g_SceneSectorDirectory;
    u8 *entries;
    volatile u16 *entry;
    byteOffset = scaledIndex;
    entries = (u8 *)root->offsets;
    entry = (u16 *)(scaledIndex + entries);
    do
    {
        asm("" : "=r"(blocking), "=r"(kind) : "0"(blocking), "1"(kind));
        archiveLba = &((SceneSectorDirectory *)g_SceneSectorDirectory)->base_lba;
        switch (state->cd_read_phase)
        {
        case 0:
            {
                u8 *table = (u8 *)root;
                u32 first;
                register u32 start asm("$3");
                entries = table + byteOffset;
                first = *(u16 *)entry;
                asm("" : "=r"(first) : "0"(first), "r"(entries) : "memory");
                scaledIndex = *(volatile u16 *)(entries + 6);
                asm("" : "=r"(scaledIndex) : "0"(scaledIndex) : "memory");
                scaledIndex -= entry[0];
                transferValue = *archiveLba + first;
                start = state->pe_image_base_lba + transferValue;
                g_CdReadTotalSectors = scaledIndex;
                g_CdReadRemainingSectors = g_CdReadTotalSectors;
                isSequence = kind == 0;
                g_CdReadStartLba = start;
                if (isSequence)
                {
                    result = Spu_SetStreamModeA();
                    if (result == (-1))
                    {
                        busy = 1;
                        finished = (blocking ^ 1) & 1;
                    }
                }
                else
                if (kind == 3)
                {
                    result = Spu_SetStreamModeB();
                    if (result == (-1))
                    {
                        busy = 1;
                        finished = (blocking ^ 1) & 1;
                    }
                }
                transferValue = 7;
                state->cd_read_phase = transferValue;
                break;
            }

        case 7:
            if (g_CdReadRemainingSectors != 0)
            {
                u32 count = g_CdReadRemainingSectors;
                if (maximum < count)
                {
                    count = maximum;
                }
                g_CdReadChunkSectors = count;
                result = CdRom_ReadSectors(g_CdReadStartLba, g_CdReadTotalSectors - g_CdReadRemainingSectors, buffer, count);
                if (result != (-1))
                {
                    state->cd_read_phase = 8;
                }
                busy = 1;
                finished = (blocking ^ 1) & 1;
            }
            else
            {
                state->cd_read_phase = 0;
                busy = 0;
                finished = 1;
            }
            break;

        case 8:
            result = CdRom_PollReady();
            if (result == (-1))
            {
                transferValue = 7;
                state->cd_read_phase = transferValue;
            }
            else
            if (result == 0)
            {
                if (g_CdReadRemainingSectors)
                {
                    state->cd_read_phase = 9;
                    break;
                }
                else
                {
                    state->cd_read_phase = 9;
                    break;
                }
            }
            busy = 1;
            finished = (blocking ^ 1) & 1;
            break;

        case 9:
            switch (kind)
            {
            case 0:
                result = Akao_StepNoteSequencer(buffer, g_CdReadChunkSectors << 11);
                break;

            case 1:
                result = Spu_UploadSampleBlockBlocking(buffer, 0);
                break;

            case 2:
                result = Spu_UploadStreamBlockB(channel, buffer);
                break;

            case 3:
                result = Spu_UploadStreamBlockA(channel, buffer, g_CdReadChunkSectors << 11);
                break;

            }

            if (result == (-1))
            {
                state->cd_read_phase = 0;
                busy = 1;
                finished = (blocking ^ 1) & 1;
            }
            else
            {
                state->cd_read_phase = 10;
            }
            break;

        case 10:
            result = Spu_GetTransferStatus();
            if (result == -1) {
                state->cd_read_phase = 0;
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else if (result != 0) {
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else {
                u32 remaining, remainingSnapshot;
                u32 transferred;
                register u32 nextPhase asm("$3");
                busy = 1;
                remainingSnapshot = g_CdReadRemainingSectors;
                finished = (blocking ^ 1) & 1;
                asm("" : : "r"(busy), "r"(finished) : "memory");
                remaining = remainingSnapshot;
                transferred = g_CdReadChunkSectors;
                nextPhase = 7;
                state->cd_read_phase = nextPhase;
                g_CdReadRemainingSectors = remaining - transferred;
            }
            break;
        }
    } while (!finished);
    return busy;
}

int CD_FindNextDataSector(void)
{
    Pe1GameState *state = &g_GameState;
    u8 *base = (u8 *)state->loaded_scene_assets;
    CdArchiveHeader *header = (CdArchiveHeader *)(base + ((CdArchiveRoot *)base)->directoryOffset);
    CdRange *ranges = (CdRange *)(base + (header->range_info & 0x3fffff));

    /* Each state falls back to this dispatch until a read is pending (1)
     * or the archive's ranges are exhausted (0). */
dispatch:
    switch (state->cd_range_state) {
    case 0:
        g_CdRangeIndex = 0;
        state->cd_range_state = 0x28;
        break;
    case 0x28:
        if (CD_ReadSectors(1, 1, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
        if (state->cd_range_read_mode >= 2) {
            state->cd_range_state = 0x29;
            return 1;
        }
        state->cd_range_state = 0x2a;
        break;
    case 0x29:
        if (CD_ReadSectors(1, state->cd_range_read_mode, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
        state->cd_range_state = 0x2a;
        break;
    case 0x2a:
    {
        int index = g_CdRangeIndex;
        CdRange *entry;

        if (index < (int)(header->range_info >> 22)) {
            entry = (CdRange *)(index * sizeof(CdRange) + (u32)ranges);
            if (!(entry->flags & 0x10) || entry->first < 2) {
                g_CdRangeIndex = index + 1;
            } else {
                state->cd_range_state = 0x2b;
            }
        } else {
            state->cd_range_state = 0;
            return 0;
        }
        break;
    }
    case 0x2b:
    {
        CdRange *entry = (CdRange *)(g_CdRangeIndex * sizeof(CdRange) + (u32)ranges);
        if (CD_ReadSectors(3, entry->first, entry->last, state->scene_load_scratch, 0x21, 0) == 1) return 1;
        state->cd_range_state = 0x2a;
        g_CdRangeIndex = g_CdRangeIndex + 1;
        break;
    }
    default:
        return 0;
    }
    goto dispatch;
}


void Akao_ResetPositionalState(void) {
    g_GameState.scene_audio.tracks.keys[0][1] = -1;
    g_GameState.scene_audio.tracks.keys[0][0] = -1;
    g_GameState.scene_audio.tracks.keys[1][1] = -1;
    g_GameState.scene_audio.tracks.keys[1][0] = -1;
    g_GameState.scene_audio.tracks.banks[1] = -1;
    g_GameState.scene_audio.tracks.banks[0] = -1;
    g_GameState.flags &= -0xF1;
    Akao_Cmd_F0();
}


/* Matching debt: finished is pinned to s1. The empty count barrier preserves
 * the signed loop test and its scheduling; volatile directory reads retain
 * the original pair of loads. The shifted state view retains the workspace
 * selection made before callbacks. Neither ASM construct emits instructions. */
int CD_SeekToTrack(int key, int mode, int group, int *slot, int blocking)
{
    int busy = 0;
    register int finished asm("$17") = 0;
    Pe1GameState *state = &g_GameState;
    int bankIndex = group != 0;
    Pe1GameState *selected = (Pe1GameState *)((u8 *)state + bankIndex * 4);
    volatile SceneAssetDirectory *directory = SceneAsset_ResolveOffset(
        *D_800B0E64, (*D_800B0E64)->directoryOffset);
    do {
        switch (state->cd_track_phase) {
        case 0: {
            SceneTrackRecord *record;
            int count;
            int i;
            g_CdTrackBank = -1;
            record = SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                             directory->trackEntries & 0x3FFFFF);
            count = directory->trackEntries >> 22;
            g_CdTrackRecord = record;
            asm volatile("" : "=r"(count) : "0"(count));
            for (i = 0; i < count; i++, record++) {
                if (record->key == key) {
                    g_CdTrackBank = record->bank;
                    g_CdTrackRecord = record;
                    break;
                }
            }
            if (g_CdTrackBank == -1) {
                for (i = 0; i < 2; i++) {
                    if (state->scene_audio.tracks.keys[i][0] == key) {
                        g_CdTrackBank = (s8)state->scene_audio.tracks.banks[i];
                        g_CdTrackRecord = 0;
                        break;
                    }
                }
                if (g_CdTrackBank == -1) {
                    *slot = -1;
                    busy = 0;
                    finished = 1;
                    break;
                }
            }
            if (mode) {
                if (state->scene_audio.tracks.keys[0][0] == key) {
                    if (state->flags & 0x40)
                        *slot = -2;
                    else
                        *slot = 0;
                    busy = 0;
                    finished = 1;
                } else if (state->scene_audio.tracks.keys[1][0] == key) {
                    if (state->flags & 0x80)
                        *slot = -2;
                    else
                        *slot = 1;
                    busy = 0;
                    finished = 1;
                } else {
                    if (!group)
                        Akao_Cmd_F0();
                    state->cd_track_phase = 7;
                }
            } else {
                if (state->scene_audio.tracks.keys[0][0] == key) {
                    state->scene_audio.tracks.keys[0][0] = -1;
                    ((s8 *)state->scene_audio.tracks.banks)[0] = -1;
                    state->flags &= ~0x40;
                    *slot = 0;
                }
                if (state->scene_audio.tracks.keys[1][0] == key) {
                    state->scene_audio.tracks.keys[1][0] = -1;
                    ((s8 *)state->scene_audio.tracks.banks)[1] = -1;
                    state->flags &= ~0x80;
                    *slot = 1;
                }
                busy = 0;
                finished = 1;
            }
            break;
        }
        case 7:
            if (CD_ReadSectors(0, g_CdTrackBank, 0, state->scene_load_scratch, 0x21, blocking) == 1) {
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else {
                state->cd_track_phase = 9;
            }
            break;
        case 9:
            *slot = bankIndex;
            if (g_CdTrackRecord) {
                memcpy((void *)selected->bank_work_base,
                       SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                                g_CdTrackRecord->offset & 0xFFFFFF),
                       g_CdTrackRecord->size & 0xFFFFFF);
            }
            busy = 0;
            finished = 1;
            state->scene_audio.tracks.keys[*slot][0] = key;
            state->scene_audio.tracks.banks[*slot] = g_CdTrackBank;
            state->cd_track_phase = 0;
            break;
        }
    } while (!finished);
    return busy;
}


/* Matching debt: one argument pin, two individually
 * wrapped NOPs, three table aliases and the retry jump. The aliases preserve
 * independent absolute reads of the table pointer around callbacks. */
int CD_StepReadState(int active)
{
    Pe1GameState *state = &g_GameState;
    int slot;
    int voice;
    u32 flags;
    u32 changing;
retry:
    {
        switch (state->cd_transition_phase) {
        case 0:
            g_CdStreamBankIndex = 0;
            Akao_Cmd_F1();
            if (active) {
                if (!(g_GameState.flags & 0x400000)) {
                    if (*tableCheck) {
                        Akao_SendTableCommand(*D_800B0E08, 0x451, 0, 0x80, 0x7F);
                        if (*tableCheckAgain)
                            Akao_SendTableCommand(*tableAgain, 0x452, 0, 0x80, 0x7F);
                    }
                }
                state->cd_transition_phase = 44;
            } else {
                if (state->flags & 4) {
                    register int zero asm("$4") = 0;
                    int duration = 60;
                    int now = *D_8009CDA4;
                    PE1_NOP_IO2_DEP(zero, duration, now);
                    g_CdFadeStartTime = now;
                    Akao_Cmd_C1_WithSlot(zero, duration, 0);
                }
                state->cd_transition_phase = 45;
            }
            break;
        case 44:
            if (state->scene_audio.tracks.pending_key != state->scene_audio.tracks.keys[0][0]) {
                if (state->flags & 0x40) {
                    int zero = 0;
                    int duration = 60;
                    int now = *D_8009CDA4;
                    PE1_NOP_IO2_DEP(zero, duration, now);
                    g_CdFadeStartTime = now;
                    Akao_Cmd_C1_WithSlot(zero, duration, 0);
                }
                state->flags |= 4;
            }
            state->cd_transition_phase = 46;
            break;
        case 46:
            if (CD_FindNextDataSector() == 1)
                return 1;
            flags = state->flags;
            changing = flags & 4;
            if (changing) {
                if (flags & 0x40) {
                    g_CdFadeFramesLeft = 60 - (*D_8009CDA4 - g_CdFadeStartTime);
                    if (g_CdFadeFramesLeft > 8) {
                        g_CdFadeFramesLeft = 8;
                        Akao_Cmd_C1_WithSlot(0, 16, 0);
                    } else if (g_CdFadeFramesLeft < 0) {
                        g_CdFadeFramesLeft = 0;
                    }
                }
            }
            state->cd_transition_phase = 63;
            break;
        case 63:
            if (state->flags & 4) {
                if (state->flags & 0x40) {
                    if (g_CdFadeFramesLeft > 0) {
                        --g_CdFadeFramesLeft;
                        return 1;
                    }
                    Akao_Cmd_40();
                    Akao_Cmd_F0();
                }
                state->cd_transition_phase = 47;
            } else {
                state->cd_transition_phase = 48;
            }
            break;
        case 47:
            if (CD_ReadSectors(0, state->scene_audio.tracks.pending_bank, 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            state->cd_transition_phase = 48;
            break;
        case 48:
            Akao_Cmd_10(state->bank_asset_table);
            Akao_Cmd_C0_WithSlot(0, 0x7F);
            state->cd_transition_phase = 0;
            return 0;
        case 45:
            if (g_CdStreamBankIndex < 2) {
                if (state->pending_stream_banks[g_CdStreamBankIndex]) {
                    state->cd_transition_phase = 49;
                    break;
                }
                ++g_CdStreamBankIndex;
            }
            state->cd_transition_phase = 50;
            break;
        case 49:
            if (CD_ReadSectors(2, state->pending_stream_banks[g_CdStreamBankIndex], g_CdStreamBankIndex,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            ++g_CdStreamBankIndex;
            state->cd_transition_phase = 45;
            break;
        case 50:
            if (state->pending_sample_bank != -1 &&
                CD_ReadSectors(1, state->pending_sample_bank, 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            if (state->flags & 4) {
                g_CdFadeFramesLeft = 60 - (*D_8009CDA4 - g_CdFadeStartTime);
                if (g_CdFadeFramesLeft > 8) {
                    g_CdFadeFramesLeft = 8;
                    Akao_Cmd_C1_WithSlot(0, 16, 0);
                } else if (g_CdFadeFramesLeft < 0) {
                    g_CdFadeFramesLeft = 0;
                }
            }
            state->cd_transition_phase = 64;
            break;
        case 64:
            if (state->flags & 4) {
                if (g_CdFadeFramesLeft > 0) {
                    --g_CdFadeFramesLeft;
                    return 1;
                }
                Akao_Cmd_F0();
                if (state->flags & 0x40) {
                    state->flags &= ~0x40;
                    state->cd_transition_phase = 62;
                    break;
                }
            }
            state->cd_transition_phase = 0;
            state->flags &= ~4;
            return 0;
        case 62:
            if (CD_ReadSectors(0, ((s8 *)state->scene_audio.tracks.banks)[0], 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            state->cd_transition_phase = 51;
            break;
        case 51:
            if (CD_SeekToTrack(state->scene_audio.tracks.keys[0][0], 1, 0, &slot, 0) == 1)
                return 1;
            voice = Akao_Cmd_10((void *)state->bank_work_base);
            if (voice == -1) {
                state->cd_transition_phase = 62;
                return 1;
            }
            Akao_Cmd_C1_WithSlot(voice, 60, state->track_volumes[0]);
            if (voice)
                Overlay_RegisterAudioSlot(0, state->scene_audio.tracks.keys[0][0], voice,
                                          state->track_volumes[0]);
            state->cd_transition_phase = 0;
            state->flags = (state->flags & ~4) | 0x40;
            return 0;
        default:
            return 0;
        }
        goto retry;
    }
}
