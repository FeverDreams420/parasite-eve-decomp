#ifndef PE1_SCENE_E20_FLARE_DRAW_H
#define PE1_SCENE_E20_FLARE_DRAW_H

#include "pe1/scene_e20_flare.h"
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/gte_window.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"

/* Declarations used only by the scene e20 flare particle callback. */
extern RenderColor D_8018EFF4;
extern RenderColor D_8018EFF8;
extern u8 D_80190804[];
/* Texture page indices: [0] is the shared effect page, [8] this scene's. */
extern u16 D_800E11EA[];

#endif
