#include "menu_memcard_glint.h"
#include "menu_memcard_link.h"
#include "menu_memcard_pulse.h"
#include "menu_memcard_trail.h"

s32 MenuCard_PulseImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    RenderColor color;
    s32 angle;
    s32 scale;
    s32 palette;
    register s16 timer;
    register s32 twist;
    register void *track;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 16) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = state->timer << 6;
        scale = rsin(angle) + 2048;
        rcos(angle);
        rotation.x = 0; rotation.y = 0; twist = state->timer;
        __asm__("" : : "r"(twist));
        track = D_801F1D5C;
        __asm__("" : : "r"(track));
        rotation.flags = 0;
        __asm__("" : "=r"(twist) : "0"(twist) : "memory");
        rotation.z = twist << 5;
        func_800CF3AC(track, &color, state->timer << 1);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428 != 0) palette += 4;
        func_800CEE20(&state->position, &rotation, scale, scale, 130,
                     GetClut(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}

s32 MenuCard_LinkEffect(s32 mode, MemcardLinkState *state) {
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
        FieldFx_TransformMatrixPoint(entity, D_801F1D86, offsetPtr, firstPtr);
        FieldFx_TransformMatrixPoint(D_8009D254, D_801F1D88, &offset, &second);
        func_800D1384(&first, &second, 1016, (unsigned char *)&color2, (unsigned char *)&color1, 128, (struct FieldTrailPair *)&state->rotation, 1);
        return func_800CE560(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 22,
                             (FieldFxCallbackListCallback)MenuCard_PulseImage);
    }
    case 1:
        {
            s16 timer = state->timer + 1;
            void *entity = D_8009D254;

            state->timer = timer;
            __asm__("" : : : "memory");
            FieldFx_TransformMatrixPoint(entity, D_801F1D88, &offset, &unused);
        }
        if (state->timer >= 23 && state->opacity > 0) state->opacity -= 8;
        if (state->timer < 22) return 0;
        __asm__ volatile("" : :);
        return 2;
    case 2:
        if (state->opacity >= 0) {
            func_800CF3AC(D_801F1D5C, &color, state->timer);
            FieldFx_TransformMatrixPoint(D_8009D254, D_801F1D86, &offset, &first);
            FieldFx_TransformMatrixPoint(D_8009D254, D_801F1D88, &offset, &second);
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

s32 MenuCard_CosineImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale, palette;
    register s32 intensity;
    register s32 cosine asm("$3");
    register s32 index;
    register s32 four;
    s16 timer;
    s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = rcos(angle) + 2048;
        cosine = rcos(angle);
        __asm__("" : : "r"(cosine));
        intensity = cosine / 32;
        __asm__("" : : "r"(intensity));
        rotation.x = 0; rotation.y = 0;
        twist = state->timer;
        index = D_800F3368.palette;
        __asm__ volatile("" ::: "memory");
        four = 4;
        __asm__("" : : "r"(four));
        rotation.flags = 0;
        rotation.z = twist << 5;
        palette = D_800E1204[index];
        if (index == four && D_800F3428 != 0) palette += 7;
        else palette += 3;
        func_800CEE20(&state->position, &rotation, scale, scale, 66,
                     GetClut(0, palette), 1, intensity, &D_801F1F30);
        break;
    }
    return 0;
}

