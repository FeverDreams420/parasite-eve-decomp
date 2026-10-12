#ifndef PE1_CDROM_H
#define PE1_CDROM_H

#include "common.h"

struct CdlLOC;

int DsControl(unsigned char command, void *param, void *result);
int DsControlB(unsigned char command, void *param, void *result);
int DsSystemStatus(void);
int DsShellOpen(void);
int DsQueueLen(void);
void DS_restart(void);
int DS_system_active(void);
int DsReset(void);
int ER_retry(void);
int DS_lastcom(void);
int DS_lastmode(void);
struct CdlLOC *DS_lastpos(void);
struct CdlLOC *DsLastPos(struct CdlLOC *destination);
int CdRom_PollReady(void);
int DsReadSync(void *argument);
int DsReady(u8 *destination);
int CdRom_ReadSectors(u32 lba, u32 offset, void *destination, u32 size);
int CdRom_ReadSectorsFromLba(u32 lba, void *destination, u32 size);
int DsRead(struct CdlLOC *position, int sectors, void *destination,
                    int mode);

/* Disc type the shell must still report before a read is issued; a
 * different value means the lid was opened and another disc inserted. */
extern u16 g_CdDiskType;
/* "CD: Read error (%d / %d)\n" */
extern char g_CdReadErrorFormat[];

/* CD_ReadSectors state: PE.IMG start sector of the current read, its total
 * and remaining sector counts, and the sectors requested by the last chunk. */
extern u32 g_CdReadStartLba;
extern u32 g_CdReadTotalSectors;
extern u32 g_CdReadRemainingSectors;
extern u32 g_CdReadChunkSectors;
/* Per-kind sector ranges (SceneSectorDirectory: base sector + u16 offsets). */
extern u8 g_SceneSectorDirectory[];

/* CD_SeekToTrack result: the scene track record and its bank (-1: none). */
struct SceneTrackRecord;
extern struct SceneTrackRecord *g_CdTrackRecord;
extern s16 g_CdTrackBank;

/* CD_StepReadState: stream bank being loaded, and the fade-out started at
 * g_CdFadeStartTime with g_CdFadeFramesLeft frames still to wait. */
extern int g_CdStreamBankIndex;
extern int g_CdFadeStartTime;
extern int g_CdFadeFramesLeft;

/* Sound table pointer the transition checks before sending its commands.
 * The three extra views keep retail's separate absolute loads of the same
 * word: with one symbol CSE merges them (docs/CODE_ORGANIZATION.md). */
extern void *D_800B0E08[];
extern void *tableCheck[] asm("D_800B0E08");
extern void *tableCheckAgain[] asm("D_800B0E08");
extern void *tableAgain[] asm("D_800B0E08");

void Akao_ResetPositionalState(void);
int CD_SeekToTrack(int key, int mode, int group, int *slot, int immediate);

#endif
