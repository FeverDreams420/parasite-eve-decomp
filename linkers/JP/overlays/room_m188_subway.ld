SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m188_subway_ROM_START = __romPos;
    room_m188_subway_VRAM = ADDR(.room_m188_subway);
    .room_m188_subway (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m188_subway_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m188_subway_RODATA_START = .;
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway.rodata.s.o(.rodata);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/spark_barrage.c.o(.rodata);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/script_arg_pair.c.o(.rodata);
        . = ALIGN(., 4);
        room_m188_subway_RODATA_END = .;
        room_m188_subway_RODATA_SIZE = ABSOLUTE(room_m188_subway_RODATA_END - room_m188_subway_RODATA_START);
        room_m188_subway_TEXT_START = .;
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/spark_barrage.c.o(.text);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/script_arg_pair.c.o(.text);
        . = ALIGN(., 4);
        room_m188_subway_TEXT_END = .;
        room_m188_subway_TEXT_SIZE = ABSOLUTE(room_m188_subway_TEXT_END - room_m188_subway_TEXT_START);
        room_m188_subway_DATA_START = .;
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway.rodata.s.o(.data);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/spark_barrage.c.o(.data);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/script_arg_pair.c.o(.data);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195E40.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195E64.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195E84.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195EBC.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195EE0.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195F00.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195F28.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195F60.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195F78.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195F98.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/g_RoomScriptArgA.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/g_RoomScriptArgB.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195FD8.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195FE0.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195FE8.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_80195FF0.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway/D_801981F8.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway/D_801B81F4.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway/D_801E01E0.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/D_801EA928.data.s.o(.data);
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway/D_801EA970.s.o(.data);
        . = ALIGN(., 4);
        room_m188_subway_DATA_END = .;
        room_m188_subway_DATA_SIZE = ABSOLUTE(room_m188_subway_DATA_END - room_m188_subway_DATA_START);
        room_m188_subway_BSS_START = .;
        build/JP/overlays/room_m188_subway/asm/JP/overlays/room_m188_subway/data/room_m188_subway.rodata.s.o(.bss);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/spark_barrage.c.o(.bss);
        build/JP/overlays/room_m188_subway/src-jp/overlays/room_lib/script_arg_pair.c.o(.bss);
        . = ALIGN(., 4);
        room_m188_subway_BSS_END = .;
        room_m188_subway_BSS_SIZE = ABSOLUTE(room_m188_subway_BSS_END - room_m188_subway_BSS_START);
    }
    __romPos += SIZEOF(.room_m188_subway);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m188_subway_ROM_END = __romPos;
    room_m188_subway_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
