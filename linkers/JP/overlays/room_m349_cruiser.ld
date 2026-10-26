SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m349_cruiser_ROM_START = __romPos;
    room_m349_cruiser_VRAM = ADDR(.room_m349_cruiser);
    .room_m349_cruiser (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m349_cruiser_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m349_cruiser_RODATA_START = .;
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m349_cruiser/src-jp/overlays/room_m349_cruiser/m349.c.o(.rodata);
        . = ALIGN(., 4);
        room_m349_cruiser_RODATA_END = .;
        room_m349_cruiser_RODATA_SIZE = ABSOLUTE(room_m349_cruiser_RODATA_END - room_m349_cruiser_RODATA_START);
        room_m349_cruiser_TEXT_START = .;
        build/JP/overlays/room_m349_cruiser/src-jp/overlays/room_m349_cruiser/m349.c.o(.text);
        . = ALIGN(., 4);
        room_m349_cruiser_TEXT_END = .;
        room_m349_cruiser_TEXT_SIZE = ABSOLUTE(room_m349_cruiser_TEXT_END - room_m349_cruiser_TEXT_START);
        room_m349_cruiser_DATA_START = .;
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_header.rodata.s.o(.data);
        build/JP/overlays/room_m349_cruiser/src-jp/overlays/room_m349_cruiser/m349.c.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_80190138.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_8019015C.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_80190194.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_80190198.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_8019019C.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_801901A0.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_cruiser/D_801901A4.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_cruiser/D_801C01E0.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/D_801C88B0.data.s.o(.data);
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_cruiser/D_801C88BC.s.o(.data);
        . = ALIGN(., 4);
        room_m349_cruiser_DATA_END = .;
        room_m349_cruiser_DATA_SIZE = ABSOLUTE(room_m349_cruiser_DATA_END - room_m349_cruiser_DATA_START);
        room_m349_cruiser_BSS_START = .;
        build/JP/overlays/room_m349_cruiser/asm/JP/overlays/room_m349_cruiser/data/room_m349_header.rodata.s.o(.bss);
        build/JP/overlays/room_m349_cruiser/src-jp/overlays/room_m349_cruiser/m349.c.o(.bss);
        . = ALIGN(., 4);
        room_m349_cruiser_BSS_END = .;
        room_m349_cruiser_BSS_SIZE = ABSOLUTE(room_m349_cruiser_BSS_END - room_m349_cruiser_BSS_START);
    }
    __romPos += SIZEOF(.room_m349_cruiser);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m349_cruiser_ROM_END = __romPos;
    room_m349_cruiser_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
