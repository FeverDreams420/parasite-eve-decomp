SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m141_hospital_ROM_START = __romPos;
    room_m141_hospital_VRAM = ADDR(.room_m141_hospital);
    .room_m141_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m141_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m141_hospital_RODATA_START = .;
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/room_m141_hospital.rodata.s.o(.rodata);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/ground_eruption.c.o(.rodata);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/homing_projectile.c.o(.rodata);
        . = ALIGN(., 4);
        room_m141_hospital_RODATA_END = .;
        room_m141_hospital_RODATA_SIZE = ABSOLUTE(room_m141_hospital_RODATA_END - room_m141_hospital_RODATA_START);
        room_m141_hospital_TEXT_START = .;
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/ground_eruption.c.o(.text);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.text);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/homing_projectile.c.o(.text);
        . = ALIGN(., 4);
        room_m141_hospital_TEXT_END = .;
        room_m141_hospital_TEXT_SIZE = ABSOLUTE(room_m141_hospital_TEXT_END - room_m141_hospital_TEXT_START);
        room_m141_hospital_DATA_START = .;
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/room_m141_hospital.rodata.s.o(.data);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/ground_eruption.c.o(.data);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/homing_projectile.c.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80191FDC.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/RoomLib_TableC.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80192014.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80192030.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80192040.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/RoomLib_TableB_8018F090.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801920A4.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801920C8.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80192100.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/homing_projectile_seeds.c.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_80192140.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/RoomLib_PairB.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/g_RoomHomingLaunch.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921A0.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921A4.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921A8.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921B0.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921B4.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921B8.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921C0.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921C4.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921C8.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921D0.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921D4.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801921D8.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/room_m141_hospital/D_801981F8.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/D_801CD0A0.data.s.o(.data);
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/room_m141_hospital/D_801CD0C4.s.o(.data);
        . = ALIGN(., 4);
        room_m141_hospital_DATA_END = .;
        room_m141_hospital_DATA_SIZE = ABSOLUTE(room_m141_hospital_DATA_END - room_m141_hospital_DATA_START);
        room_m141_hospital_BSS_START = .;
        build/JP/overlays/room_m141_hospital/asm/JP/overlays/room_m141_hospital/data/room_m141_hospital.rodata.s.o(.bss);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/ground_eruption.c.o(.bss);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.bss);
        build/JP/overlays/room_m141_hospital/src-jp/overlays/room_lib/homing_projectile.c.o(.bss);
        . = ALIGN(., 4);
        room_m141_hospital_BSS_END = .;
        room_m141_hospital_BSS_SIZE = ABSOLUTE(room_m141_hospital_BSS_END - room_m141_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m141_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m141_hospital_ROM_END = __romPos;
    room_m141_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
