SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e20_cruiser_ROM_START = __romPos;
    scene_e20_cruiser_VRAM = ADDR(.scene_e20_cruiser);
    .scene_e20_cruiser (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e20_cruiser_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e20_cruiser_RODATA_START = .;
        build/JP/overlays/scene_e20_cruiser/asm/JP/overlays/scene_e20_cruiser/data/scene_e20_cruiser/scene_e20_header.s.o(.rodata);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_HoverOrbController.c.o(.rodata);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_FlareParticle.c.o(.rodata);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_CommandEffectSlot.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e20_cruiser_RODATA_END = .;
        scene_e20_cruiser_RODATA_SIZE = ABSOLUTE(scene_e20_cruiser_RODATA_END - scene_e20_cruiser_RODATA_START);
        scene_e20_cruiser_TEXT_START = .;
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_FlareParticle.c.o(.text);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_HoverOrbController.c.o(.text);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_CommandEffectSlot.c.o(.text);
        . = ALIGN(., 4);
        scene_e20_cruiser_TEXT_END = .;
        scene_e20_cruiser_TEXT_SIZE = ABSOLUTE(scene_e20_cruiser_TEXT_END - scene_e20_cruiser_TEXT_START);
        scene_e20_cruiser_DATA_START = .;
        build/JP/overlays/scene_e20_cruiser/asm/JP/overlays/scene_e20_cruiser/data/scene_e20_cruiser/scene_e20_header.s.o(.data);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_HoverOrbController.c.o(.data);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_FlareParticle.c.o(.data);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_CommandEffectSlot.c.o(.data);
        build/JP/overlays/scene_e20_cruiser/asm/JP/overlays/scene_e20_cruiser/data/scene_e20_tail.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e20_cruiser_DATA_END = .;
        scene_e20_cruiser_DATA_SIZE = ABSOLUTE(scene_e20_cruiser_DATA_END - scene_e20_cruiser_DATA_START);
        scene_e20_cruiser_BSS_START = .;
        build/JP/overlays/scene_e20_cruiser/asm/JP/overlays/scene_e20_cruiser/data/scene_e20_cruiser/scene_e20_header.s.o(.bss);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_HoverOrbController.c.o(.bss);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_FlareParticle.c.o(.bss);
        build/JP/overlays/scene_e20_cruiser/src-jp/overlays/scene_e20_cruiser/SceneE20_CommandEffectSlot.c.o(.bss);
        . = ALIGN(., 4);
        scene_e20_cruiser_BSS_END = .;
        scene_e20_cruiser_BSS_SIZE = ABSOLUTE(scene_e20_cruiser_BSS_END - scene_e20_cruiser_BSS_START);
    }
    __romPos += SIZEOF(.scene_e20_cruiser);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e20_cruiser_ROM_END = __romPos;
    scene_e20_cruiser_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
