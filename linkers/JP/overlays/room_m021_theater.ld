SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m021_theater_ROM_START = __romPos;
    room_m021_theater_VRAM = ADDR(.room_m021_theater);
    .room_m021_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m021_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m021_theater_RODATA_START = .;
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/room_m021_theater.rodata.s.o(.rodata);
        build/JP/overlays/room_m021_theater/src/jp/overlays/room_lib/mirror_model.c.o(.rodata);
        . = ALIGN(., 4);
        room_m021_theater_RODATA_END = .;
        room_m021_theater_RODATA_SIZE = ABSOLUTE(room_m021_theater_RODATA_END - room_m021_theater_RODATA_START);
        room_m021_theater_TEXT_START = .;
        build/JP/overlays/room_m021_theater/src/jp/overlays/room_lib/mirror_model.c.o(.text);
        . = ALIGN(., 4);
        room_m021_theater_TEXT_END = .;
        room_m021_theater_TEXT_SIZE = ABSOLUTE(room_m021_theater_TEXT_END - room_m021_theater_TEXT_START);
        room_m021_theater_DATA_START = .;
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/room_m021_theater.rodata.s.o(.data);
        build/JP/overlays/room_m021_theater/src/jp/overlays/room_lib/mirror_model.c.o(.data);
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/room_m021_theater/D_8018FB80.s.o(.data);
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/D_801AB600.data.s.o(.data);
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/room_m021_theater/D_801AB60C.s.o(.data);
        . = ALIGN(., 4);
        room_m021_theater_DATA_END = .;
        room_m021_theater_DATA_SIZE = ABSOLUTE(room_m021_theater_DATA_END - room_m021_theater_DATA_START);
        room_m021_theater_BSS_START = .;
        build/JP/overlays/room_m021_theater/asm/JP/overlays/room_m021_theater/data/room_m021_theater.rodata.s.o(.bss);
        build/JP/overlays/room_m021_theater/src/jp/overlays/room_lib/mirror_model.c.o(.bss);
        . = ALIGN(., 4);
        room_m021_theater_BSS_END = .;
        room_m021_theater_BSS_SIZE = ABSOLUTE(room_m021_theater_BSS_END - room_m021_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m021_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m021_theater_ROM_END = __romPos;
    room_m021_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
