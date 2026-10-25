SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m154_hospital_ROM_START = __romPos;
    room_m154_hospital_VRAM = ADDR(.room_m154_hospital);
    .room_m154_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m154_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m154_hospital_RODATA_START = .;
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital.rodata.s.o(.rodata);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/ground_eruption.c.o(.rodata);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.rodata);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.rodata);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/homing_projectile.c.o(.rodata);
        . = ALIGN(., 4);
        room_m154_hospital_RODATA_END = .;
        room_m154_hospital_RODATA_SIZE = ABSOLUTE(room_m154_hospital_RODATA_END - room_m154_hospital_RODATA_START);
        room_m154_hospital_TEXT_START = .;
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/ground_eruption.c.o(.text);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.text);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.text);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/homing_projectile.c.o(.text);
        . = ALIGN(., 4);
        room_m154_hospital_TEXT_END = .;
        room_m154_hospital_TEXT_SIZE = ABSOLUTE(room_m154_hospital_TEXT_END - room_m154_hospital_TEXT_START);
        room_m154_hospital_DATA_START = .;
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital.rodata.s.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/ground_eruption.c.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/homing_projectile.c.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_801954BC.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/RoomLib_TableC.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_801954F4.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195510.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195520.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/RoomLib_TableB_8018F22C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_8019562C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195648.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195668.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195688.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_801956C0.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/homing_projectile_seeds.c.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195700.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/g_RoomHomingLaunch.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195758.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_8019575C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195760.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195768.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_8019576C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195770.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195778.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_8019577C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195780.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195788.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_8019578C.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_80195790.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital/D_801981EC.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital/D_801C01E4.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_801D01EC.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital/D_801D01F8.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/D_801DD2E0.data.s.o(.data);
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital/D_801DD31C.s.o(.data);
        . = ALIGN(., 4);
        room_m154_hospital_DATA_END = .;
        room_m154_hospital_DATA_SIZE = ABSOLUTE(room_m154_hospital_DATA_END - room_m154_hospital_DATA_START);
        room_m154_hospital_BSS_START = .;
        build/JP/overlays/room_m154_hospital/asm/JP/overlays/room_m154_hospital/data/room_m154_hospital.rodata.s.o(.bss);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/ground_eruption.c.o(.bss);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/timed_spawn.c.o(.bss);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/floor_walker_class.c.o(.bss);
        build/JP/overlays/room_m154_hospital/src/jp/overlays/room_lib/homing_projectile.c.o(.bss);
        . = ALIGN(., 4);
        room_m154_hospital_BSS_END = .;
        room_m154_hospital_BSS_SIZE = ABSOLUTE(room_m154_hospital_BSS_END - room_m154_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m154_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m154_hospital_ROM_END = __romPos;
    room_m154_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
