#ifndef PE1_EXT_380AAEC66ADE
#define PE1_EXT_380AAEC66ADE

#include "common.h"
#include "pe1/card_obj.h"

/* LIBPAD hook slots that _padInitDirSeq points at this object's routines. */
extern void (*g_MemCardResponseHandler)(CardObj *port);
#endif /* PE1_EXT_380AAEC66ADE */
