#ifndef PE1_AKAO_INIT_STATE_H
#define PE1_AKAO_INIT_STATE_H

#include "common.h"
#include "pe1/akao/queue.h"
#include "pe1/akao/spu_common.h"
#include "pe1/akao/seq_param.h"
#include "pe1/akao/track.h"
#include "pe1/akao/tick.h"
#include "pe1/akao/voice_masks.h"
#include "pe1/akao_counters.h"

/* Primary (music) and secondary (sound effect) sequencer banks;
 * g_AkaoCurTrack selects one of them. */
extern AkaoSequencerState g_AkaoSequencerState;
extern SpuCommonAttr g_AkaoSpuCommonAttr;

extern int g_AkaoPlaybackMode;
extern short g_AkaoGlobalD2CCSlideCounter;
extern short g_AkaoGlobalD2D0SlideCounter;
extern short g_AkaoGlobalPitchSlideCounter;
extern int g_AkaoSeqLoopCounter;
extern unsigned int g_AkaoSeqPendingFlags;
extern int D_8009D2E0;
extern u32 D_800BCD54;
extern u32 D_800BCD60;
extern u32 D_800BCD64;

/* Silent ADPCM samples: a looping block uploaded to SPU address 0x1010 at
 * start-up, and end blocks uploaded at shutdown over a running transfer. */
extern unsigned char g_AkaoSilentLoopSample[0x20];
extern unsigned char g_AkaoSilentEndSample[0x40];
/* SpuInitMalloc table for four blocks. */
extern char g_AkaoSpuMallocTable[];
/* Root counter 2 event that drives Akao_TimerCallback. */
extern int g_AkaoTimerEventDesc;

void Spu_InitVoiceState(void);

#endif
