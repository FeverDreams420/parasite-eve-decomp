SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m256_museum_ROM_START = __romPos;
    room_m256_museum_VRAM = ADDR(.room_m256_museum);
    .room_m256_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m256_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m256_museum_RODATA_START = .;
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomLib_HandlerCArgsData_80191630.rodata.s.o(.rodata);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256.c.o(.rodata);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomEffect_TrailBurstPadding.rodata.s.o(.rodata);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256_class.c.o(.rodata);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/class_update_callback.c.o(.rodata);
        . = ALIGN(., 4);
        room_m256_museum_RODATA_END = .;
        room_m256_museum_RODATA_SIZE = ABSOLUTE(room_m256_museum_RODATA_END - room_m256_museum_RODATA_START);
        room_m256_museum_TEXT_START = .;
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256_class.c.o(.text);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/class_update_callback.c.o(.text);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256.c.o(.text);
        . = ALIGN(., 4);
        room_m256_museum_TEXT_END = .;
        room_m256_museum_TEXT_SIZE = ABSOLUTE(room_m256_museum_TEXT_END - room_m256_museum_TEXT_START);
        room_m256_museum_DATA_START = .;
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum.rodata.s.o(.data);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomLib_HandlerCArgsData_80191630.rodata.s.o(.data);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256.c.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomEffect_TrailBurstPadding.rodata.s.o(.data);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256_class.c.o(.data);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/class_update_callback.c.o(.data);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195DA8.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195DC4.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195DE8.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195E20.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195E64.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195E8C.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195EB4.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195EEC.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195EF0.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/g_RoomM256TrailConfig.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195EF8.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195EFC.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195F1C.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80195F20.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80196094.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_80196098.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_801960A0.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_801960A4.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum/D_801960A8.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_801B81E8.data.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum/D_801C01E0.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum/D_801D01E8.s.o(.data);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/D_801E01E0.data.s.o(.data);
        . = ALIGN(., 4);
        room_m256_museum_DATA_END = .;
        room_m256_museum_DATA_SIZE = ABSOLUTE(room_m256_museum_DATA_END - room_m256_museum_DATA_START);
        room_m256_museum_BSS_START = .;
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/room_m256_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomLib_HandlerCArgsData_80191630.rodata.s.o(.bss);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256.c.o(.bss);
        build/JP/overlays/room_m256_museum/asm/JP/overlays/room_m256_museum/data/RoomEffect_TrailBurstPadding.rodata.s.o(.bss);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_m256_museum/m256_class.c.o(.bss);
        build/JP/overlays/room_m256_museum/src/jp/overlays/room_lib/class_update_callback.c.o(.bss);
        . = ALIGN(., 4);
        room_m256_museum_BSS_END = .;
        room_m256_museum_BSS_SIZE = ABSOLUTE(room_m256_museum_BSS_END - room_m256_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m256_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m256_museum_ROM_END = __romPos;
    room_m256_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
