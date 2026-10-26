SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e22_liberty_ROM_START = __romPos;
    scene_e22_liberty_VRAM = ADDR(.scene_e22_liberty);
    .scene_e22_liberty (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e22_liberty_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e22_liberty_RODATA_START = .;
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22.rodata.s.o(.rodata);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22_tail.rodata.s.o(.rodata);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/scene_e22_liberty/e22.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e22_liberty_RODATA_END = .;
        scene_e22_liberty_RODATA_SIZE = ABSOLUTE(scene_e22_liberty_RODATA_END - scene_e22_liberty_RODATA_START);
        scene_e22_liberty_TEXT_START = .;
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/scene_e22_liberty/e22.c.o(.text);
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22_data_00A0D8.data.s.o(.code_data);
        . = ALIGN(., 4);
        scene_e22_liberty_TEXT_END = .;
        scene_e22_liberty_TEXT_SIZE = ABSOLUTE(scene_e22_liberty_TEXT_END - scene_e22_liberty_TEXT_START);
        scene_e22_liberty_DATA_START = .;
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22.rodata.s.o(.data);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22_tail.rodata.s.o(.data);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/scene_e22_liberty/e22.c.o(.data);
        . = ALIGN(., 4);
        scene_e22_liberty_DATA_END = .;
        scene_e22_liberty_DATA_SIZE = ABSOLUTE(scene_e22_liberty_DATA_END - scene_e22_liberty_DATA_START);
        scene_e22_liberty_BSS_START = .;
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22.rodata.s.o(.bss);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e22_liberty/asm/JP/overlays/scene_e22_liberty/data/scene_e22_tail.rodata.s.o(.bss);
        build/JP/overlays/scene_e22_liberty/src-jp/overlays/scene_e22_liberty/e22.c.o(.bss);
        . = ALIGN(., 4);
        scene_e22_liberty_BSS_END = .;
        scene_e22_liberty_BSS_SIZE = ABSOLUTE(scene_e22_liberty_BSS_END - scene_e22_liberty_BSS_START);
    }
    __romPos += SIZEOF(.scene_e22_liberty);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e22_liberty_ROM_END = __romPos;
    scene_e22_liberty_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
