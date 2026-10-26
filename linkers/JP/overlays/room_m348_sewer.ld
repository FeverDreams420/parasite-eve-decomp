SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m348_sewer_ROM_START = __romPos;
    room_m348_sewer_VRAM = ADDR(.room_m348_sewer);
    .room_m348_sewer (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m348_sewer_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m348_sewer_RODATA_START = .;
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer.rodata.s.o(.rodata);
        build/JP/overlays/room_m348_sewer/src-jp/overlays/room_lib/drop_field.c.o(.rodata);
        . = ALIGN(., 4);
        room_m348_sewer_RODATA_END = .;
        room_m348_sewer_RODATA_SIZE = ABSOLUTE(room_m348_sewer_RODATA_END - room_m348_sewer_RODATA_START);
        room_m348_sewer_TEXT_START = .;
        build/JP/overlays/room_m348_sewer/src-jp/overlays/room_lib/drop_field.c.o(.text);
        . = ALIGN(., 4);
        room_m348_sewer_TEXT_END = .;
        room_m348_sewer_TEXT_SIZE = ABSOLUTE(room_m348_sewer_TEXT_END - room_m348_sewer_TEXT_START);
        room_m348_sewer_DATA_START = .;
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer.rodata.s.o(.data);
        build/JP/overlays/room_m348_sewer/src-jp/overlays/room_lib/drop_field.c.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192848.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192880.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_801928B8.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_801928F0.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_8019290C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/RoomLib_TableB_8018F06C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_801929E8.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192A00.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192C00.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E00.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E04.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E08.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E10.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E14.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E18.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E1C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E20.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E24.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E28.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E34.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E38.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E40.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E44.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E48.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E54.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E58.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E5C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E60.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E64.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E7C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E84.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E88.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E8C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E90.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E94.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192E9C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EA0.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EA4.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EA8.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EAC.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EB0.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EB4.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EB8.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EC0.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_80192EC4.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer/D_80192EC8.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer/D_801A81F8.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer/D_801B81E4.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/D_801C270C.data.s.o(.data);
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer/D_801C2718.s.o(.data);
        . = ALIGN(., 4);
        room_m348_sewer_DATA_END = .;
        room_m348_sewer_DATA_SIZE = ABSOLUTE(room_m348_sewer_DATA_END - room_m348_sewer_DATA_START);
        room_m348_sewer_BSS_START = .;
        build/JP/overlays/room_m348_sewer/asm/JP/overlays/room_m348_sewer/data/room_m348_sewer.rodata.s.o(.bss);
        build/JP/overlays/room_m348_sewer/src-jp/overlays/room_lib/drop_field.c.o(.bss);
        . = ALIGN(., 4);
        room_m348_sewer_BSS_END = .;
        room_m348_sewer_BSS_SIZE = ABSOLUTE(room_m348_sewer_BSS_END - room_m348_sewer_BSS_START);
    }
    __romPos += SIZEOF(.room_m348_sewer);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m348_sewer_ROM_END = __romPos;
    room_m348_sewer_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
