SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m152_hospital_ROM_START = __romPos;
    room_m152_hospital_VRAM = ADDR(.room_m152_hospital);
    .room_m152_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m152_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m152_hospital_RODATA_START = .;
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_0_C.rodata.s.o(.rodata);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.rodata);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.rodata);
        . = ALIGN(., 4);
        room_m152_hospital_RODATA_END = .;
        room_m152_hospital_RODATA_SIZE = ABSOLUTE(room_m152_hospital_RODATA_END - room_m152_hospital_RODATA_START);
        room_m152_hospital_TEXT_START = .;
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.text);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.text);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.text);
        . = ALIGN(., 4);
        room_m152_hospital_TEXT_END = .;
        room_m152_hospital_TEXT_SIZE = ABSOLUTE(room_m152_hospital_TEXT_END - room_m152_hospital_TEXT_START);
        room_m152_hospital_DATA_START = .;
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_0_C.rodata.s.o(.data);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.data);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.data);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80193F4C.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80193F68.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80193F8C.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80193FC4.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80193FE4.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_80194004.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/RoomLib_PairB.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_hospital/D_801981E8.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_801C01E8.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_hospital/D_801C01F8.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/D_801CE68C.data.s.o(.data);
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_hospital/D_801CE6BC.s.o(.data);
        . = ALIGN(., 4);
        room_m152_hospital_DATA_END = .;
        room_m152_hospital_DATA_SIZE = ABSOLUTE(room_m152_hospital_DATA_END - room_m152_hospital_DATA_START);
        room_m152_hospital_BSS_START = .;
        build/JP/overlays/room_m152_hospital/asm/JP/overlays/room_m152_hospital/data/room_m152_0_C.rodata.s.o(.bss);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/dropped_flare.c.o(.bss);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.bss);
        build/JP/overlays/room_m152_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.bss);
        . = ALIGN(., 4);
        room_m152_hospital_BSS_END = .;
        room_m152_hospital_BSS_SIZE = ABSOLUTE(room_m152_hospital_BSS_END - room_m152_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m152_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m152_hospital_ROM_END = __romPos;
    room_m152_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
