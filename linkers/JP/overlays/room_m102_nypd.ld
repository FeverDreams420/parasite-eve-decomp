SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m102_nypd_ROM_START = __romPos;
    room_m102_nypd_VRAM = ADDR(.room_m102_nypd);
    .room_m102_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m102_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m102_nypd_RODATA_START = .;
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/room_m102_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m102_nypd/src/jp/overlays/room_lib/mirror_model.c.o(.rodata);
        . = ALIGN(., 4);
        room_m102_nypd_RODATA_END = .;
        room_m102_nypd_RODATA_SIZE = ABSOLUTE(room_m102_nypd_RODATA_END - room_m102_nypd_RODATA_START);
        room_m102_nypd_TEXT_START = .;
        build/JP/overlays/room_m102_nypd/src/jp/overlays/room_lib/mirror_model.c.o(.text);
        . = ALIGN(., 4);
        room_m102_nypd_TEXT_END = .;
        room_m102_nypd_TEXT_SIZE = ABSOLUTE(room_m102_nypd_TEXT_END - room_m102_nypd_TEXT_START);
        room_m102_nypd_DATA_START = .;
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/room_m102_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m102_nypd/src/jp/overlays/room_lib/mirror_model.c.o(.data);
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/room_m102_nypd/D_8018FB80.s.o(.data);
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/D_801AC554.data.s.o(.data);
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/room_m102_nypd/D_801AC560.s.o(.data);
        . = ALIGN(., 4);
        room_m102_nypd_DATA_END = .;
        room_m102_nypd_DATA_SIZE = ABSOLUTE(room_m102_nypd_DATA_END - room_m102_nypd_DATA_START);
        room_m102_nypd_BSS_START = .;
        build/JP/overlays/room_m102_nypd/asm/JP/overlays/room_m102_nypd/data/room_m102_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m102_nypd/src/jp/overlays/room_lib/mirror_model.c.o(.bss);
        . = ALIGN(., 4);
        room_m102_nypd_BSS_END = .;
        room_m102_nypd_BSS_SIZE = ABSOLUTE(room_m102_nypd_BSS_END - room_m102_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m102_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m102_nypd_ROM_END = __romPos;
    room_m102_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
