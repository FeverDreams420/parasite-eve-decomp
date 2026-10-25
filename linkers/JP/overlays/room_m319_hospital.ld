SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m319_hospital_ROM_START = __romPos;
    room_m319_hospital_VRAM = ADDR(.room_m319_hospital);
    .room_m319_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m319_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m319_hospital_RODATA_START = .;
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital.rodata.s.o(.rodata);
        build/JP/overlays/room_m319_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.rodata);
        . = ALIGN(., 4);
        room_m319_hospital_RODATA_END = .;
        room_m319_hospital_RODATA_SIZE = ABSOLUTE(room_m319_hospital_RODATA_END - room_m319_hospital_RODATA_START);
        room_m319_hospital_TEXT_START = .;
        build/JP/overlays/room_m319_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.text);
        . = ALIGN(., 4);
        room_m319_hospital_TEXT_END = .;
        room_m319_hospital_TEXT_SIZE = ABSOLUTE(room_m319_hospital_TEXT_END - room_m319_hospital_TEXT_START);
        room_m319_hospital_DATA_START = .;
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital.rodata.s.o(.data);
        build/JP/overlays/room_m319_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.data);
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital/D_8018FB80.s.o(.data);
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital/D_801981F8.s.o(.data);
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/D_801AD0A8.data.s.o(.data);
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital/D_801AD0B4.s.o(.data);
        . = ALIGN(., 4);
        room_m319_hospital_DATA_END = .;
        room_m319_hospital_DATA_SIZE = ABSOLUTE(room_m319_hospital_DATA_END - room_m319_hospital_DATA_START);
        room_m319_hospital_BSS_START = .;
        build/JP/overlays/room_m319_hospital/asm/JP/overlays/room_m319_hospital/data/room_m319_hospital.rodata.s.o(.bss);
        build/JP/overlays/room_m319_hospital/src/jp/overlays/room_lib/mirror_model.c.o(.bss);
        . = ALIGN(., 4);
        room_m319_hospital_BSS_END = .;
        room_m319_hospital_BSS_SIZE = ABSOLUTE(room_m319_hospital_BSS_END - room_m319_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m319_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m319_hospital_ROM_END = __romPos;
    room_m319_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
