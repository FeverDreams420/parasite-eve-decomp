SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m018_theater_ROM_START = __romPos;
    room_m018_theater_VRAM = ADDR(.room_m018_theater);
    .room_m018_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m018_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m018_theater_RODATA_START = .;
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/room_m018_theater.rodata.s.o(.rodata);
        build/JP/overlays/room_m018_theater/src/jp/overlays/room_lib/mirror_model.c.o(.rodata);
        . = ALIGN(., 4);
        room_m018_theater_RODATA_END = .;
        room_m018_theater_RODATA_SIZE = ABSOLUTE(room_m018_theater_RODATA_END - room_m018_theater_RODATA_START);
        room_m018_theater_TEXT_START = .;
        build/JP/overlays/room_m018_theater/src/jp/overlays/room_lib/mirror_model.c.o(.text);
        . = ALIGN(., 4);
        room_m018_theater_TEXT_END = .;
        room_m018_theater_TEXT_SIZE = ABSOLUTE(room_m018_theater_TEXT_END - room_m018_theater_TEXT_START);
        room_m018_theater_DATA_START = .;
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/room_m018_theater.rodata.s.o(.data);
        build/JP/overlays/room_m018_theater/src/jp/overlays/room_lib/mirror_model.c.o(.data);
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/room_m018_theater/D_8018FB80.s.o(.data);
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/D_801A6D18.data.s.o(.data);
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/room_m018_theater/D_801A6D24.s.o(.data);
        . = ALIGN(., 4);
        room_m018_theater_DATA_END = .;
        room_m018_theater_DATA_SIZE = ABSOLUTE(room_m018_theater_DATA_END - room_m018_theater_DATA_START);
        room_m018_theater_BSS_START = .;
        build/JP/overlays/room_m018_theater/asm/JP/overlays/room_m018_theater/data/room_m018_theater.rodata.s.o(.bss);
        build/JP/overlays/room_m018_theater/src/jp/overlays/room_lib/mirror_model.c.o(.bss);
        . = ALIGN(., 4);
        room_m018_theater_BSS_END = .;
        room_m018_theater_BSS_SIZE = ABSOLUTE(room_m018_theater_BSS_END - room_m018_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m018_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m018_theater_ROM_END = __romPos;
    room_m018_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
