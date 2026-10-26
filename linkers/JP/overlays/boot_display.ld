SECTIONS
{
    HIDDEN(__romPos = 0);
    boot_display_ROM_START = __romPos;
    boot_display_VRAM = ADDR(.boot_display);
    .boot_display (OVERLAY_WINDOW_BOOT_DISPLAY + 0x0) : AT(boot_display_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        boot_display_RODATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/text_screen.c.o(.rodata);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/sprites.c.o(.rodata);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/image_display.c.o(.rodata);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_stop.c.o(.rodata);
        . = ALIGN(., 4);
        boot_display_RODATA_END = .;
        boot_display_RODATA_SIZE = ABSOLUTE(boot_display_RODATA_END - boot_display_RODATA_START);
        boot_display_TEXT_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/text_screen.c.o(.text);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/sprites.c.o(.text);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/image_display.c.o(.text);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_stop.c.o(.text);
        . = ALIGN(., 4);
        boot_display_TEXT_END = .;
        boot_display_TEXT_SIZE = ABSOLUTE(boot_display_TEXT_END - boot_display_TEXT_START);
        boot_display_DATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/text_screen.c.o(.data);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/sprites.c.o(.data);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/image_display.c.o(.data);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_stop.c.o(.data);
        . = ALIGN(., 4);
        boot_display_DATA_END = .;
        boot_display_DATA_SIZE = ABSOLUTE(boot_display_DATA_END - boot_display_DATA_START);
        boot_display_BSS_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/text_screen.c.o(.bss);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/sprites.c.o(.bss);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/image_display.c.o(.bss);
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_stop.c.o(.bss);
        . = ALIGN(., 4);
        boot_display_BSS_END = .;
        boot_display_BSS_SIZE = ABSOLUTE(boot_display_BSS_END - boot_display_BSS_START);
    }
    __romPos += SIZEOF(.boot_display);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    boot_display_ROM_END = __romPos;
    boot_display_VRAM_END = .;

    boot_display_file_init_linkbase_ROM_START = __romPos;
    boot_display_file_init_linkbase_VRAM = ADDR(.boot_display_file_init_linkbase);
    .boot_display_file_init_linkbase (OVERLAY_WINDOW_BOOT_DISPLAY + 0x25A0) : AT(boot_display_file_init_linkbase_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        boot_display_file_init_linkbase_RODATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_file.c.o(.rodata);
        . = ALIGN(., 4);
        boot_display_file_init_linkbase_RODATA_END = .;
        boot_display_file_init_linkbase_RODATA_SIZE = ABSOLUTE(boot_display_file_init_linkbase_RODATA_END - boot_display_file_init_linkbase_RODATA_START);
        boot_display_file_init_linkbase_TEXT_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_file.c.o(.text);
        . = ALIGN(., 4);
        boot_display_file_init_linkbase_TEXT_END = .;
        boot_display_file_init_linkbase_TEXT_SIZE = ABSOLUTE(boot_display_file_init_linkbase_TEXT_END - boot_display_file_init_linkbase_TEXT_START);
        boot_display_file_init_linkbase_DATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_file.c.o(.data);
        . = ALIGN(., 4);
        boot_display_file_init_linkbase_DATA_END = .;
        boot_display_file_init_linkbase_DATA_SIZE = ABSOLUTE(boot_display_file_init_linkbase_DATA_END - boot_display_file_init_linkbase_DATA_START);
        boot_display_file_init_linkbase_BSS_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback_file.c.o(.bss);
        . = ALIGN(., 4);
        boot_display_file_init_linkbase_BSS_END = .;
        boot_display_file_init_linkbase_BSS_SIZE = ABSOLUTE(boot_display_file_init_linkbase_BSS_END - boot_display_file_init_linkbase_BSS_START);
    }
    __romPos += SIZEOF(.boot_display_file_init_linkbase);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    boot_display_file_init_linkbase_ROM_END = __romPos;
    boot_display_file_init_linkbase_VRAM_END = .;

    boot_display_playback_ROM_START = __romPos;
    boot_display_playback_VRAM = ADDR(.boot_display_playback);
    .boot_display_playback (OVERLAY_WINDOW_BOOT_DISPLAY + 0x27A0) : AT(boot_display_playback_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        boot_display_playback_RODATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback.c.o(.rodata);
        . = ALIGN(., 4);
        boot_display_playback_RODATA_END = .;
        boot_display_playback_RODATA_SIZE = ABSOLUTE(boot_display_playback_RODATA_END - boot_display_playback_RODATA_START);
        boot_display_playback_TEXT_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback.c.o(.text);
        . = ALIGN(., 4);
        boot_display_playback_TEXT_END = .;
        boot_display_playback_TEXT_SIZE = ABSOLUTE(boot_display_playback_TEXT_END - boot_display_playback_TEXT_START);
        boot_display_playback_DATA_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback.c.o(.data);
        . = ALIGN(., 4);
        boot_display_playback_DATA_END = .;
        boot_display_playback_DATA_SIZE = ABSOLUTE(boot_display_playback_DATA_END - boot_display_playback_DATA_START);
        boot_display_playback_BSS_START = .;
        build/JP/overlays/boot_display/src-jp/overlays/boot_display/playback.c.o(.bss);
        . = ALIGN(., 4);
        boot_display_playback_BSS_END = .;
        boot_display_playback_BSS_SIZE = ABSOLUTE(boot_display_playback_BSS_END - boot_display_playback_BSS_START);
    }
    __romPos += SIZEOF(.boot_display_playback);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    boot_display_playback_ROM_END = __romPos;
    boot_display_playback_VRAM_END = .;

    boot_display_tables_ROM_START = __romPos;
    boot_display_tables_VRAM = ADDR(.boot_display_tables);
    .boot_display_tables (OVERLAY_WINDOW_BOOT_DISPLAY + 0x2BA4) : AT(boot_display_tables_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        boot_display_tables_TEXT_START = .;
        build/JP/overlays/boot_display/asm/JP/overlays/boot_display/data/boot_display_2648.data.s.o(.code_data);
        . = ALIGN(., 4);
        boot_display_tables_TEXT_END = .;
        boot_display_tables_TEXT_SIZE = ABSOLUTE(boot_display_tables_TEXT_END - boot_display_tables_TEXT_START);
    }
    __romPos += SIZEOF(.boot_display_tables);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    boot_display_tables_ROM_END = __romPos;
    boot_display_tables_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
