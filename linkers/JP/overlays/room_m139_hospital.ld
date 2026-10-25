SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m139_hospital_ROM_START = __romPos;
    room_m139_hospital_VRAM = ADDR(.room_m139_hospital);
    .room_m139_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m139_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m139_hospital_RODATA_START = .;
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/room_m139_0_C.rodata.s.o(.rodata);
        build/JP/overlays/room_m139_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        . = ALIGN(., 4);
        room_m139_hospital_RODATA_END = .;
        room_m139_hospital_RODATA_SIZE = ABSOLUTE(room_m139_hospital_RODATA_END - room_m139_hospital_RODATA_START);
        room_m139_hospital_TEXT_START = .;
        build/JP/overlays/room_m139_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.text);
        . = ALIGN(., 4);
        room_m139_hospital_TEXT_END = .;
        room_m139_hospital_TEXT_SIZE = ABSOLUTE(room_m139_hospital_TEXT_END - room_m139_hospital_TEXT_START);
        room_m139_hospital_DATA_START = .;
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/room_m139_0_C.rodata.s.o(.data);
        build/JP/overlays/room_m139_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/D_8018FCDC.data.s.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/D_8018FD00.data.s.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/room_m139_hospital/RoomLib_PairB.s.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/D_801B0400.data.s.o(.data);
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/room_m139_hospital/D_801B040C.s.o(.data);
        . = ALIGN(., 4);
        room_m139_hospital_DATA_END = .;
        room_m139_hospital_DATA_SIZE = ABSOLUTE(room_m139_hospital_DATA_END - room_m139_hospital_DATA_START);
        room_m139_hospital_BSS_START = .;
        build/JP/overlays/room_m139_hospital/asm/JP/overlays/room_m139_hospital/data/room_m139_0_C.rodata.s.o(.bss);
        build/JP/overlays/room_m139_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.bss);
        . = ALIGN(., 4);
        room_m139_hospital_BSS_END = .;
        room_m139_hospital_BSS_SIZE = ABSOLUTE(room_m139_hospital_BSS_END - room_m139_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m139_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m139_hospital_ROM_END = __romPos;
    room_m139_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
