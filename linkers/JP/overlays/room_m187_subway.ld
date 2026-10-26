SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m187_subway_ROM_START = __romPos;
    room_m187_subway_VRAM = ADDR(.room_m187_subway);
    .room_m187_subway (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m187_subway_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m187_subway_RODATA_START = .;
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/room_m187_subway.rodata.s.o(.rodata);
        build/JP/overlays/room_m187_subway/src-jp/overlays/room_lib/orbit_set.c.o(.rodata);
        . = ALIGN(., 4);
        room_m187_subway_RODATA_END = .;
        room_m187_subway_RODATA_SIZE = ABSOLUTE(room_m187_subway_RODATA_END - room_m187_subway_RODATA_START);
        room_m187_subway_TEXT_START = .;
        build/JP/overlays/room_m187_subway/src-jp/overlays/room_lib/orbit_set.c.o(.text);
        . = ALIGN(., 4);
        room_m187_subway_TEXT_END = .;
        room_m187_subway_TEXT_SIZE = ABSOLUTE(room_m187_subway_TEXT_END - room_m187_subway_TEXT_START);
        room_m187_subway_DATA_START = .;
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/room_m187_subway.rodata.s.o(.data);
        build/JP/overlays/room_m187_subway/src-jp/overlays/room_lib/orbit_set.c.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_8018FFA8.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/RoomLib_TableC.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_8018FFD0.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_8018FFE4.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_8018FFF0.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/RoomLib_TableB_8018F01C.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190080.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190084.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190088.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190090.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190094.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_80190098.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801900A0.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801900A4.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801900A8.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801900B0.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801900B4.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/room_m187_subway/D_801900B8.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/D_801A6580.data.s.o(.data);
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/room_m187_subway/D_801A658C.s.o(.data);
        . = ALIGN(., 4);
        room_m187_subway_DATA_END = .;
        room_m187_subway_DATA_SIZE = ABSOLUTE(room_m187_subway_DATA_END - room_m187_subway_DATA_START);
        room_m187_subway_BSS_START = .;
        build/JP/overlays/room_m187_subway/asm/JP/overlays/room_m187_subway/data/room_m187_subway.rodata.s.o(.bss);
        build/JP/overlays/room_m187_subway/src-jp/overlays/room_lib/orbit_set.c.o(.bss);
        . = ALIGN(., 4);
        room_m187_subway_BSS_END = .;
        room_m187_subway_BSS_SIZE = ABSOLUTE(room_m187_subway_BSS_END - room_m187_subway_BSS_START);
    }
    __romPos += SIZEOF(.room_m187_subway);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m187_subway_ROM_END = __romPos;
    room_m187_subway_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
