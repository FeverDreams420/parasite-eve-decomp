SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m032_sewer_ROM_START = __romPos;
    room_m032_sewer_VRAM = ADDR(.room_m032_sewer);
    .room_m032_sewer (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m032_sewer_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m032_sewer_RODATA_START = .;
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/room_m032_sewer.rodata.s.o(.rodata);
        build/JP/overlays/room_m032_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m032_sewer_RODATA_END = .;
        room_m032_sewer_RODATA_SIZE = ABSOLUTE(room_m032_sewer_RODATA_END - room_m032_sewer_RODATA_START);
        room_m032_sewer_TEXT_START = .;
        build/JP/overlays/room_m032_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m032_sewer_TEXT_END = .;
        room_m032_sewer_TEXT_SIZE = ABSOLUTE(room_m032_sewer_TEXT_END - room_m032_sewer_TEXT_START);
        room_m032_sewer_DATA_START = .;
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/room_m032_sewer.rodata.s.o(.data);
        build/JP/overlays/room_m032_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.data);
        build/JP/overlays/room_m032_sewer/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/D_80192590.data.s.o(.data);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/room_m032_sewer/D_8019A268.s.o(.data);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/D_801A68B0.data.s.o(.data);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/room_m032_sewer/D_801A68BC.s.o(.data);
        . = ALIGN(., 4);
        room_m032_sewer_DATA_END = .;
        room_m032_sewer_DATA_SIZE = ABSOLUTE(room_m032_sewer_DATA_END - room_m032_sewer_DATA_START);
        room_m032_sewer_BSS_START = .;
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/room_m032_sewer.rodata.s.o(.bss);
        build/JP/overlays/room_m032_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m032_sewer/asm/JP/overlays/room_m032_sewer/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m032_sewer_BSS_END = .;
        room_m032_sewer_BSS_SIZE = ABSOLUTE(room_m032_sewer_BSS_END - room_m032_sewer_BSS_START);
    }
    __romPos += SIZEOF(.room_m032_sewer);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m032_sewer_ROM_END = __romPos;
    room_m032_sewer_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
