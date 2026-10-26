SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e06_chrysler_ROM_START = __romPos;
    scene_e06_chrysler_VRAM = ADDR(.scene_e06_chrysler);
    .scene_e06_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e06_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e06_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_chrysler/scene_e06_header.s.o(.rodata);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/seeker_spark.c.o(.rodata);
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_1EC_1FC.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        scene_e06_chrysler_RODATA_END = .;
        scene_e06_chrysler_RODATA_SIZE = ABSOLUTE(scene_e06_chrysler_RODATA_END - scene_e06_chrysler_RODATA_START);
        scene_e06_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/seeker_spark.c.o(.text);
        . = ALIGN(., 4);
        scene_e06_chrysler_TEXT_END = .;
        scene_e06_chrysler_TEXT_SIZE = ABSOLUTE(scene_e06_chrysler_TEXT_END - scene_e06_chrysler_TEXT_START);
        scene_e06_chrysler_DATA_START = .;
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_chrysler/scene_e06_header.s.o(.data);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/seeker_spark.c.o(.data);
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_1EC_1FC.rodata.s.o(.data);
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/3D8C.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e06_chrysler_DATA_END = .;
        scene_e06_chrysler_DATA_SIZE = ABSOLUTE(scene_e06_chrysler_DATA_END - scene_e06_chrysler_DATA_START);
        scene_e06_chrysler_BSS_START = .;
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_chrysler/scene_e06_header.s.o(.bss);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e06_chrysler/src-jp/overlays/room_lib/seeker_spark.c.o(.bss);
        build/JP/overlays/scene_e06_chrysler/asm/JP/overlays/scene_e06_chrysler/data/scene_e06_1EC_1FC.rodata.s.o(.bss);
        . = ALIGN(., 4);
        scene_e06_chrysler_BSS_END = .;
        scene_e06_chrysler_BSS_SIZE = ABSOLUTE(scene_e06_chrysler_BSS_END - scene_e06_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e06_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e06_chrysler_ROM_END = __romPos;
    scene_e06_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
