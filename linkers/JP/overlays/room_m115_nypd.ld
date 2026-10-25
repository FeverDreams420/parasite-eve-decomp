SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m115_nypd_ROM_START = __romPos;
    room_m115_nypd_VRAM = ADDR(.room_m115_nypd);
    .room_m115_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m115_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m115_nypd_RODATA_START = .;
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/room_m115_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m115_nypd_RODATA_END = .;
        room_m115_nypd_RODATA_SIZE = ABSOLUTE(room_m115_nypd_RODATA_END - room_m115_nypd_RODATA_START);
        room_m115_nypd_TEXT_START = .;
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m115_nypd_TEXT_END = .;
        room_m115_nypd_TEXT_SIZE = ABSOLUTE(room_m115_nypd_TEXT_END - room_m115_nypd_TEXT_START);
        room_m115_nypd_DATA_START = .;
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/room_m115_header.rodata.s.o(.data);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/D_8018FE3C.data.s.o(.data);
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/D_8018FE5C.data.s.o(.data);
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/room_m115_nypd/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/D_801C01E4.data.s.o(.data);
        . = ALIGN(., 4);
        room_m115_nypd_DATA_END = .;
        room_m115_nypd_DATA_SIZE = ABSOLUTE(room_m115_nypd_DATA_END - room_m115_nypd_DATA_START);
        room_m115_nypd_BSS_START = .;
        build/JP/overlays/room_m115_nypd/asm/JP/overlays/room_m115_nypd/data/room_m115_header.rodata.s.o(.bss);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m115_nypd/src/jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m115_nypd_BSS_END = .;
        room_m115_nypd_BSS_SIZE = ABSOLUTE(room_m115_nypd_BSS_END - room_m115_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m115_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m115_nypd_ROM_END = __romPos;
    room_m115_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
