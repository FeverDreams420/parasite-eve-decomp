#ifndef PSYQ_LIBPAD_PADIF_EXTERNS_H
#define PSYQ_LIBPAD_PADIF_EXTERNS_H

#include "common.h"
#include "pe1/card_obj.h"

extern int g_MemCardDispatchResult;
extern void (*D_8009B750)(void);
extern void (*D_8009B754)(void);
extern int D_8009B760;
extern int D_8009B764;
extern int g_MemCardPort1Present;
extern int g_MemCardWriteBlockCount;
extern void (*D_8009B730)(CardObj *);
extern void (*D_8009B748)(CardObj *);
extern int g_MemCardCommandByte, D_8009B764, g_MemCardWriteBlockCount, D_8009B77C[];
extern volatile u8 *D_8009B7A0;

#endif /* PSYQ_LIBPAD_PADIF_EXTERNS_H */
