SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m245_museum_ROM_START = __romPos;
    room_m245_museum_VRAM = ADDR(.room_m245_museum);
    .room_m245_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m245_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m245_museum_RODATA_START = .;
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/room_m245_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.rodata);
        . = ALIGN(., 4);
        room_m245_museum_RODATA_END = .;
        room_m245_museum_RODATA_SIZE = ABSOLUTE(room_m245_museum_RODATA_END - room_m245_museum_RODATA_START);
        room_m245_museum_TEXT_START = .;
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.text);
        . = ALIGN(., 4);
        room_m245_museum_TEXT_END = .;
        room_m245_museum_TEXT_SIZE = ABSOLUTE(room_m245_museum_TEXT_END - room_m245_museum_TEXT_START);
        room_m245_museum_DATA_START = .;
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/room_m245_museum.rodata.s.o(.data);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.data);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801956AC.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801956F8.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195720.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195758.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801957BC.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801957E4.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_8019581C.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801959AC.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801959B0.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801959B4.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801959B8.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801959D8.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195B68.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195B6C.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195B70.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_80195B74.data.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/room_m245_museum/D_801981F8.s.o(.data);
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/D_801D01E0.data.s.o(.data);
        . = ALIGN(., 4);
        room_m245_museum_DATA_END = .;
        room_m245_museum_DATA_SIZE = ABSOLUTE(room_m245_museum_DATA_END - room_m245_museum_DATA_START);
        room_m245_museum_BSS_START = .;
        build/JP/overlays/room_m245_museum/asm/JP/overlays/room_m245_museum/data/room_m245_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m245_museum/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.bss);
        . = ALIGN(., 4);
        room_m245_museum_BSS_END = .;
        room_m245_museum_BSS_SIZE = ABSOLUTE(room_m245_museum_BSS_END - room_m245_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m245_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m245_museum_ROM_END = __romPos;
    room_m245_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
