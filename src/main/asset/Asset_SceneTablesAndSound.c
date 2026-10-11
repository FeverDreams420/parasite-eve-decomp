/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/scene_assets.h"
#include "pe1/akao/commands.h"
#include "pe1/game_audio_state.h"
#include "pe1/akao/pan3d.h"
#include "pe1/game_state.h"
#include "pe1/gte.h"
#include "pe1/entity_frame_update.h"
#include "pe1/render_object.h"
#include "pe1/asset_key_record.h"
#include "pe1/psyq_tim.h"
#include "pe1/render_buffers.h"
#include "pe1/akao.h"

int Asset_SearchByKeyType(int arg0)
{
  u32 key;
  u32 mask;
  int wanted;
  register void *base;
  u32 type_mask;
  u32 high_mask;
  u32 high_value;
  u8 *entry;
  u32 value;
  base = (void *)g_LoadedSceneAssetBlock[0];
  wanted = arg0;
  key = 0x73DECD80;
  mask = 0x1FFC00;
  type_mask = 0xD0000;
  high_mask = 0xFFE00000;
  high_value = 0x20000000;
  while (1)
  {
    entry = (u8 *) Asset_FindTable08ByU32Key(base, key);
    if (entry == 0)
    {
      return -1;
    }
    if (entry[3] == wanted)
    {
      value = *((u32 *) (entry + 8));
      if ((value & mask) == type_mask)
      {
        return ((value & high_mask) ^ high_value) != 0;
      }
    }
    key += 4;
  }

}

typedef struct AkaoPosPanWork {
    AkaoPackedRect3 rect;
    s32 out1;
    s32 out2;
} AkaoPosPanWork;


int Akao_Calc3DPan(AkaoPackedRect3 *pos, int *pan, int *volume);
s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, AkaoPosCoord arg3, AkaoPosCoord arg4, AkaoPosCoord arg5);

s32 Asset_Find08w(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    return Akao_SendPositionalCmd((void *)g_LoadedSceneAssetBlock[0], arg0, arg1, arg2, arg3, arg4);
}

s32 Akao_SetPos3D(s32 arg0, s32 arg1, u16 arg2, u16 arg3, u16 arg4) {
    AkaoPosPanWork local;
    s32 ret = 0;

    local.rect.x = arg2;
    local.rect.y = arg3;
    local.rect.z = arg4;
    Akao_Calc3DPan(&local.rect, &local.out1, &local.out2);

    {
        if (D_800B0CE8.reset_pending != 0) {
            u8 **table = &D_800B0CE8.voice_banks[3];

            ret = Akao_Cmd_24(table[arg0], arg1, local.out1, local.out2);
        }
    }

    return ret;
}

s32 Akao_SendPositionalCmdStereo(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    s32 ret;
    void **basep = g_AkaoBgmHandle;

    ret = Akao_SendPositionalCmd(*basep, arg0, arg1, arg2, arg3, arg4);
    Akao_SendPositionalCmd(*basep, arg0 + 1, arg1, arg2, arg3, arg4);
    return ret;
}

s32 Asset_Find08Alt(s32 arg0, s32 arg1, s16 arg2, s16 arg3, s16 arg4) {
    return Akao_SendPositionalCmd(g_AkaoBgmHandle[0], arg0, arg1, arg2, arg3, arg4);
}

s32 Akao_SendPositionalCmd(void *arg0, s32 arg1, s32 arg2, AkaoPosCoord arg3, AkaoPosCoord arg4, AkaoPosCoord arg5) {
    AkaoPackedRect3 rect;
    s32 out1;
    s32 out2;

    rect.x = arg3;
    rect.y = arg4;
    rect.z = arg5;

    Akao_Calc3DPan(&rect, &out1, &out2);
    return Akao_SendTableCommand(arg0, arg1, arg2, out1, out2);
}

int Akao_SendTableCommand(void *arg0, int arg1, int arg2, int arg3, int arg4) {
    void *entry = Asset_FindTable2CByU16Key(arg0, arg1);
    int ret;

    if (entry != 0) {
        ret = Akao_Cmd_24(entry, arg2, arg3, arg4);
    } else {
        ret = -1;
    }
    return ret;
}

int Akao_Calc3DPan(AkaoPackedRect3 *pos, int *pan, int *volume)
{
    Pe1GameState *state;
    long sxy;
    long x;
    long y;
    int depth;
    int far;
    int span;
    int low;

    Render_SetGteScreenOffset();
    state = &g_GameState;
    gte_ldrotmatrix(D_800BCFA4.value);
    gte_ldtransmatrix(D_800BCFA4.value);
    gte_ctc2_26(*D_800BCFA8.distance);
    depth = RotTransPers((GteShortVector *)pos, (int *)&sxy, (int *)&x, (int *)&y);
    x = (short)sxy;
    y = sxy >> 16;
    Render_ResetGteScreenOffset();

    *pan = ((x + 40) << 7) / 400 + 64;
    if ((unsigned int)*pan >= 256) {
        *pan = 255;
    }

    if (depth < g_GameState.bank_value_f8) {
        depth = g_GameState.bank_value_f8;
    } else if (g_GameState.bank_value_fa < depth) {
        depth = g_GameState.bank_value_fa;
    }

    far = state->bank_value_fa;
    depth = far - depth;
    depth = depth * depth / (far - state->bank_value_f8);
    span = state->bank_value_f7;
    low = state->bank_value_f6;
    span -= low;
    *volume = depth * span / (far - state->bank_value_f8) + low;
    if ((unsigned int)*volume >= 128) {
        *volume = 127;
    }
    return 0;
}

/* Contiguous scene-container helpers: TIM upload, key encoding and lookup. */

typedef unsigned char u8_1;

