SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e08_chrysler_ROM_START = __romPos;
    scene_e08_chrysler_VRAM = ADDR(.scene_e08_chrysler);
    .scene_e08_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e08_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e08_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e08_chrysler/asm/JP/overlays/scene_e08_chrysler/data/scene_e08_chrysler/scene_e08_header.s.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop0.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/SceneE08_ModuleStart.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop6.c.o(.rodata);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/e08.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e08_chrysler_RODATA_END = .;
        scene_e08_chrysler_RODATA_SIZE = ABSOLUTE(scene_e08_chrysler_RODATA_END - scene_e08_chrysler_RODATA_START);
        scene_e08_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop0.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/SceneE08_ModuleStart.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop6.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/e08.c.o(.text);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        scene_e08_chrysler_TEXT_END = .;
        scene_e08_chrysler_TEXT_SIZE = ABSOLUTE(scene_e08_chrysler_TEXT_END - scene_e08_chrysler_TEXT_START);
        scene_e08_chrysler_DATA_START = .;
        build/JP/overlays/scene_e08_chrysler/asm/JP/overlays/scene_e08_chrysler/data/scene_e08_chrysler/scene_e08_header.s.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop0.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/SceneE08_ModuleStart.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop6.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/e08.c.o(.data);
        build/JP/overlays/scene_e08_chrysler/asm/JP/overlays/scene_e08_chrysler/data/968C.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e08_chrysler_DATA_END = .;
        scene_e08_chrysler_DATA_SIZE = ABSOLUTE(scene_e08_chrysler_DATA_END - scene_e08_chrysler_DATA_START);
        scene_e08_chrysler_BSS_START = .;
        build/JP/overlays/scene_e08_chrysler/asm/JP/overlays/scene_e08_chrysler/data/scene_e08_chrysler/scene_e08_header.s.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop0.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/SceneE08_ModuleStart.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_close_target.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/room_lib/class_nop6.c.o(.bss);
        build/JP/overlays/scene_e08_chrysler/src/jp/overlays/scene_e08_chrysler/e08.c.o(.bss);
        . = ALIGN(., 4);
        scene_e08_chrysler_BSS_END = .;
        scene_e08_chrysler_BSS_SIZE = ABSOLUTE(scene_e08_chrysler_BSS_END - scene_e08_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e08_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e08_chrysler_ROM_END = __romPos;
    scene_e08_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
