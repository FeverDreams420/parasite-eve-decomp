#ifndef PE1_FIELD_MODEL_DRAW_H
#define PE1_FIELD_MODEL_DRAW_H

#include "common.h"
#include "pe1/gte_types.h"

/* Textured model packet helpers (engine/FieldEng_TexturedModel.c). The model
 * layout is FieldTexturedModel in pe1/field_textured_model.h. */
union FieldTexturedModel;

void GsSetOrign(int tpage, int clut);
/* Scrolls the model's texture coordinates by the given offsets. */
void func_800C6D5C(union FieldTexturedModel *model, u8 x_offset, u8 y_offset);
void func_800C6ED8(short value);
void func_800C6EF8(char *data);
void func_800C6F4C(char *data);
void func_800C6FA0(char *data, u16 factor);
void func_800C7098(char *data, u8 r, u8 g, u8 b);
/* Draws the model through placement. */
void func_800C71E4(union FieldTexturedModel *model, GteMatrix *placement);

#endif
