SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m122_nypd_ROM_START = __romPos;
    room_m122_nypd_VRAM = ADDR(.room_m122_nypd);
    .room_m122_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m122_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m122_nypd_RODATA_START = .;
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/room_m122_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.rodata);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m122_nypd_RODATA_END = .;
        room_m122_nypd_RODATA_SIZE = ABSOLUTE(room_m122_nypd_RODATA_END - room_m122_nypd_RODATA_START);
        room_m122_nypd_TEXT_START = .;
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m122_nypd_TEXT_END = .;
        room_m122_nypd_TEXT_SIZE = ABSOLUTE(room_m122_nypd_TEXT_END - room_m122_nypd_TEXT_START);
        room_m122_nypd_DATA_START = .;
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/room_m122_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190EDC.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/RoomLib_TableA_8018F068.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190F0C.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190F24.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190F30.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190F4C.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190FB4.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190FCC.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_80190FEC.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/RoomLib_SlotTable.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_801910A8.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_801910AC.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_801910B0.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/room_m122_nypd/D_801910B4.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/D_801BBF3C.data.s.o(.data);
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/room_m122_nypd/D_801BBF54.s.o(.data);
        . = ALIGN(., 4);
        room_m122_nypd_DATA_END = .;
        room_m122_nypd_DATA_SIZE = ABSOLUTE(room_m122_nypd_DATA_END - room_m122_nypd_DATA_START);
        room_m122_nypd_BSS_START = .;
        build/JP/overlays/room_m122_nypd/asm/JP/overlays/room_m122_nypd/data/room_m122_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/paired_glow.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/class_close_target.c.o(.bss);
        build/JP/overlays/room_m122_nypd/src-jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m122_nypd_BSS_END = .;
        room_m122_nypd_BSS_SIZE = ABSOLUTE(room_m122_nypd_BSS_END - room_m122_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m122_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m122_nypd_ROM_END = __romPos;
    room_m122_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