#include "Asset_SceneTablesAndSound_externs.h"

int Gpu_LoadTimAsset(TimUploadRecord *asset, void *base) {
    RECT rect;
    int mask;
    u8 *saved_base;
    unsigned int packed;
    int h;
    unsigned int offset;
    int secondary;
    unsigned int image_offset;
    u8 *image_address;

    packed = asset->image_rectangle;
    rect.x = (packed >> 10) & 0x7FF;
    packed = asset->image_rectangle;
    rect.y = packed >> 21;
    packed = asset->image_rectangle;
    rect.w = packed & 0x3FF;
    saved_base = base;
    packed = asset->image_source.bytes.height;
    if (packed != 0) {
        h = packed & 0xFF;
    } else {
        h = 0x100;
    }

    mask = 0xFFFFFF;
    rect.h = h;
    LoadImage(&rect, saved_base + (asset->image_source.offsetAndHeight & mask));

    secondary = asset->secondary_source.offsetAndHeight & mask;
    if (secondary != 0) {
        packed = asset->secondary_rectangle;
        rect.x = (packed >> 10) & 0x7FF;
        packed = asset->secondary_rectangle;
        rect.y = packed >> 21;
        packed = asset->secondary_rectangle;
        rect.w = packed & 0x3FF;
        rect.h = asset->secondary_source.bytes.height;
        image_offset = asset->image_source.offsetAndHeight & mask;
        image_address = saved_base + image_offset;
        offset = asset->secondary_source.offsetAndHeight & mask;
        LoadImage(&rect, offset + image_address);
    }

    return 0;
}

int Str_EncodeBase32(char *out, unsigned int value) {
    int i;
    int last;
    char *dst;

    i = 0;
    last = 5;
    dst = out;
    while (i < 6) {
        int shift = ((last - i) * 5) + 2;
        u8_1 c = ((u8_1 *)g_Base32CharTable)[(value >> shift) & 0x1F];

        *dst = c;
        if ((unsigned int)(c - 0x61) < 0x1A) {
            *dst = c - 0x20;
        }
        i++;
        dst++;
    }

    out[6] = 0;
    return 0;
}

int Str_DecodeBase32(unsigned int a0) {
    int total = 0;
    int i;

    for (i = 1; i < 5; i++) {
        int factor = 1;
        int j;

        for (j = 0; j < 4 - i; j++) {
            factor *= 10;
        }

        total += (g_Base32CharTable[(a0 >> ((5 * (5 - i)) + 2)) & 0x1F] - 0x30) * factor;
    }

    return total;
}

int Str_ParseBase32Id(signed char *arg0) {
    unsigned int result = 0;
    int index = 0;

    while (index < 6) {
        signed char c = *arg0;
        int value = ((unsigned int)(c - '0') < 10U)
            ? (c - '0')
            : (c - 'W') - (c >= 'k') - (c >= 'r') - (c >= 'y');

        result |= (unsigned int)value << ((5 - index) * 5 + 2);
        index++;
        arg0++;
    }

    return result;
}

int Str_ParseMapNumber(signed char *arg0) {
    int hundreds;
    int tens;
    int result;

    hundreds = arg0[2] - 0x30;
    result = hundreds * 3;
    result <<= 3;
    result += hundreds;
    result <<= 2;

    tens = arg0[3] - 0x30;
    result += tens * 10;
    result -= 0x30;

    return result + arg0[4];
}

void *Asset_FindTable08ByU32Key(void *arg0, s32 arg1) {
    u32 mask;
    register u8 *table asm("$2");
    u32 descriptor;
    register AssetU32KeyRecord *record asm("$6");
    register s32 none asm("$7");
    register u32 i asm("$8");
    u32 data_mask;
    char frame[1];

    asm volatile("" : : "r"(frame));

    mask = 0x3FFFFF;
    descriptor = ((SceneAssetDirectory *)SceneAsset_ResolveOffset(arg0,
        ((SceneAssetBlob *)arg0)->directoryOffset))->keyEntries;
    i = 0;
    none = 0;
    table = (u8 *)arg0 + (descriptor & mask);
    descriptor >>= 22;

    if (descriptor != 0) {
        data_mask = 0xFFFFFF;
        record = (AssetU32KeyRecord *)table;
        do {
            if (record->key == arg1) {
                return (u8 *)arg0 + (record->packedOffset & data_mask);
            }
            i++;
            if (i >= descriptor)
                break;
            record++;
        } while (1);
    }

    return (void *)none;
}

#define DEFINE_ASSET_SEARCHER(name, table_member, record_type) \
void *name(char *base, int key) { \
    u32 mask; \
    register char *table asm("$2"); \
    u32 descriptor; \
    register record_type *record asm("$6"); \
    register void *none asm("$7"); \
    register int i asm("$8"); \
    u32 data_mask; \
    char frame[1]; \
 \
    asm volatile("" : : "r"(frame)); \
    mask = 0x3FFFFF; \
    descriptor = ((SceneAssetDirectory *)(base + \
        ((SceneAssetBlob *)base)->directoryOffset))->table_member; \
    i = 0; \
    none = 0; \
    table = base + (descriptor & mask); \
    descriptor >>= 22; \
    if (descriptor != 0) { \
        data_mask = 0xFFFFFF; \
        record = (record_type *)table; \
        do { \
            if (record->key == key) { \
                return base + (record->offset & data_mask); \
            } \
            i++; \
            if (i >= descriptor) \
                break; \
            record++; \
        } while (1); \
    } \
    return none; \
}

DEFINE_ASSET_SEARCHER(Asset_FindTable2CByU16Key, streamEntries, SceneCdStreamRecord)
DEFINE_ASSET_SEARCHER(Asset_FindTable30ByU16Key, trackEntries, SceneTrackRecord)