s32 MenuCard_TrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardJointOffset offset = D_801ED818;
    RenderColor color;
    s32 scale, palette;
    s32 index;
    s32 four;
    switch(mode) {
    case 0:
        state->attachment = 19; state->phase = 0; state->timer = 0;
        return func_800CE560(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 24,
                             (FieldFxCallbackListCallback)MenuCard_CosineImage);
    case 1:
        state->timer++;
        FieldFx_TransformMatrixPoint(D_8009D254, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = func_800CE610(ANIM_POOL_AS(MemcardLinkChannel)->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                image->phase = 0; image->timer = 0;
            }
        }
        if (state->timer >= 32) return 2;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        FieldFx_TransformMatrixPoint(D_8009D254, state->attachment, &offset, &position);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32; D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11F6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 32;
        if (state->timer < 33) {
            func_800CF3AC(D_801F1DE8, &color, state->timer);
            scale = rsin(state->timer << 6) + 4096;
            __asm__("" : "=r"(scale) : "0"(scale) : "memory");
            index = D_800F3368.palette;

            four = 4;

            palette = D_800E1204[index];
            if (index == four && D_800F3428) palette += 8;
            else palette += 4;
            func_800CEE20(&position, 0, scale, scale, 64, GetClut(0,palette), 1, 128, &color);
            index = D_800F3368.palette;
            __asm__("" : "=r"(index) : "0"(index) : "$3", "memory");
            four = 4;

            palette = D_800E1204[index];
            if (index == four && D_800F3428) palette += 7;
            else palette += 3;
            func_800CEE20(&position, 0, scale, scale, 70, GetClut(0,palette), 1, 128, &color);
            D_801F1F30.r = color.r; D_801F1F30.g = color.g; D_801F1F30.b = color.b;
        } else {
            *(u32 *)&D_801F1F30 = 0;
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}

s32 MenuCard_SineRotatingImage(s32 mode, MemcardPulseState *state) {
    GteRotation rotation;
    s32 angle, scale;
    register s32 palette;
    register s32 intensity;
    register s32 cosine asm("$5");
    register s32 index;
    register s32 four asm("$3");
    s16 timer;
    register s32 twist;
    switch (mode) {
    case 1:
        if (state->phase != 0) return 0;
        timer = state->timer + 1;
        state->timer = timer;
        if (timer < 12) return 0;
        __asm__ volatile("" : : : "memory");
        return 1;
    case 2:
        if (state->phase != 0) return 0;
        angle = (state->timer << 10) / 12;
        scale = rsin(angle) + 4096;
        cosine = rcos(angle);
        if (cosine < 0) cosine += 31;
        rotation.x = state->rotation.x;
        index = D_800F3368.palette;
        rotation.y = state->rotation.y;
        rotation.z = state->rotation.z;
        intensity = cosine >> 5;
        __asm__ volatile("" ::: "memory");
        twist = state->timer;
        rotation.flags = 1;
        rotation.z = twist * 50;
        {
            four = 4;
            palette = D_800E1204[index];
        }
        if (index == four && D_800F3428 != 0) palette += 7;
        else palette += 3;
        func_800CEE20(&state->position, &rotation, scale, scale, 68,
                     GetClut(0, palette), 1, intensity, &D_801F1F34);
        break;
    }
    return 0;
}

s32 MenuCard_SineTrailEffect(s32 mode, MemcardTrailState *state) {
    GteShortVector position;
    MemcardJointOffset offset = D_801ED818;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    s32 scale, kind, palette;

    switch (mode) {
    case 0:
        state->attachment = 25;
        state->phase = 0;
        state->timer = 0;
        return func_800CE560(ANIM_POOL_AS(MemcardLinkChannel)->slots, 20, 24,
                             (FieldFxCallbackListCallback)MenuCard_SineRotatingImage);
    case 1:
        state->timer++;
        FieldFx_TransformMatrixPoint(D_8009D254, state->attachment, &offset, &position);
        if (state->timer < 33 && (state->timer & 1)) {
            MemcardPulseState *image = func_800CE610(ANIM_POOL_AS(MemcardLinkChannel)->slots);
            if (image) {
                image->position.x = position.x;
                image->position.y = position.y;
                image->position.z = position.z;
                func_800CE9D4(D_8009D254, 0, (GteShortVector *)&image->rotation);
                image->phase = 0;
                image->timer = 0;
            }
        }
        if (state->timer >= 32) {
            return 2;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        FieldFx_TransformMatrixPoint(D_8009D254, state->attachment, &offset, &position);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        {
            u16 tpage = D_800E2850[D_800E11F6];
            D_800F3368.palette = 1;
            D_800F3368.tpage = tpage;
        }
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 5;
        D_800F3368.depth = 32;
        if (state->timer < 33) {
            func_800CF3AC(D_801F1E74, &color, state->timer);
            scale = rsin(state->timer << 6) / 2 + 2048;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&position, 0, scale, scale, 64,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, 128, &color);
            D_801F1F34.r = color.r;
            D_801F1F34.g = color.g;
            D_801F1F34.b = color.b;
        } else {
            *(u32 *)&D_801F1F34 = 0;
        }
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}

/* Origin callbacks of the glint effect descriptors; every effect is
 * anchored at the shared glint origin. */
GteShortVector *MenuCard_GetGlintOriginA(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginB(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginC(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginD(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginE(void) {
    return &D_801F1F28;
}

GteShortVector *MenuCard_GetGlintOriginF(void) {
    return &D_801F1F28;
}

/* Update hook of the cross flash: keeps it alive while the flash raises the
 * flag each frame and clears the flag for the next frame. */
int MenuCard_ConsumeCrossFlashFlag(int mode) {
    if (mode == 1) {
        if (D_801F1F3A == 0) return 1;
        D_801F1F3A = 0;
    }
    return 0;
}
