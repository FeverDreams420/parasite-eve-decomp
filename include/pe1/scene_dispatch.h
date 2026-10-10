#ifndef PE1_SCENE_DISPATCH_H
#define PE1_SCENE_DISPATCH_H

/* Dispatch token of the scene that is running; the main loop sets it and the
 * map selection compares it with g_SceneDispatchToken. */
extern int g_SceneDispatchCur;

/* Mirror of the current room id (D_8009D280); see scene_transition.h. */
extern int D_8009D1C4;

#endif
