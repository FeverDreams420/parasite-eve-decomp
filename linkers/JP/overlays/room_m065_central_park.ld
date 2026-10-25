SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m065_central_park_ROM_START = __romPos;
    room_m065_central_park_VRAM = ADDR(.room_m065_central_park);
    .room_m065_central_park (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m065_central_park_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m065_central_park_RODATA_START = .;
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/room_m065_central_park.rodata.s.o(.rodata);
        build/JP/overlays/room_m065_central_park/src/jp/overlays/room_lib/three_particles.c.o(.rodata);
        . = ALIGN(., 4);
        room_m065_central_park_RODATA_END = .;
        room_m065_central_park_RODATA_SIZE = ABSOLUTE(room_m065_central_park_RODATA_END - room_m065_central_park_RODATA_START);
        room_m065_central_park_TEXT_START = .;
        build/JP/overlays/room_m065_central_park/src/jp/overlays/room_lib/three_particles.c.o(.text);
        . = ALIGN(., 4);
        room_m065_central_park_TEXT_END = .;
        room_m065_central_park_TEXT_SIZE = ABSOLUTE(room_m065_central_park_TEXT_END - room_m065_central_park_TEXT_START);
        room_m065_central_park_DATA_START = .;
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/room_m065_central_park.rodata.s.o(.data);
        build/JP/overlays/room_m065_central_park/src/jp/overlays/room_lib/three_particles.c.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_8018FFF8.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/RoomLib_TableA_8018F054.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190010.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_8019001C.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190024.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190040.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190078.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190088.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_8019008C.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190090.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_80190098.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_8019009C.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/room_m065_central_park/D_801900A0.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801A7588.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/room_m065_central_park/D_801A7828.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801ADFB8.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801ADFF8.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801AE018.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801AE118.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801AE198.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801AE258.data.s.o(.data);
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/D_801AE2B8.data.s.o(.data);
        . = ALIGN(., 4);
        room_m065_central_park_DATA_END = .;
        room_m065_central_park_DATA_SIZE = ABSOLUTE(room_m065_central_park_DATA_END - room_m065_central_park_DATA_START);
        room_m065_central_park_BSS_START = .;
        build/JP/overlays/room_m065_central_park/asm/JP/overlays/room_m065_central_park/data/room_m065_central_park.rodata.s.o(.bss);
        build/JP/overlays/room_m065_central_park/src/jp/overlays/room_lib/three_particles.c.o(.bss);
        . = ALIGN(., 4);
        room_m065_central_park_BSS_END = .;
        room_m065_central_park_BSS_SIZE = ABSOLUTE(room_m065_central_park_BSS_END - room_m065_central_park_BSS_START);
    }
    __romPos += SIZEOF(.room_m065_central_park);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m065_central_park_ROM_END = __romPos;
    room_m065_central_park_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
