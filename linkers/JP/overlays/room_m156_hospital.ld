SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m156_hospital_ROM_START = __romPos;
    room_m156_hospital_VRAM = ADDR(.room_m156_hospital);
    .room_m156_hospital (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m156_hospital_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m156_hospital_RODATA_START = .;
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/room_m156_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/spark_attacks.c.o(.rodata);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_arg.c.o(.rodata);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_slot.c.o(.rodata);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/capture_frame_sample.c.o(.rodata);
        . = ALIGN(., 4);
        room_m156_hospital_RODATA_END = .;
        room_m156_hospital_RODATA_SIZE = ABSOLUTE(room_m156_hospital_RODATA_END - room_m156_hospital_RODATA_START);
        room_m156_hospital_TEXT_START = .;
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/spark_attacks.c.o(.text);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_arg.c.o(.text);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_slot.c.o(.text);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/capture_frame_sample.c.o(.text);
        . = ALIGN(., 4);
        room_m156_hospital_TEXT_END = .;
        room_m156_hospital_TEXT_SIZE = ABSOLUTE(room_m156_hospital_TEXT_END - room_m156_hospital_TEXT_START);
        room_m156_hospital_DATA_START = .;
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/room_m156_header.rodata.s.o(.data);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/spark_attacks.c.o(.data);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_arg.c.o(.data);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/capture_frame_sample.c.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192ADC.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192B08.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192B40.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192B64.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192B9C.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/RoomSlotA_801929E8.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192BFC.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/RoomLib_SlotTable.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_80192C30.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/g_RoomCaptureX.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/g_RoomCaptureZ.data.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/room_m156_hospital/D_80192C3C.s.o(.data);
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/D_801D01E0.data.s.o(.data);
        . = ALIGN(., 4);
        room_m156_hospital_DATA_END = .;
        room_m156_hospital_DATA_SIZE = ABSOLUTE(room_m156_hospital_DATA_END - room_m156_hospital_DATA_START);
        room_m156_hospital_BSS_START = .;
        build/JP/overlays/room_m156_hospital/asm/JP/overlays/room_m156_hospital/data/room_m156_header.rodata.s.o(.bss);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/spark_attacks.c.o(.bss);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_arg.c.o(.bss);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/script_slot.c.o(.bss);
        build/JP/overlays/room_m156_hospital/src-jp/overlays/room_lib/capture_frame_sample.c.o(.bss);
        . = ALIGN(., 4);
        room_m156_hospital_BSS_END = .;
        room_m156_hospital_BSS_SIZE = ABSOLUTE(room_m156_hospital_BSS_END - room_m156_hospital_BSS_START);
    }
    __romPos += SIZEOF(.room_m156_hospital);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m156_hospital_ROM_END = __romPos;
    room_m156_hospital_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
