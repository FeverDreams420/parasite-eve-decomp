SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e25_chrysler_ROM_START = __romPos;
    scene_e25_chrysler_VRAM = ADDR(.scene_e25_chrysler);
    .scene_e25_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e25_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e25_chrysler_RODATA_START = .;
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/scene_e25_chrysler/scene_e25_header.s.o(.rodata);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.rodata);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e25_chrysler_RODATA_END = .;
        scene_e25_chrysler_RODATA_SIZE = ABSOLUTE(scene_e25_chrysler_RODATA_END - scene_e25_chrysler_RODATA_START);
        scene_e25_chrysler_TEXT_START = .;
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.text);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        scene_e25_chrysler_TEXT_END = .;
        scene_e25_chrysler_TEXT_SIZE = ABSOLUTE(scene_e25_chrysler_TEXT_END - scene_e25_chrysler_TEXT_START);
        scene_e25_chrysler_DATA_START = .;
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/scene_e25_chrysler/scene_e25_header.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.data);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/D_801931E4.data.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/D_80193234.data.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/D_80193254.data.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/RoomSlotA_80193118.data.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/RoomSlotB_80193118.data.s.o(.data);
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/scene_e25_chrysler/RoomSlotC_80193118.s.o(.data);
        . = ALIGN(., 4);
        scene_e25_chrysler_DATA_END = .;
        scene_e25_chrysler_DATA_SIZE = ABSOLUTE(scene_e25_chrysler_DATA_END - scene_e25_chrysler_DATA_START);
        scene_e25_chrysler_BSS_START = .;
        build/JP/overlays/scene_e25_chrysler/asm/JP/overlays/scene_e25_chrysler/data/scene_e25_chrysler/scene_e25_header.s.o(.bss);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.bss);
        build/JP/overlays/scene_e25_chrysler/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        scene_e25_chrysler_BSS_END = .;
        scene_e25_chrysler_BSS_SIZE = ABSOLUTE(scene_e25_chrysler_BSS_END - scene_e25_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.scene_e25_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e25_chrysler_ROM_END = __romPos;
    scene_e25_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
