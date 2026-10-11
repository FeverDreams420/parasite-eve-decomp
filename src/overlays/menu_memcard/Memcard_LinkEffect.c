#include "menu_memcard_link.h"

s32 Memcard_LinkEffect(s32 mode, MemcardLinkState *state) {
    GteShortVector unused;
    GteShortVector first;
    GteShortVector second;
    MemcardJointOffset offset = D_801ED818;
    RenderColor color;
    MemcardLinkColor color1 = D_801ED850;
    MemcardLinkColor color2 = D_801ED854;
    register MemcardJointOffset *offsetPtr asm("$6");
    switch (mode) {
    case 0: {
        s32 opacity;
        void *entity;
        register GteShortVector *firstPtr asm("$17");
        offsetPtr = &offset;
        __asm__("" : : "r"(offsetPtr));
        opacity = 128; entity = D_8009D254;
        __asm__("" : "=r"(entity), "=r"(opacity) : "0"(entity), "1"(opacity) : "$17");
        firstPtr = &first;
        state->opacity = opacity; state->phase = 0; state->timer = 0;
        __asm__("" : : : "memory");
        FieldEng_TransformMatrixPoint(entity, D_801F1D86, offsetPtr, firstPtr);
        FieldEng_TransformMatrixPoint(D_8009D254, D_801F1D88, &offset, &second);
        func_800D1384(&first, &second, 1016, (unsigned char *)&color2, (unsigned char *)&color1, 128, (struct FieldTrailPair *)&state->rotation, 1);
        return func_800CE560(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 22,
                             (FieldAnimCallbackListCallback)Memcard_PulseImage);
    }
    case 1:
        {
            s16 timer = state->timer + 1;
            void *entity = D_8009D254;

            state->timer = timer;
            __asm__("" : : : "memory");
            FieldEng_TransformMatrixPoint(entity, D_801F1D88, &offset, &unused);
        }
        if (state->timer >= 23 && state->opacity > 0) state->opacity -= 8;
        if (state->timer < 22) return 0;
        __asm__ volatile("" : :);
        return 2;
    case 2:
        if (state->opacity >= 0) {
            func_800CF3AC(D_801F1D5C, &color, state->timer);
            FieldEng_TransformMatrixPoint(D_8009D254, D_801F1D86, &offset, &first);
            FieldEng_TransformMatrixPoint(D_8009D254, D_801F1D88, &offset, &second);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D1384(&first, &second, 16, (unsigned char *)&color2, (unsigned char *)&color1,
                         state->opacity, (struct FieldTrailPair *)&state->rotation, 1);
        }
        {
            u16 tpage = D_800E2850[D_800E11E6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
