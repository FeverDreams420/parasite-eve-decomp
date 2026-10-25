SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m030_sewer_ROM_START = __romPos;
    room_m030_sewer_VRAM = ADDR(.room_m030_sewer);
    .room_m030_sewer (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m030_sewer_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m030_sewer_RODATA_START = .;
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/room_m030_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m030_sewer_RODATA_END = .;
        room_m030_sewer_RODATA_SIZE = ABSOLUTE(room_m030_sewer_RODATA_END - room_m030_sewer_RODATA_START);
        room_m030_sewer_TEXT_START = .;
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m030_sewer_TEXT_END = .;
        room_m030_sewer_TEXT_SIZE = ABSOLUTE(room_m030_sewer_TEXT_END - room_m030_sewer_TEXT_START);
        room_m030_sewer_DATA_START = .;
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/room_m030_header.rodata.s.o(.data);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/D_8018FC78.data.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/D_8018FCB4.data.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/RoomSlotA_8018FC54.data.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/RoomSlotB_8018FC54.data.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/room_m030_sewer/RoomSlotC_8018FC54.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/D_801A0E48.data.s.o(.data);
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/room_m030_sewer/D_801A0E54.s.o(.data);
        . = ALIGN(., 4);
        room_m030_sewer_DATA_END = .;
        room_m030_sewer_DATA_SIZE = ABSOLUTE(room_m030_sewer_DATA_END - room_m030_sewer_DATA_START);
        room_m030_sewer_BSS_START = .;
        build/JP/overlays/room_m030_sewer/asm/JP/overlays/room_m030_sewer/data/room_m030_header.rodata.s.o(.bss);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m030_sewer/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m030_sewer_BSS_END = .;
        room_m030_sewer_BSS_SIZE = ABSOLUTE(room_m030_sewer_BSS_END - room_m030_sewer_BSS_START);
    }
    __romPos += SIZEOF(.room_m030_sewer);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m030_sewer_ROM_END = __romPos;
    room_m030_sewer_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
