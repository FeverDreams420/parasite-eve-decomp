SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m220_museum_ROM_START = __romPos;
    room_m220_museum_VRAM = ADDR(.room_m220_museum);
    .room_m220_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m220_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m220_museum_RODATA_START = .;
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/room_m220_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m220_museum/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m220_museum_RODATA_END = .;
        room_m220_museum_RODATA_SIZE = ABSOLUTE(room_m220_museum_RODATA_END - room_m220_museum_RODATA_START);
        room_m220_museum_TEXT_START = .;
        build/JP/overlays/room_m220_museum/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m220_museum_TEXT_END = .;
        room_m220_museum_TEXT_SIZE = ABSOLUTE(room_m220_museum_TEXT_END - room_m220_museum_TEXT_START);
        room_m220_museum_DATA_START = .;
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/room_m220_museum.rodata.s.o(.data);
        build/JP/overlays/room_m220_museum/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.data);
        build/JP/overlays/room_m220_museum/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/D_80192590.data.s.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/room_m220_museum/D_801981E4.s.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/D_801C01E0.data.s.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/D_801C01E8.data.s.o(.data);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/D_801C01EC.data.s.o(.data);
        . = ALIGN(., 4);
        room_m220_museum_DATA_END = .;
        room_m220_museum_DATA_SIZE = ABSOLUTE(room_m220_museum_DATA_END - room_m220_museum_DATA_START);
        room_m220_museum_BSS_START = .;
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/room_m220_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m220_museum/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m220_museum/asm/JP/overlays/room_m220_museum/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m220_museum_BSS_END = .;
        room_m220_museum_BSS_SIZE = ABSOLUTE(room_m220_museum_BSS_END - room_m220_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m220_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m220_museum_ROM_END = __romPos;
    room_m220_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
