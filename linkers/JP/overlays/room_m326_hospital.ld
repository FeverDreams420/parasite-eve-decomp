SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m326_hospital_ROM_START = __romPos;
    room_m326_hospital_VRAM = ADDR(.room_m326_hospital);
    .room_m326_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m326_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m326_hospital_RODATA_START = .;
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/room_m326_0_C.rodata.s.o(.rodata);
        build/JP/overlays/room_m326_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        . = ALIGN(., 4);
        room_m326_hospital_RODATA_END = .;
        room_m326_hospital_RODATA_SIZE = ABSOLUTE(room_m326_hospital_RODATA_END - room_m326_hospital_RODATA_START);
        room_m326_hospital_TEXT_START = .;
        build/JP/overlays/room_m326_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.text);
        . = ALIGN(., 4);
        room_m326_hospital_TEXT_END = .;
        room_m326_hospital_TEXT_SIZE = ABSOLUTE(room_m326_hospital_TEXT_END - room_m326_hospital_TEXT_START);
        room_m326_hospital_DATA_START = .;
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/room_m326_0_C.rodata.s.o(.data);
        build/JP/overlays/room_m326_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/D_8018FCDC.data.s.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/D_8018FD00.data.s.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/room_m326_hospital/RoomLib_PairB.s.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/D_801AF598.data.s.o(.data);
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/room_m326_hospital/D_801AF5A4.s.o(.data);
        . = ALIGN(., 4);
        room_m326_hospital_DATA_END = .;
        room_m326_hospital_DATA_SIZE = ABSOLUTE(room_m326_hospital_DATA_END - room_m326_hospital_DATA_START);
        room_m326_hospital_BSS_START = .;
        build/JP/overlays/room_m326_hospital/asm/JP/overlays/room_m326_hospital/data/room_m326_0_C.rodata.s.o(.bss);
        build/JP/overlays/room_m326_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.bss);
        . = ALIGN(., 4);
        room_m326_hospital_BSS_END = .;
        room_m326_hospital_BSS_SIZE = ABSOLUTE(room_m326_hospital_BSS_END - room_m326_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m326_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m326_hospital_ROM_END = __romPos;
    room_m326_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
