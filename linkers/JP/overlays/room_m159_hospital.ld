SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m159_hospital_ROM_START = __romPos;
    room_m159_hospital_VRAM = ADDR(.room_m159_hospital);
    .room_m159_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m159_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m159_hospital_RODATA_START = .;
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/room_m159_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m159_hospital_RODATA_END = .;
        room_m159_hospital_RODATA_SIZE = ABSOLUTE(room_m159_hospital_RODATA_END - room_m159_hospital_RODATA_START);
        room_m159_hospital_TEXT_START = .;
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m159_hospital_TEXT_END = .;
        room_m159_hospital_TEXT_SIZE = ABSOLUTE(room_m159_hospital_TEXT_END - room_m159_hospital_TEXT_START);
        room_m159_hospital_DATA_START = .;
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/room_m159_header.rodata.s.o(.data);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/D_8018FE3C.data.s.o(.data);
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/D_8018FE5C.data.s.o(.data);
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/room_m159_hospital/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/D_801ACE18.data.s.o(.data);
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/room_m159_hospital/D_801ACE24.s.o(.data);
        . = ALIGN(., 4);
        room_m159_hospital_DATA_END = .;
        room_m159_hospital_DATA_SIZE = ABSOLUTE(room_m159_hospital_DATA_END - room_m159_hospital_DATA_START);
        room_m159_hospital_BSS_START = .;
        build/JP/overlays/room_m159_hospital/asm/JP/overlays/room_m159_hospital/data/room_m159_header.rodata.s.o(.bss);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m159_hospital/src/jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m159_hospital_BSS_END = .;
        room_m159_hospital_BSS_SIZE = ABSOLUTE(room_m159_hospital_BSS_END - room_m159_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m159_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m159_hospital_ROM_END = __romPos;
    room_m159_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
