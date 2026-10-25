SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e11_chrysler_ROM_START = __romPos;
    scene_e11_chrysler_VRAM = ADDR(.scene_e11_chrysler);
    .scene_e11_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e11_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e11_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e11_chrysler/asm/JP/overlays/scene_e11_chrysler/data/scene_e11_header.rodata.s.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/paired_glow.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/dropped_flare.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_draw_list.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_tables.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.rodata);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e11_chrysler_RODATA_END = .;
        scene_e11_chrysler_RODATA_SIZE = ABSOLUTE(scene_e11_chrysler_RODATA_END - scene_e11_chrysler_RODATA_START);
        scene_e11_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_draw_list.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_tables.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/paired_glow.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/dropped_flare.c.o(.text);
        . = ALIGN(., 4);
        scene_e11_chrysler_TEXT_END = .;
        scene_e11_chrysler_TEXT_SIZE = ABSOLUTE(scene_e11_chrysler_TEXT_END - scene_e11_chrysler_TEXT_START);
        scene_e11_chrysler_DATA_START = .;
        build/JP/overlays/scene_e11_chrysler/asm/JP/overlays/scene_e11_chrysler/data/scene_e11_header.rodata.s.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/paired_glow.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/dropped_flare.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_draw_list.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_tables.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/scene_e11_chrysler/asm/JP/overlays/scene_e11_chrysler/data/2A18.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e11_chrysler_DATA_END = .;
        scene_e11_chrysler_DATA_SIZE = ABSOLUTE(scene_e11_chrysler_DATA_END - scene_e11_chrysler_DATA_START);
        scene_e11_chrysler_BSS_START = .;
        build/JP/overlays/scene_e11_chrysler/asm/JP/overlays/scene_e11_chrysler/data/scene_e11_header.rodata.s.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/paired_glow.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/dropped_flare.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_draw_list.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_register_tables.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.bss);
        build/JP/overlays/scene_e11_chrysler/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        scene_e11_chrysler_BSS_END = .;
        scene_e11_chrysler_BSS_SIZE = ABSOLUTE(scene_e11_chrysler_BSS_END - scene_e11_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e11_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e11_chrysler_ROM_END = __romPos;
    scene_e11_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
