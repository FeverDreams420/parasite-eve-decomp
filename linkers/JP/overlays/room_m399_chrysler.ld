SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m399_chrysler_ROM_START = __romPos;
    room_m399_chrysler_VRAM = ADDR(.room_m399_chrysler);
    .room_m399_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m399_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m399_chrysler_RODATA_START = .;
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/room_m399_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.rodata);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/timed_spawn.c.o(.rodata);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/floor_walker_class.c.o(.rodata);
        . = ALIGN(., 4);
        room_m399_chrysler_RODATA_END = .;
        room_m399_chrysler_RODATA_SIZE = ABSOLUTE(room_m399_chrysler_RODATA_END - room_m399_chrysler_RODATA_START);
        room_m399_chrysler_TEXT_START = .;
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.text);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/floor_walker_class.c.o(.text);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/timed_spawn.c.o(.text);
        . = ALIGN(., 4);
        room_m399_chrysler_TEXT_END = .;
        room_m399_chrysler_TEXT_SIZE = ABSOLUTE(room_m399_chrysler_TEXT_END - room_m399_chrysler_TEXT_START);
        room_m399_chrysler_DATA_START = .;
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/room_m399_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.data);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/timed_spawn.c.o(.data);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/floor_walker_class.c.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801941C0.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/RoomLib_TableA_8018F234.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801941D8.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801941E4.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801941EC.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194208.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194240.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801942F8.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194314.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194334.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194354.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194390.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194394.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_80194398.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801943A0.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801943A4.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/room_m399_chrysler/D_801943A8.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/D_801C2248.data.s.o(.data);
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/room_m399_chrysler/D_801C2278.s.o(.data);
        . = ALIGN(., 4);
        room_m399_chrysler_DATA_END = .;
        room_m399_chrysler_DATA_SIZE = ABSOLUTE(room_m399_chrysler_DATA_END - room_m399_chrysler_DATA_START);
        room_m399_chrysler_BSS_START = .;
        build/JP/overlays/room_m399_chrysler/asm/JP/overlays/room_m399_chrysler/data/room_m399_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.bss);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/timed_spawn.c.o(.bss);
        build/JP/overlays/room_m399_chrysler/src-jp/overlays/room_lib/floor_walker_class.c.o(.bss);
        . = ALIGN(., 4);
        room_m399_chrysler_BSS_END = .;
        room_m399_chrysler_BSS_SIZE = ABSOLUTE(room_m399_chrysler_BSS_END - room_m399_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m399_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m399_chrysler_ROM_END = __romPos;
    room_m399_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
