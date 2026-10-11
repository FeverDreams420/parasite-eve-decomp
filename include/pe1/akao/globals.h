#ifndef PE1_AKAO_GLOBALS_H
#define PE1_AKAO_GLOBALS_H

/* Driver-wide AKAO state shared by the sequencer, voice driver and
 * voice-bank units (src/main/akao). One declaration per word; the layout
 * headers (track.h, tick.h, queue.h) own the record types. */

#include "common.h"
#include "pe1/akao/track.h"

/* Sequencer bank selected by the commands (primary or secondary). */
extern AkaoSequencerBank *g_AkaoCurTrack;

/* Global slides stepped every fourth tick: counters, 16.16 steps and the
 * integer half of the pitch accumulator D_8009D2B4. */
extern u16 D_8009CDEC; /* tick divider */
extern short g_AkaoGlobalPitchSlideCounter;
extern short g_AkaoGlobalD2D0SlideCounter;
extern short g_AkaoGlobalD2CCSlideCounter;
extern u32 g_AkaoGlobalPitchSlideStep;
extern u32 g_AkaoGlobalD2D0SlideStep;
extern u32 g_AkaoGlobalD2CCSlideStep;
extern unsigned short D_8009D2B6;

/* Rolling timer-callback durations, newest first, and their sum. */
extern s32 g_AkaoTimerDeltaHist0;
extern s32 g_AkaoTimerDeltaHist1;
extern s32 g_AkaoTimerDeltaHist2;
extern s32 g_AkaoTimerDeltaHist3;
extern s32 g_AkaoTimerDeltaSum;

/* Argument byte counts of opcodes 0xA0-0xFF and of the 0xFC page. */
extern AkaoU8 g_AkaoOpcodeArgLengths[];
extern AkaoU8 g_AkaoExtOpcodeArgLengths[];

/* Pitch table of the instrument records, viewed as words. */
extern int g_AkaoPitchPeriodTable[];
extern void *g_AkaoSoundEntryTable[];

/* Voice masks. */
extern unsigned int g_SpuActiveVoiceMask;
extern unsigned int g_AkaoTrack34Mask;
extern unsigned int g_AkaoTrack38Mask;
extern unsigned int g_AkaoTrack3CMask;
extern unsigned int g_AkaoVoiceUpdateFlags;
extern int g_AkaoVoiceMaskScratch;
extern u16 g_AkaoTrack5ATransposeValue;
extern int g_AkaoSeqLoopCounter;

/* Nested-stream source words filled by SeqOp_LoadNestedStreams and passed
 * to Seq_StartNestedStreams as an AkaoNestedSource. */
extern u32 D_800B89D0;
extern u32 D_800B89D4;
extern u32 D_800B89D8;
extern u32 D_800B89DC;
extern u32 D_800B89E0;

#endif
