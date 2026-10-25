SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e26_chrysler_ROM_START = __romPos;
    scene_e26_chrysler_VRAM = ADDR(.scene_e26_chrysler);
    .scene_e26_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e26_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e26_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e26_chrysler/asm/JP/overlays/scene_e26_chrysler/data/scene_e26_chrysler/scene_e26_header.s.o(.rodata);
        build/JP/overlays/scene_e26_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e26_chrysler_RODATA_END = .;
        scene_e26_chrysler_RODATA_SIZE = ABSOLUTE(scene_e26_chrysler_RODATA_END - scene_e26_chrysler_RODATA_START);
        scene_e26_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e26_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        scene_e26_chrysler_TEXT_END = .;
        scene_e26_chrysler_TEXT_SIZE = ABSOLUTE(scene_e26_chrysler_TEXT_END - scene_e26_chrysler_TEXT_START);
        scene_e26_chrysler_DATA_START = .;
        build/JP/overlays/scene_e26_chrysler/asm/JP/overlays/scene_e26_chrysler/data/scene_e26_chrysler/scene_e26_header.s.o(.data);
        build/JP/overlays/scene_e26_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e26_chrysler/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/scene_e26_chrysler/asm/JP/overlays/scene_e26_chrysler/data/35A8.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e26_chrysler_DATA_END = .;
        scene_e26_chrysler_DATA_SIZE = ABSOLUTE(scene_e26_chrysler_DATA_END - scene_e26_chrysler_DATA_START);
        scene_e26_chrysler_BSS_START = .;
        build/JP/overlays/scene_e26_chrysler/asm/JP/overlays/scene_e26_chrysler/data/scene_e26_chrysler/scene_e26_header.s.o(.bss);
        build/JP/overlays/scene_e26_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        . = ALIGN(., 4);
        scene_e26_chrysler_BSS_END = .;
        scene_e26_chrysler_BSS_SIZE = ABSOLUTE(scene_e26_chrysler_BSS_END - scene_e26_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e26_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e26_chrysler_ROM_END = __romPos;
    scene_e26_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
