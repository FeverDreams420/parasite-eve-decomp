/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/akao.h"
#include "pe1/game_state.h"
#include "pe1/psyq_nop.h"
#include "pe1/scene_archive.h"

#include "CD_TrackReadFlow_externs.h"
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
    SceneSectorDirectory *root = (SceneSectorDirectory *)D_8009317C;
    u8 *entries;
    volatile u16 *entry;
    byteOffset = scaledIndex;
    entries = (u8 *)root->offsets;
    entry = (u16 *)(scaledIndex + entries);
    do
    {
        asm("" : "=r"(blocking), "=r"(kind) : "0"(blocking), "1"(kind));
        archiveLba = &((SceneSectorDirectory *)D_8009317C)->base_lba;
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
                D_8009D174 = scaledIndex;
                D_8009D178 = D_8009D174;
                isSequence = kind == 0;
                D_8009D170 = start;
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
            if (D_8009D178 != 0)
            {
                u32 count = D_8009D178;
                if (maximum < count)
                {
                    count = maximum;
                }
                D_8009D17C = count;
                result = CdRom_ReadSectors(D_8009D170, D_8009D174 - D_8009D178, buffer, count);
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
                if (D_8009D178)
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
                result = Akao_StepNoteSequencer(buffer, D_8009D17C << 11);
                break;

            case 1:
                result = Spu_UploadSampleBlockBlocking(buffer, 0);
                break;

            case 2:
                result = Spu_UploadStreamBlockB(channel, buffer);
                break;

            case 3:
                result = Spu_UploadStreamBlockA(channel, buffer, D_8009D17C << 11);
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
                remainingSnapshot = D_8009D178;
                finished = (blocking ^ 1) & 1;
                asm("" : : "r"(busy), "r"(finished) : "memory");
                remaining = remainingSnapshot;
                transferred = D_8009D17C;
                nextPhase = 7;
                state->cd_read_phase = nextPhase;
                D_8009D178 = remaining - transferred;
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

dispatch:
    switch (state->cd_range_state) {
        case 0:
            D_8009CDCC = 0;
            state->cd_range_state = 0x28;
            goto dispatch;
        case 0x28:
        {
            if (CD_ReadSectors(1, 1, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
            if (state->cd_range_read_mode >= 2) {
                state->cd_range_state = 0x29;
                return 1;
            }
            state->cd_range_state = 0x2a;
            goto dispatch;
        }
        case 0x29:
        {
            if (CD_ReadSectors(1, state->cd_range_read_mode, 0, state->scene_load_scratch, 0x21, 0) == 1) return 1;
            state->cd_range_state = 0x2a;
            goto dispatch;
        }
        case 0x2a:
        {
            int index;
            CdRange *entry;
            int byte_offset;
            index = D_8009CDCC;
            if (index < (int)(header->range_info >> 22)) {
                byte_offset = index * sizeof(CdRange);
                entry = (CdRange *)(byte_offset + (u32)ranges);
                if (entry->flags & 0x10) {
                    if (entry->first >= 2) goto read_range;
                }
                D_8009CDCC = index + 1;
                goto dispatch;
read_range:
                state->cd_range_state = 0x2b;
                goto dispatch;
            } else {
                state->cd_range_state = 0;
                return 0;
            }
        }
        case 0x2b:
        {
            CdRange *entry = (CdRange *)(D_8009CDCC * sizeof(CdRange) + (u32)ranges);
            if (CD_ReadSectors(3, entry->first, entry->last, state->scene_load_scratch, 0x21, 0) == 1) return 1;
            {
                state->cd_range_state = 0x2a;
                D_8009CDCC = D_8009CDCC + 1;
            }
            goto dispatch;
        }
        default:
            return 0;
    }
}

void Akao_Cmd_F0(void);

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

#include "CD_TrackReadFlow_externs_1.h"
void *memcpy(void *, const void *, unsigned);

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
            D_8009D184 = -1;
            record = SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                             directory->trackEntries & 0x3FFFFF);
            count = directory->trackEntries >> 22;
            D_8009D180 = record;
            asm volatile("" : "=r"(count) : "0"(count));
            for (i = 0; i < count; i++, record++) {
                if (record->key == key) {
                    D_8009D184 = record->bank;
                    D_8009D180 = record;
                    break;
                }
            }
            if (D_8009D184 == -1) {
                for (i = 0; i < 2; i++) {
                    if (state->scene_audio.tracks.keys[i][0] == key) {
                        D_8009D184 = (s8)state->scene_audio.tracks.banks[i];
                        D_8009D180 = 0;
                        break;
                    }
                }
                if (D_8009D184 == -1) {
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
            if (CD_ReadSectors(0, D_8009D184, 0, state->scene_load_scratch, 0x21, blocking) == 1) {
                busy = 1;
                finished = (blocking ^ 1) & 1;
            } else {
                state->cd_track_phase = 9;
            }
            break;
        case 9:
            *slot = bankIndex;
            if (D_8009D180) {
                memcpy((void *)selected->bank_work_base,
                       SceneAsset_ResolveOffset(state->loaded_scene_assets,
                                                D_8009D180->offset & 0xFFFFFF),
                       D_8009D180->size & 0xFFFFFF);
            }
            busy = 0;
            finished = 1;
            state->scene_audio.tracks.keys[*slot][0] = key;
            state->scene_audio.tracks.banks[*slot] = D_8009D184;
            state->cd_track_phase = 0;
            break;
        }
    } while (!finished);
    return busy;
}

#include "CD_TrackReadFlow_externs_2.h"
void Akao_Cmd_F1(void);
void Akao_Cmd_40(void);
void Akao_Cmd_C1_WithSlot(int, int, int);
void Akao_Cmd_C0_WithSlot(int, int);
void Overlay_RegisterAudioSlot(int, int, int, int);

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
            D_8009D188 = 0;
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
                    D_8009D18C = now;
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
                    D_8009D18C = now;
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
                    D_8009D190 = 60 - (*D_8009CDA4 - D_8009D18C);
                    if (D_8009D190 > 8) {
                        D_8009D190 = 8;
                        Akao_Cmd_C1_WithSlot(0, 16, 0);
                    } else if (D_8009D190 < 0) {
                        D_8009D190 = 0;
                    }
                }
            }
            state->cd_transition_phase = 63;
            break;
        case 63:
            if (state->flags & 4) {
                if (state->flags & 0x40) {
                    if (D_8009D190 > 0) {
                        --D_8009D190;
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
            if (D_8009D188 < 2) {
                if (state->pending_stream_banks[D_8009D188]) {
                    state->cd_transition_phase = 49;
                    break;
                }
                ++D_8009D188;
            }
            state->cd_transition_phase = 50;
            break;
        case 49:
            if (CD_ReadSectors(2, state->pending_stream_banks[D_8009D188], D_8009D188,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            ++D_8009D188;
            state->cd_transition_phase = 45;
            break;
        case 50:
            if (state->pending_sample_bank != -1 &&
                CD_ReadSectors(1, state->pending_sample_bank, 0,
                               state->scene_load_scratch, 0x21, 0) == 1)
                return 1;
            if (state->flags & 4) {
                D_8009D190 = 60 - (*D_8009CDA4 - D_8009D18C);
                if (D_8009D190 > 8) {
                    D_8009D190 = 8;
                    Akao_Cmd_C1_WithSlot(0, 16, 0);
                } else if (D_8009D190 < 0) {
                    D_8009D190 = 0;
                }
            }
            state->cd_transition_phase = 64;
            break;
        case 64:
            if (state->flags & 4) {
                if (D_8009D190 > 0) {
                    --D_8009D190;
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
            Akao_Cmd_C1_WithSlot(voice, 60, state->transition_volume);
            if (voice)
                Overlay_RegisterAudioSlot(0, state->scene_audio.tracks.keys[0][0], voice,
                                          state->transition_volume);
            state->cd_transition_phase = 0;
            state->flags = (state->flags & ~4) | 0x40;
            return 0;
        default:
            return 0;
        }
        goto retry;
    }
}
