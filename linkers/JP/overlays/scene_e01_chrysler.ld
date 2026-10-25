SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e01_chrysler_ROM_START = __romPos;
    scene_e01_chrysler_VRAM = ADDR(.scene_e01_chrysler);
    .scene_e01_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e01_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e01_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e01_chrysler/asm/JP/overlays/scene_e01_chrysler/data/scene_e01_header.s.o(.rodata);
        build/JP/overlays/scene_e01_chrysler/src/jp/overlays/room_lib/grab_attack.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e01_chrysler_RODATA_END = .;
        scene_e01_chrysler_RODATA_SIZE = ABSOLUTE(scene_e01_chrysler_RODATA_END - scene_e01_chrysler_RODATA_START);
        scene_e01_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e01_chrysler/src/jp/overlays/room_lib/grab_attack.c.o(.text);
        . = ALIGN(., 4);
        scene_e01_chrysler_TEXT_END = .;
        scene_e01_chrysler_TEXT_SIZE = ABSOLUTE(scene_e01_chrysler_TEXT_END - scene_e01_chrysler_TEXT_START);
        scene_e01_chrysler_DATA_START = .;
        build/JP/overlays/scene_e01_chrysler/asm/JP/overlays/scene_e01_chrysler/data/scene_e01_header.s.o(.data);
        build/JP/overlays/scene_e01_chrysler/src/jp/overlays/room_lib/grab_attack.c.o(.data);
        build/JP/overlays/scene_e01_chrysler/asm/JP/overlays/scene_e01_chrysler/data/31E0.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e01_chrysler_DATA_END = .;
        scene_e01_chrysler_DATA_SIZE = ABSOLUTE(scene_e01_chrysler_DATA_END - scene_e01_chrysler_DATA_START);
        scene_e01_chrysler_BSS_START = .;
        build/JP/overlays/scene_e01_chrysler/asm/JP/overlays/scene_e01_chrysler/data/scene_e01_header.s.o(.bss);
        build/JP/overlays/scene_e01_chrysler/src/jp/overlays/room_lib/grab_attack.c.o(.bss);
        . = ALIGN(., 4);
        scene_e01_chrysler_BSS_END = .;
        scene_e01_chrysler_BSS_SIZE = ABSOLUTE(scene_e01_chrysler_BSS_END - scene_e01_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e01_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e01_chrysler_ROM_END = __romPos;
    scene_e01_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
