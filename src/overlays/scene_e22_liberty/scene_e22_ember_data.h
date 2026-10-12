#ifndef SCENE_E22_EMBER_DATA_H
#define SCENE_E22_EMBER_DATA_H

#include "pe1/scene_e22_ember_burst.h"

/* scene_e22's own templates, colours, particle tables and particle
 * callbacks for the ember, link beam, swirl ring, ember column, twist model
 * and sweep bolt controllers. scene_e19 reuses the controller layouts from
 * pe1/scene_e22_ember_burst.h with its own data at these addresses. */
extern RenderColor D_8018F208;
extern RenderColor D_8018F20C;
extern RenderColor D_8018F204;
extern u8 D_80199388[];
extern int func_80193940(int mode, RoomOrbitTrailParticle *p);
extern SceneE22LinkJoints D_801991B8[];
extern GteShortVector D_8018F1D8;
extern GteShortVector D_8018F1E0;
extern RenderColor D_8018F1E8;
extern RenderColor D_8018F1EC;
extern RenderColor D_8018F1F0;
extern int func_801931B8(int mode, RoomOrbitTrailParticle *p);
extern GteRotation D_8018F1F4;
extern RenderColor D_8018F214;
extern u8 D_80199434[];
extern int func_801957CC(int mode, RoomOrbitTrailParticle *p);
extern GteRotation D_8018F1FC;
extern u8 D_80199308[];
extern u8 D_80199190[];
extern RenderColor D_8018F210;
extern int func_8019485C(int mode, RoomOrbitTrailParticle *p);
extern void *D_80199500;
extern RenderColor D_8018F218;
extern int func_801962FC(int mode, RoomOrbitTrailParticle *p);
extern RenderColor D_8018F224;
extern s16 D_80199504;
extern int func_8019702C(int mode, RoomOrbitTrailParticle *p);

#endif /* SCENE_E22_EMBER_DATA_H */
