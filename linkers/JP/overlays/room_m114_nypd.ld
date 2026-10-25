SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m114_nypd_ROM_START = __romPos;
    room_m114_nypd_VRAM = ADDR(.room_m114_nypd);
    .room_m114_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m114_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m114_nypd_RODATA_START = .;
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/room_m114_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/paired_glow.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/RoomLib_HandlerCArgsData_8019263C.rodata.s.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_close_target.c.o(.rodata);
        . = ALIGN(., 4);
        room_m114_nypd_RODATA_END = .;
        room_m114_nypd_RODATA_SIZE = ABSOLUTE(room_m114_nypd_RODATA_END - room_m114_nypd_RODATA_START);
        room_m114_nypd_TEXT_START = .;
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/paired_glow.c.o(.text);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m114_nypd_TEXT_END = .;
        room_m114_nypd_TEXT_SIZE = ABSOLUTE(room_m114_nypd_TEXT_END - room_m114_nypd_TEXT_START);
        room_m114_nypd_DATA_START = .;
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/room_m114_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/paired_glow.c.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/RoomLib_HandlerCArgsData_8019263C.rodata.s.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_80193588.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/RoomLib_TableA_8018F22C.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_801935B8.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_801935D0.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_801935DC.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_801935F8.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_80193660.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_80193720.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_80193724.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_80193728.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/room_m114_nypd/D_8019372C.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/D_801B5B14.data.s.o(.data);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/room_m114_nypd/D_801B5B2C.s.o(.data);
        . = ALIGN(., 4);
        room_m114_nypd_DATA_END = .;
        room_m114_nypd_DATA_SIZE = ABSOLUTE(room_m114_nypd_DATA_END - room_m114_nypd_DATA_START);
        room_m114_nypd_BSS_START = .;
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/room_m114_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/paired_glow.c.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m114_nypd/asm/JP/overlays/room_m114_nypd/data/RoomLib_HandlerCArgsData_8019263C.rodata.s.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m114_nypd/src/jp/overlays/room_lib/class_close_target.c.o(.bss);
        . = ALIGN(., 4);
        room_m114_nypd_BSS_END = .;
        room_m114_nypd_BSS_SIZE = ABSOLUTE(room_m114_nypd_BSS_END - room_m114_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m114_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m114_nypd_ROM_END = __romPos;
    room_m114_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
