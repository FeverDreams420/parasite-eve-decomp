#ifndef PE1_AKAO_H
#define PE1_AKAO_H

#include "pe1/akao/track.h"
#include "pe1/akao/queue.h"
#include "pe1/akao/commands.h"
#include "pe1/akao/spu_voice.h"
#include "pe1/akao_send.h"

/*
 * Track structs are still intentionally partial in C files. The FF7/FF8 AKAO
 * cross-reference has stable field roles, but PE1 offsets do not fully match
 * either game yet.
 */

/* SPU sample transfers (Spu_TransferAndLifecycle.c). The transfer status is
 * 1 while a DMA transfer runs; the completion callback clears it. */
extern volatile int g_SpuTransferStatus;
void Spu_ClearTransferCallback(void);
void Spu_PrepareTransfer(void);
void Spu_UploadWithPrepare(void *source, unsigned int size);
void Spu_ReadWithPrepare(void *destination, unsigned int size);
void Spu_WaitTransferDone(void);
int Spu_UploadSampleBlock(int *header, int wait);
void Spu_InitHardware(void);
void Spu_SetSampleTableBase(unsigned short *table);
void Spu_Shutdown(void);

/* Stream instrument relocation. */
void Spu_RebaseStreamAddrs(unsigned char *instruments, int address, int count);
int Spu_UploadStreamBlockA(unsigned bank, unsigned char *cursor, unsigned size);
int Spu_UploadStreamBlockB(unsigned bank, unsigned char *cursor);
int Akao_StepNoteSequencer(unsigned char *cursor, unsigned size);
int Spu_SetStreamModeA(void);
int Spu_SetStreamModeB(void);
int Spu_GetTransferStatus(void);
/* Stream instrument scratch table; bank selection advances by 0x400 bytes. */
extern AkaoInstrument D_800B4900[];
extern AkaoInstrument D_800B4D00[];
/* An instrument cache slot as the word retail adds a byte offset to; the
 * relocated records are copied into it word by word. */
typedef union AkaoInstrumentCacheAddress {
    AkaoInstrument *instrument;
    unsigned *words;
    unsigned word;
} AkaoInstrumentCacheAddress;
extern AkaoInstrument D_800B2900[];
/* Instrument records replaced by Spu_UploadSampleBlock. */
extern AkaoInstrument g_AkaoInstrumentTable[];
extern unsigned D_8009D270, D_8009D1EC, D_8009D204, D_8009D2BC, D_8009D2E4;

extern void *g_AkaoBgmHandle[];

int Spu_UploadSampleBlockBlocking(void *block, int mode);
int Akao_SendPositionalCmdStereo(int arg0, int arg1, short x, short y, short z);

#endif
