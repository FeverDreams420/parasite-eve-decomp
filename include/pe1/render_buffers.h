#ifndef PE1_RENDER_BUFFERS_H
#define PE1_RENDER_BUFFERS_H

/* Work-buffer bases carved out by Gpu_InitDisplay. The -G8 menu units read
 * them with absolute addresses, so they are incomplete arrays used at
 * element 0. */
extern int g_RenderOtBufferBaseAlt[];
extern int g_RenderScratchBufferBase[];
extern int g_RenderFrontBufferBase[];
extern int g_RenderBackBufferBase[];
extern int g_LoadedSceneAssetBlock[];
extern int g_SceneLoadScratchBuffer[];
/* Double-buffer slot (0 or 1) selecting the per-slot tables below and the
 * display/draw environments. */
extern int g_ActiveDrawSlot;
/* Per-slot ordering tables and animation buffers. */
extern unsigned char *g_OtBufferTable[];
extern unsigned char *g_RenderAnimBufferBase[];

#endif
