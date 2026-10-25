SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e18_cruiser_ROM_START = __romPos;
    scene_e18_cruiser_VRAM = ADDR(.scene_e18_cruiser);
    .scene_e18_cruiser (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e18_cruiser_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e18_cruiser_RODATA_START = .;
        build/JP/overlays/scene_e18_cruiser/asm/JP/overlays/scene_e18_cruiser/data/scene_e18_cruiser/scene_e18_header.s.o(.rodata);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/scene_e18_cruiser/e18_emitters.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e18_cruiser_RODATA_END = .;
        scene_e18_cruiser_RODATA_SIZE = ABSOLUTE(scene_e18_cruiser_RODATA_END - scene_e18_cruiser_RODATA_START);
        scene_e18_cruiser_TEXT_START = .;
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/scene_e18_cruiser/e18_emitters.c.o(.text);
        . = ALIGN(., 4);
        scene_e18_cruiser_TEXT_END = .;
        scene_e18_cruiser_TEXT_SIZE = ABSOLUTE(scene_e18_cruiser_TEXT_END - scene_e18_cruiser_TEXT_START);
        scene_e18_cruiser_DATA_START = .;
        build/JP/overlays/scene_e18_cruiser/asm/JP/overlays/scene_e18_cruiser/data/scene_e18_cruiser/scene_e18_header.s.o(.data);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/scene_e18_cruiser/e18_emitters.c.o(.data);
        build/JP/overlays/scene_e18_cruiser/asm/JP/overlays/scene_e18_cruiser/data/4EF4.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e18_cruiser_DATA_END = .;
        scene_e18_cruiser_DATA_SIZE = ABSOLUTE(scene_e18_cruiser_DATA_END - scene_e18_cruiser_DATA_START);
        scene_e18_cruiser_BSS_START = .;
        build/JP/overlays/scene_e18_cruiser/asm/JP/overlays/scene_e18_cruiser/data/scene_e18_cruiser/scene_e18_header.s.o(.bss);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e18_cruiser/src/jp/overlays/scene_e18_cruiser/e18_emitters.c.o(.bss);
        . = ALIGN(., 4);
        scene_e18_cruiser_BSS_END = .;
        scene_e18_cruiser_BSS_SIZE = ABSOLUTE(scene_e18_cruiser_BSS_END - scene_e18_cruiser_BSS_START);
    }
    __romPos += SIZEOF(.scene_e18_cruiser);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e18_cruiser_ROM_END = __romPos;
    scene_e18_cruiser_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
