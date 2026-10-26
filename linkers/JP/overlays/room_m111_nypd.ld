SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m111_nypd_ROM_START = __romPos;
    room_m111_nypd_VRAM = ADDR(.room_m111_nypd);
    .room_m111_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m111_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m111_nypd_RODATA_START = .;
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/room_m111_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.rodata);
        . = ALIGN(., 4);
        room_m111_nypd_RODATA_END = .;
        room_m111_nypd_RODATA_SIZE = ABSOLUTE(room_m111_nypd_RODATA_END - room_m111_nypd_RODATA_START);
        room_m111_nypd_TEXT_START = .;
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.text);
        . = ALIGN(., 4);
        room_m111_nypd_TEXT_END = .;
        room_m111_nypd_TEXT_SIZE = ABSOLUTE(room_m111_nypd_TEXT_END - room_m111_nypd_TEXT_START);
        room_m111_nypd_DATA_START = .;
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/room_m111_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_80190094.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/RoomLib_TableA_8018F054.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_801900C4.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_801900DC.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_801900E8.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_80190104.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_8019016C.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_80190188.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_8019018C.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_80190190.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/room_m111_nypd/D_80190194.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/D_801C0888.data.s.o(.data);
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/room_m111_nypd/D_801C0894.s.o(.data);
        . = ALIGN(., 4);
        room_m111_nypd_DATA_END = .;
        room_m111_nypd_DATA_SIZE = ABSOLUTE(room_m111_nypd_DATA_END - room_m111_nypd_DATA_START);
        room_m111_nypd_BSS_START = .;
        build/JP/overlays/room_m111_nypd/asm/JP/overlays/room_m111_nypd/data/room_m111_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m111_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.bss);
        . = ALIGN(., 4);
        room_m111_nypd_BSS_END = .;
        room_m111_nypd_BSS_SIZE = ABSOLUTE(room_m111_nypd_BSS_END - room_m111_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m111_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m111_nypd_ROM_END = __romPos;
    room_m111_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
