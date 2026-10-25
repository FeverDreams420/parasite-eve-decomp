SECTIONS
{
    HIDDEN(__romPos = 0);
    fx_field_ROM_START = __romPos;
    fx_field_VRAM = ADDR(.fx_field);
    .fx_field (OVERLAY_WINDOW_ROOM + 0x0) : AT(fx_field_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        fx_field_RODATA_START = .;
        build/JP/overlays/fx_field/asm/JP/overlays/fx_field/data/fx_field_header.rodata.s.o(.rodata);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        fx_field_RODATA_END = .;
        fx_field_RODATA_SIZE = ABSOLUTE(fx_field_RODATA_END - fx_field_RODATA_START);
        fx_field_TEXT_START = .;
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/fx_field/asm/JP/overlays/fx_field/data/fx_field_data.data.s.o(.code_data);
        . = ALIGN(., 4);
        fx_field_TEXT_END = .;
        fx_field_TEXT_SIZE = ABSOLUTE(fx_field_TEXT_END - fx_field_TEXT_START);
        fx_field_DATA_START = .;
        build/JP/overlays/fx_field/asm/JP/overlays/fx_field/data/fx_field_header.rodata.s.o(.data);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/script_args.c.o(.data);
        . = ALIGN(., 4);
        fx_field_DATA_END = .;
        fx_field_DATA_SIZE = ABSOLUTE(fx_field_DATA_END - fx_field_DATA_START);
        fx_field_BSS_START = .;
        build/JP/overlays/fx_field/asm/JP/overlays/fx_field/data/fx_field_header.rodata.s.o(.bss);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/fx_field/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        fx_field_BSS_END = .;
        fx_field_BSS_SIZE = ABSOLUTE(fx_field_BSS_END - fx_field_BSS_START);
    }
    __romPos += SIZEOF(.fx_field);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    fx_field_ROM_END = __romPos;
    fx_field_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
