#ifndef PSYQ_LIBPAD_PADPORTD_EXTERNS_H
#define PSYQ_LIBPAD_PADPORTD_EXTERNS_H

#include "common.h"
#include "pe1/card_obj.h"
#include "pe1/memcard_state.h"

extern CardObj D_800A5B70[];
extern u8 D_800A5AE0[];
extern u8 D_800A5B28[];
extern void (*D_8009B730)(CardObj *);
extern int (*D_8009B734)(CardObj *);
extern void (*D_8009B748)(CardObj *);
extern int D_8009B764;
extern int D_8009B768;
extern int g_MemCardPort2Present;
extern int D_8009B77C[];
extern MemCardSioRegisters *D_8009B7BC;
extern int D_8009B76C;

#endif /* PSYQ_LIBPAD_PADPORTD_EXTERNS_H */
