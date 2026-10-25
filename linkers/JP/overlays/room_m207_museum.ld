SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m207_museum_ROM_START = __romPos;
    room_m207_museum_VRAM = ADDR(.room_m207_museum);
    .room_m207_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m207_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m207_museum_RODATA_START = .;
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/room_m207_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m207_museum/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m207_museum_RODATA_END = .;
        room_m207_museum_RODATA_SIZE = ABSOLUTE(room_m207_museum_RODATA_END - room_m207_museum_RODATA_START);
        room_m207_museum_TEXT_START = .;
        build/JP/overlays/room_m207_museum/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m207_museum_TEXT_END = .;
        room_m207_museum_TEXT_SIZE = ABSOLUTE(room_m207_museum_TEXT_END - room_m207_museum_TEXT_START);
        room_m207_museum_DATA_START = .;
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/room_m207_museum.rodata.s.o(.data);
        build/JP/overlays/room_m207_museum/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.data);
        build/JP/overlays/room_m207_museum/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/room_m207_museum/D_80192590.s.o(.data);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/D_801C3798.data.s.o(.data);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/room_m207_museum/D_801C37B0.s.o(.data);
        . = ALIGN(., 4);
        room_m207_museum_DATA_END = .;
        room_m207_museum_DATA_SIZE = ABSOLUTE(room_m207_museum_DATA_END - room_m207_museum_DATA_START);
        room_m207_museum_BSS_START = .;
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/room_m207_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m207_museum/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m207_museum/asm/JP/overlays/room_m207_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m207_museum_BSS_END = .;
        room_m207_museum_BSS_SIZE = ABSOLUTE(room_m207_museum_BSS_END - room_m207_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m207_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m207_museum_ROM_END = __romPos;
    room_m207_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
