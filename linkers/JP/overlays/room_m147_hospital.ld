SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m147_hospital_ROM_START = __romPos;
    room_m147_hospital_VRAM = ADDR(.room_m147_hospital);
    .room_m147_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m147_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m147_hospital_RODATA_START = .;
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_0_44.rodata.s.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/falling_sprite_burst.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop0.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_close_target.c.o(.rodata);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop6.c.o(.rodata);
        . = ALIGN(., 4);
        room_m147_hospital_RODATA_END = .;
        room_m147_hospital_RODATA_SIZE = ABSOLUTE(room_m147_hospital_RODATA_END - room_m147_hospital_RODATA_START);
        room_m147_hospital_TEXT_START = .;
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop0.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop6.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/falling_sprite_burst.c.o(.text);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.text);
        . = ALIGN(., 4);
        room_m147_hospital_TEXT_END = .;
        room_m147_hospital_TEXT_SIZE = ABSOLUTE(room_m147_hospital_TEXT_END - room_m147_hospital_TEXT_START);
        room_m147_hospital_DATA_START = .;
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_0_44.rodata.s.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/falling_sprite_burst.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop0.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop6.c.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190E30.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/RoomLib_TableA_8018F0D0.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190E60.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190E78.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190E84.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/RoomLib_TableB_8018F078.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190EEC.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F10.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/RoomLib_PairB.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F50.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F54.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F58.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F60.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F64.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F68.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F70.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F74.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F78.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F80.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F84.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F88.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F90.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F94.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_80190F98.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_hospital/D_801981EC.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_hospital/D_801C01E0.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/D_801D22DC.data.s.o(.data);
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_hospital/D_801D22F4.s.o(.data);
        . = ALIGN(., 4);
        room_m147_hospital_DATA_END = .;
        room_m147_hospital_DATA_SIZE = ABSOLUTE(room_m147_hospital_DATA_END - room_m147_hospital_DATA_START);
        room_m147_hospital_BSS_START = .;
        build/JP/overlays/room_m147_hospital/asm/JP/overlays/room_m147_hospital/data/room_m147_0_44.rodata.s.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/falling_sprite_burst.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/dropped_flare.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop0.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_close_target.c.o(.bss);
        build/JP/overlays/room_m147_hospital/src-jp/overlays/room_lib/class_nop6.c.o(.bss);
        . = ALIGN(., 4);
        room_m147_hospital_BSS_END = .;
        room_m147_hospital_BSS_SIZE = ABSOLUTE(room_m147_hospital_BSS_END - room_m147_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m147_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m147_hospital_ROM_END = __romPos;
    room_m147_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
