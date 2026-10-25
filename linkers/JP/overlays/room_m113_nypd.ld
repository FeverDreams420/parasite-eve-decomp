SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m113_nypd_ROM_START = __romPos;
    room_m113_nypd_VRAM = ADDR(.room_m113_nypd);
    .room_m113_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m113_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m113_nypd_RODATA_START = .;
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m113_nypd_RODATA_END = .;
        room_m113_nypd_RODATA_SIZE = ABSOLUTE(room_m113_nypd_RODATA_END - room_m113_nypd_RODATA_START);
        room_m113_nypd_TEXT_START = .;
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m113_nypd_TEXT_END = .;
        room_m113_nypd_TEXT_SIZE = ABSOLUTE(room_m113_nypd_TEXT_END - room_m113_nypd_TEXT_START);
        room_m113_nypd_DATA_START = .;
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_header.rodata.s.o(.data);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/D_80190AC0.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/D_80190AFC.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/D_80190B34.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/D_80190B54.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/RoomSlotA_8018FC68.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/RoomSlotB_8018FC68.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/RoomSlotC_8018FC68.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_nypd/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_nypd/D_801A5F78.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/D_801BC544.data.s.o(.data);
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_nypd/D_801BC55C.s.o(.data);
        . = ALIGN(., 4);
        room_m113_nypd_DATA_END = .;
        room_m113_nypd_DATA_SIZE = ABSOLUTE(room_m113_nypd_DATA_END - room_m113_nypd_DATA_START);
        room_m113_nypd_BSS_START = .;
        build/JP/overlays/room_m113_nypd/asm/JP/overlays/room_m113_nypd/data/room_m113_header.rodata.s.o(.bss);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_args.c.o(.bss);
        build/JP/overlays/room_m113_nypd/src/jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m113_nypd_BSS_END = .;
        room_m113_nypd_BSS_SIZE = ABSOLUTE(room_m113_nypd_BSS_END - room_m113_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m113_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m113_nypd_ROM_END = __romPos;
    room_m113_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
