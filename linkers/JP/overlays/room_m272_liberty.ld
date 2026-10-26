SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m272_liberty_ROM_START = __romPos;
    room_m272_liberty_VRAM = ADDR(.room_m272_liberty);
    .room_m272_liberty (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m272_liberty_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m272_liberty_RODATA_START = .;
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/room_m272_liberty.rodata.s.o(.rodata);
        build/JP/overlays/room_m272_liberty/src-jp/overlays/room_m272_liberty/m272.c.o(.rodata);
        . = ALIGN(., 4);
        room_m272_liberty_RODATA_END = .;
        room_m272_liberty_RODATA_SIZE = ABSOLUTE(room_m272_liberty_RODATA_END - room_m272_liberty_RODATA_START);
        room_m272_liberty_TEXT_START = .;
        build/JP/overlays/room_m272_liberty/src-jp/overlays/room_m272_liberty/m272.c.o(.text);
        . = ALIGN(., 4);
        room_m272_liberty_TEXT_END = .;
        room_m272_liberty_TEXT_SIZE = ABSOLUTE(room_m272_liberty_TEXT_END - room_m272_liberty_TEXT_START);
        room_m272_liberty_DATA_START = .;
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/room_m272_liberty.rodata.s.o(.data);
        build/JP/overlays/room_m272_liberty/src-jp/overlays/room_m272_liberty/m272.c.o(.data);
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/D_8018F1A8.data.s.o(.data);
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/room_m272_liberty/RoomM272_Rec.s.o(.data);
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/room_m272_liberty/D_801991C8.s.o(.data);
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/D_801D01F4.data.s.o(.data);
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/D_801D01F8.data.s.o(.data);
        . = ALIGN(., 4);
        room_m272_liberty_DATA_END = .;
        room_m272_liberty_DATA_SIZE = ABSOLUTE(room_m272_liberty_DATA_END - room_m272_liberty_DATA_START);
        room_m272_liberty_BSS_START = .;
        build/JP/overlays/room_m272_liberty/asm/JP/overlays/room_m272_liberty/data/room_m272_liberty.rodata.s.o(.bss);
        build/JP/overlays/room_m272_liberty/src-jp/overlays/room_m272_liberty/m272.c.o(.bss);
        . = ALIGN(., 4);
        room_m272_liberty_BSS_END = .;
        room_m272_liberty_BSS_SIZE = ABSOLUTE(room_m272_liberty_BSS_END - room_m272_liberty_BSS_START);
    }
    __romPos += SIZEOF(.room_m272_liberty);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m272_liberty_ROM_END = __romPos;
    room_m272_liberty_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
