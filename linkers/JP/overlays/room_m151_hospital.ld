SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m151_hospital_ROM_START = __romPos;
    room_m151_hospital_VRAM = ADDR(.room_m151_hospital);
    .room_m151_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m151_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m151_hospital_RODATA_START = .;
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/room_m151_hospital.rodata.s.o(.rodata);
        build/JP/overlays/room_m151_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.rodata);
        . = ALIGN(., 4);
        room_m151_hospital_RODATA_END = .;
        room_m151_hospital_RODATA_SIZE = ABSOLUTE(room_m151_hospital_RODATA_END - room_m151_hospital_RODATA_START);
        room_m151_hospital_TEXT_START = .;
        build/JP/overlays/room_m151_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.text);
        . = ALIGN(., 4);
        room_m151_hospital_TEXT_END = .;
        room_m151_hospital_TEXT_SIZE = ABSOLUTE(room_m151_hospital_TEXT_END - room_m151_hospital_TEXT_START);
        room_m151_hospital_DATA_START = .;
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/room_m151_hospital.rodata.s.o(.data);
        build/JP/overlays/room_m151_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.data);
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/room_m151_hospital/D_8018FB80.s.o(.data);
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/D_801A0120.data.s.o(.data);
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/room_m151_hospital/D_801A012C.s.o(.data);
        . = ALIGN(., 4);
        room_m151_hospital_DATA_END = .;
        room_m151_hospital_DATA_SIZE = ABSOLUTE(room_m151_hospital_DATA_END - room_m151_hospital_DATA_START);
        room_m151_hospital_BSS_START = .;
        build/JP/overlays/room_m151_hospital/asm/JP/overlays/room_m151_hospital/data/room_m151_hospital.rodata.s.o(.bss);
        build/JP/overlays/room_m151_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.bss);
        . = ALIGN(., 4);
        room_m151_hospital_BSS_END = .;
        room_m151_hospital_BSS_SIZE = ABSOLUTE(room_m151_hospital_BSS_END - room_m151_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m151_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m151_hospital_ROM_END = __romPos;
    room_m151_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
