#ifndef PE1_EXT_D0CA6D1E1EB4
#define PE1_EXT_D0CA6D1E1EB4
extern void InitSystem(void);
extern void Sys_SyncShutdown(void);
extern int OpenPeImage(void);
extern void Boot_InitGameState(void);
extern void Boot_InitSubsystems(void);
extern void Scene_LoadSceneData(void);
extern void Overlay_LoadTables(void);
extern void func_8019234C(void);
extern void CD_LoadBootAudio(void);
extern void func_801235DC(void);
extern int Overlay_LoadInitialImage(void);
extern int func_801909B4(void);
extern void Gpu_InitDisplay(int arg0);
extern void Boot_RunFrame(void);
extern void SetDispMask(int arg0);
extern int g_SceneDispatchToken;
#endif /* PE1_EXT_D0CA6D1E1EB4 */
