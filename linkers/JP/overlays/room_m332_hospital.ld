SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m332_hospital_ROM_START = __romPos;
    room_m332_hospital_VRAM = ADDR(.room_m332_hospital);
    .room_m332_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m332_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m332_hospital_RODATA_START = .;
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital.rodata.s.o(.rodata);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/timed_spawn.c.o(.rodata);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/floor_walker_class.c.o(.rodata);
        . = ALIGN(., 4);
        room_m332_hospital_RODATA_END = .;
        room_m332_hospital_RODATA_SIZE = ABSOLUTE(room_m332_hospital_RODATA_END - room_m332_hospital_RODATA_START);
        room_m332_hospital_TEXT_START = .;
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/floor_walker_class.c.o(.text);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/timed_spawn.c.o(.text);
        . = ALIGN(., 4);
        room_m332_hospital_TEXT_END = .;
        room_m332_hospital_TEXT_SIZE = ABSOLUTE(room_m332_hospital_TEXT_END - room_m332_hospital_TEXT_START);
        room_m332_hospital_DATA_START = .;
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital.rodata.s.o(.data);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/timed_spawn.c.o(.data);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/floor_walker_class.c.o(.data);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/D_80193264.data.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/D_80193280.data.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/RoomLib_TimedRenderTable.data.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/D_801932C0.data.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital/D_801932F8.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital/D_801A4528.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/D_801BD07C.data.s.o(.data);
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital/D_801BD0A0.s.o(.data);
        . = ALIGN(., 4);
        room_m332_hospital_DATA_END = .;
        room_m332_hospital_DATA_SIZE = ABSOLUTE(room_m332_hospital_DATA_END - room_m332_hospital_DATA_START);
        room_m332_hospital_BSS_START = .;
        build/JP/overlays/room_m332_hospital/asm/JP/overlays/room_m332_hospital/data/room_m332_hospital.rodata.s.o(.bss);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/timed_spawn.c.o(.bss);
        build/JP/overlays/room_m332_hospital/src-jp/overlays/room_lib/floor_walker_class.c.o(.bss);
        . = ALIGN(., 4);
        room_m332_hospital_BSS_END = .;
        room_m332_hospital_BSS_SIZE = ABSOLUTE(room_m332_hospital_BSS_END - room_m332_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m332_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m332_hospital_ROM_END = __romPos;
    room_m332_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
