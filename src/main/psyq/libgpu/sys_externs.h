#ifndef PSYQ_LIBGPU_SYS_EXTERNS_H
#define PSYQ_LIBGPU_SYS_EXTERNS_H

extern unsigned char D_80095704[];
extern char D_800117E0[], D_80011800[];
extern unsigned short D_800957CC[][2], D_800957D8[][2];
extern GpuCallbacks *D_80095744;
extern GpuDebugPrintf D_80095748;
extern char D_80011814[];
extern char D_80011854[];
extern char D_80011870[];
extern GpuCallbacks *g_GpuCallbacks[];
extern GpuDebugPrintf g_GpuDebugPrintf[];
extern u8 g_GraphDebug[];
extern char D_80011884[];
extern char D_800118B8[], D_800118A4[], D_80011898[];
extern char D_800118BC[];
extern char D_800118C8[];
extern char D_800118D4[];
extern char D_800118E0[];
extern char D_800118EC[];
extern u32 D_800957EC[];
extern char D_800118F8[];
extern u32 D_800957F8;
extern u32 D_8009580C;
extern char D_80011910[];
extern char D_80011928[];
extern char D_8001193C[];
extern char D_80011954[];
extern void *memcpy(void *dest, const void *src, unsigned int n);

#endif /* PSYQ_LIBGPU_SYS_EXTERNS_H */
