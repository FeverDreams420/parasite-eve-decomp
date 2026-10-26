SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m013_theater_ROM_START = __romPos;
    room_m013_theater_VRAM = ADDR(.room_m013_theater);
    .room_m013_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m013_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m013_theater_RODATA_START = .;
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/room_m013_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m013_theater_RODATA_END = .;
        room_m013_theater_RODATA_SIZE = ABSOLUTE(room_m013_theater_RODATA_END - room_m013_theater_RODATA_START);
        room_m013_theater_TEXT_START = .;
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m013_theater_TEXT_END = .;
        room_m013_theater_TEXT_SIZE = ABSOLUTE(room_m013_theater_TEXT_END - room_m013_theater_TEXT_START);
        room_m013_theater_DATA_START = .;
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/room_m013_header.rodata.s.o(.data);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/D_8018FC78.data.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/D_8018FCB4.data.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/RoomSlotA_8018FC54.data.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/RoomSlotB_8018FC54.data.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/room_m013_theater/RoomSlotC_8018FC54.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/D_8019F1E0.data.s.o(.data);
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/room_m013_theater/D_8019F1EC.s.o(.data);
        . = ALIGN(., 4);
        room_m013_theater_DATA_END = .;
        room_m013_theater_DATA_SIZE = ABSOLUTE(room_m013_theater_DATA_END - room_m013_theater_DATA_START);
        room_m013_theater_BSS_START = .;
        build/JP/overlays/room_m013_theater/asm/JP/overlays/room_m013_theater/data/room_m013_header.rodata.s.o(.bss);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m013_theater/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m013_theater_BSS_END = .;
        room_m013_theater_BSS_SIZE = ABSOLUTE(room_m013_theater_BSS_END - room_m013_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m013_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m013_theater_ROM_END = __romPos;
    room_m013_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
