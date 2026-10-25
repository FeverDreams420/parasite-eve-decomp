SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m291_chrysler_ROM_START = __romPos;
    room_m291_chrysler_VRAM = ADDR(.room_m291_chrysler);
    .room_m291_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m291_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m291_chrysler_RODATA_START = .;
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/room_m291_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/spark_attacks.c.o(.rodata);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_arg.c.o(.rodata);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_slot.c.o(.rodata);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/capture_frame_sample.c.o(.rodata);
        . = ALIGN(., 4);
        room_m291_chrysler_RODATA_END = .;
        room_m291_chrysler_RODATA_SIZE = ABSOLUTE(room_m291_chrysler_RODATA_END - room_m291_chrysler_RODATA_START);
        room_m291_chrysler_TEXT_START = .;
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/spark_attacks.c.o(.text);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_arg.c.o(.text);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_slot.c.o(.text);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/capture_frame_sample.c.o(.text);
        . = ALIGN(., 4);
        room_m291_chrysler_TEXT_END = .;
        room_m291_chrysler_TEXT_SIZE = ABSOLUTE(room_m291_chrysler_TEXT_END - room_m291_chrysler_TEXT_START);
        room_m291_chrysler_DATA_START = .;
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/room_m291_header.rodata.s.o(.data);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/spark_attacks.c.o(.data);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_arg.c.o(.data);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/capture_frame_sample.c.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192ADC.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192B08.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192B40.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192B64.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192B9C.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/RoomSlotA_801929E8.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192BFC.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/RoomLib_SlotTable.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_80192C30.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/g_RoomCaptureX.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/g_RoomCaptureZ.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/room_m291_chrysler/D_80192C3C.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/D_801CD288.data.s.o(.data);
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/room_m291_chrysler/D_801CD2A0.s.o(.data);
        . = ALIGN(., 4);
        room_m291_chrysler_DATA_END = .;
        room_m291_chrysler_DATA_SIZE = ABSOLUTE(room_m291_chrysler_DATA_END - room_m291_chrysler_DATA_START);
        room_m291_chrysler_BSS_START = .;
        build/JP/overlays/room_m291_chrysler/asm/JP/overlays/room_m291_chrysler/data/room_m291_header.rodata.s.o(.bss);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/spark_attacks.c.o(.bss);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_arg.c.o(.bss);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/script_slot.c.o(.bss);
        build/JP/overlays/room_m291_chrysler/src/jp/overlays/room_lib/capture_frame_sample.c.o(.bss);
        . = ALIGN(., 4);
        room_m291_chrysler_BSS_END = .;
        room_m291_chrysler_BSS_SIZE = ABSOLUTE(room_m291_chrysler_BSS_END - room_m291_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m291_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m291_chrysler_ROM_END = __romPos;
    room_m291_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
