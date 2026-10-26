SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m389_chrysler_ROM_START = __romPos;
    room_m389_chrysler_VRAM = ADDR(.room_m389_chrysler);
    .room_m389_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m389_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m389_chrysler_RODATA_START = .;
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/room_m389_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/twelve_elements.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/orbit_set.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_HandlerCArgsData_801931EC.rodata.s.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_close_target.c.o(.rodata);
        . = ALIGN(., 4);
        room_m389_chrysler_RODATA_END = .;
        room_m389_chrysler_RODATA_SIZE = ABSOLUTE(room_m389_chrysler_RODATA_END - room_m389_chrysler_RODATA_START);
        room_m389_chrysler_TEXT_START = .;
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/twelve_elements.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/orbit_set.c.o(.text);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m389_chrysler_TEXT_END = .;
        room_m389_chrysler_TEXT_SIZE = ABSOLUTE(room_m389_chrysler_TEXT_END - room_m389_chrysler_TEXT_START);
        room_m389_chrysler_DATA_START = .;
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/room_m389_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/twelve_elements.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/orbit_set.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_HandlerCArgsData_801931EC.rodata.s.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194138.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_TableA_8018F264.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194150.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_8019415C.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194164.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194180.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801941BC.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801941CC.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_TableC.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801941F4.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194208.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194214.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_TableB_8018FE90.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_8019434C.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194350.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194354.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194358.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194360.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194364.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194368.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194370.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194374.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194378.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194380.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194384.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194388.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194390.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194394.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_80194398.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801943A0.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801943A4.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/room_m389_chrysler/D_801943A8.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/D_801B7ECC.data.s.o(.data);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/room_m389_chrysler/D_801B7EFC.s.o(.data);
        . = ALIGN(., 4);
        room_m389_chrysler_DATA_END = .;
        room_m389_chrysler_DATA_SIZE = ABSOLUTE(room_m389_chrysler_DATA_END - room_m389_chrysler_DATA_START);
        room_m389_chrysler_BSS_START = .;
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/room_m389_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/twelve_elements.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/orbit_set.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/asm/JP/overlays/room_m389_chrysler/data/RoomLib_HandlerCArgsData_801931EC.rodata.s.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m389_chrysler/src-jp/overlays/room_lib/class_close_target.c.o(.bss);
        . = ALIGN(., 4);
        room_m389_chrysler_BSS_END = .;
        room_m389_chrysler_BSS_SIZE = ABSOLUTE(room_m389_chrysler_BSS_END - room_m389_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m389_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m389_chrysler_ROM_END = __romPos;
    room_m389_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
