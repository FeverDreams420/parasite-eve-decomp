SECTIONS
{
    HIDDEN(__romPos = 0);
    scene_e19_cruiser_ROM_START = __romPos;
    scene_e19_cruiser_VRAM = ADDR(.scene_e19_cruiser);
    .scene_e19_cruiser (OVERLAY_WINDOW_ROOM + 0x0) : AT(scene_e19_cruiser_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        scene_e19_cruiser_RODATA_START = .;
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header.s.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail.s.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19.c.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail2.s.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19_class.c.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/class_update_callback.c.o(.rodata);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/grab_await_stance.c.o(.rodata);
        . = ALIGN(., 4);
        scene_e19_cruiser_RODATA_END = .;
        scene_e19_cruiser_RODATA_SIZE = ABSOLUTE(scene_e19_cruiser_RODATA_END - scene_e19_cruiser_RODATA_START);
        scene_e19_cruiser_TEXT_START = .;
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19_class.c.o(.text);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/class_update_callback.c.o(.text);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/grab_await_stance.c.o(.text);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19.c.o(.text);
        . = ALIGN(., 4);
        scene_e19_cruiser_TEXT_END = .;
        scene_e19_cruiser_TEXT_SIZE = ABSOLUTE(scene_e19_cruiser_TEXT_END - scene_e19_cruiser_TEXT_START);
        scene_e19_cruiser_DATA_START = .;
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header.s.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail.s.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail2.s.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19_class.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/class_update_callback.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/grab_await_stance.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/C314.data.s.o(.data);
        . = ALIGN(., 4);
        scene_e19_cruiser_DATA_END = .;
        scene_e19_cruiser_DATA_SIZE = ABSOLUTE(scene_e19_cruiser_DATA_END - scene_e19_cruiser_DATA_START);
        scene_e19_cruiser_BSS_START = .;
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header.s.o(.bss);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail.s.o(.bss);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19.c.o(.bss);
        build/JP/overlays/scene_e19_cruiser/asm/JP/overlays/scene_e19_cruiser/data/scene_e19_cruiser/scene_e19_header_tail2.s.o(.bss);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/scene_e19_cruiser/e19_class.c.o(.bss);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/class_update_callback.c.o(.bss);
        build/JP/overlays/scene_e19_cruiser/src/jp/overlays/room_lib/grab_await_stance.c.o(.bss);
        . = ALIGN(., 4);
        scene_e19_cruiser_BSS_END = .;
        scene_e19_cruiser_BSS_SIZE = ABSOLUTE(scene_e19_cruiser_BSS_END - scene_e19_cruiser_BSS_START);
    }
    __romPos += SIZEOF(.scene_e19_cruiser);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    scene_e19_cruiser_ROM_END = __romPos;
    scene_e19_cruiser_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
