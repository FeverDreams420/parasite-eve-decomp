SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m178_sewer2_ROM_START = __romPos;
    room_m178_sewer2_VRAM = ADDR(.room_m178_sewer2);
    .room_m178_sewer2 (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m178_sewer2_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m178_sewer2_RODATA_START = .;
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/room_m178_sewer2.rodata.s.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/twelve_elements.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/RoomLib_HandlerCArgsData_80192238.rodata.s.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_close_target.c.o(.rodata);
        . = ALIGN(., 4);
        room_m178_sewer2_RODATA_END = .;
        room_m178_sewer2_RODATA_SIZE = ABSOLUTE(room_m178_sewer2_RODATA_END - room_m178_sewer2_RODATA_START);
        room_m178_sewer2_TEXT_START = .;
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/twelve_elements.c.o(.text);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m178_sewer2_TEXT_END = .;
        room_m178_sewer2_TEXT_SIZE = ABSOLUTE(room_m178_sewer2_TEXT_END - room_m178_sewer2_TEXT_START);
        room_m178_sewer2_DATA_START = .;
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/room_m178_sewer2.rodata.s.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/twelve_elements.c.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/RoomLib_HandlerCArgsData_80192238.rodata.s.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_80193184.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/RoomLib_TableA_8018F244.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_8019319C.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801931A8.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801931B0.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801931CC.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_80193208.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801932C0.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801932C4.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801932C8.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801932D0.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801932D4.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/room_m178_sewer2/D_801932D8.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/D_801A8D7C.data.s.o(.data);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/room_m178_sewer2/D_801A8DA0.s.o(.data);
        . = ALIGN(., 4);
        room_m178_sewer2_DATA_END = .;
        room_m178_sewer2_DATA_SIZE = ABSOLUTE(room_m178_sewer2_DATA_END - room_m178_sewer2_DATA_START);
        room_m178_sewer2_BSS_START = .;
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/room_m178_sewer2.rodata.s.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/twelve_elements.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/asm/JP/overlays/room_m178_sewer2/data/RoomLib_HandlerCArgsData_80192238.rodata.s.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m178_sewer2/src-jp/overlays/room_lib/class_close_target.c.o(.bss);
        . = ALIGN(., 4);
        room_m178_sewer2_BSS_END = .;
        room_m178_sewer2_BSS_SIZE = ABSOLUTE(room_m178_sewer2_BSS_END - room_m178_sewer2_BSS_START);
    }
    __romPos += SIZEOF(.room_m178_sewer2);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m178_sewer2_ROM_END = __romPos;
    room_m178_sewer2_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
