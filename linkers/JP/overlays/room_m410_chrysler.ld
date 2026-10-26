SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m410_chrysler_ROM_START = __romPos;
    room_m410_chrysler_VRAM = ADDR(.room_m410_chrysler);
    .room_m410_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m410_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m410_chrysler_RODATA_START = .;
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/room_m410_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/sound_burst.c.o(.rodata);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m410_chrysler_RODATA_END = .;
        room_m410_chrysler_RODATA_SIZE = ABSOLUTE(room_m410_chrysler_RODATA_END - room_m410_chrysler_RODATA_START);
        room_m410_chrysler_TEXT_START = .;
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/sound_burst.c.o(.text);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m410_chrysler_TEXT_END = .;
        room_m410_chrysler_TEXT_SIZE = ABSOLUTE(room_m410_chrysler_TEXT_END - room_m410_chrysler_TEXT_START);
        room_m410_chrysler_DATA_START = .;
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/room_m410_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/sound_burst.c.o(.data);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/D_80192CE0.data.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/D_80192D04.data.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/D_80192D3C.data.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/D_80192D40.data.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/room_m410_chrysler/D_80192D44.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/D_801BE78C.data.s.o(.data);
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/room_m410_chrysler/D_801BE7BC.s.o(.data);
        . = ALIGN(., 4);
        room_m410_chrysler_DATA_END = .;
        room_m410_chrysler_DATA_SIZE = ABSOLUTE(room_m410_chrysler_DATA_END - room_m410_chrysler_DATA_START);
        room_m410_chrysler_BSS_START = .;
        build/JP/overlays/room_m410_chrysler/asm/JP/overlays/room_m410_chrysler/data/room_m410_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/sound_burst.c.o(.bss);
        build/JP/overlays/room_m410_chrysler/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m410_chrysler_BSS_END = .;
        room_m410_chrysler_BSS_SIZE = ABSOLUTE(room_m410_chrysler_BSS_END - room_m410_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m410_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m410_chrysler_ROM_END = __romPos;
    room_m410_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
