#ifndef PE1_FIELD_MODEL_DRAW_H
#define PE1_FIELD_MODEL_DRAW_H

#include "common.h"
#include "pe1/gte_types.h"

/* Textured model packet helpers (fieldfx/FieldEng_TexturedModel.c). The model
 * layout is FieldTexturedModel in pe1/field_textured_model.h. */
union FieldTexturedModel;

void GsSetOrign(int tpage, int clut);
/* Scrolls the model's texture coordinates by the given offsets. */
void FieldModel_ScrollUv(union FieldTexturedModel *model, u8 x_offset, u8 y_offset);
void FieldModel_SetSemiTrans(short value);
void FieldModel_SaveColors(char *data);
void FieldModel_RestoreColors(char *data);
void FieldModel_ScaleColors(char *data, u16 factor);
void FieldModel_SetColors(char *data, u8 r, u8 g, u8 b);
/* Draws the model through placement. */
void FieldModel_Draw(union FieldTexturedModel *model, GteMatrix *placement);

#endif
