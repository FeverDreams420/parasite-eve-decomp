SECTIONS
{
    HIDDEN(__romPos = 0);
    mdec_diag_diagnostics_ROM_START = __romPos;
    mdec_diag_diagnostics_VRAM = ADDR(.mdec_diag_diagnostics);
    .mdec_diag_diagnostics (OVERLAY_WINDOW_MDEC_DIAG + 0x0) : AT(mdec_diag_diagnostics_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_diagnostics_DATA_START = .;
        build/JP/overlays/mdec_diag/asm/JP/overlays/mdec_diag/data/mdec_diag_diagnostics.data.s.o(.data);
        . = ALIGN(., 4);
        mdec_diag_diagnostics_DATA_END = .;
        mdec_diag_diagnostics_DATA_SIZE = ABSOLUTE(mdec_diag_diagnostics_DATA_END - mdec_diag_diagnostics_DATA_START);
    }
    __romPos += SIZEOF(.mdec_diag_diagnostics);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_diagnostics_ROM_END = __romPos;
    mdec_diag_diagnostics_VRAM_END = .;

    mdec_diag_expand_ROM_START = __romPos;
    mdec_diag_expand_VRAM = ADDR(.mdec_diag_expand);
    .mdec_diag_expand (OVERLAY_WINDOW_MDEC_DIAG + 0x54) : AT(mdec_diag_expand_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_expand_RODATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_build.c.o(.rodata);
        . = ALIGN(., 4);
        mdec_diag_expand_RODATA_END = .;
        mdec_diag_expand_RODATA_SIZE = ABSOLUTE(mdec_diag_expand_RODATA_END - mdec_diag_expand_RODATA_START);
        mdec_diag_expand_TEXT_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_build.c.o(.text);
        . = ALIGN(., 4);
        mdec_diag_expand_TEXT_END = .;
        mdec_diag_expand_TEXT_SIZE = ABSOLUTE(mdec_diag_expand_TEXT_END - mdec_diag_expand_TEXT_START);
        mdec_diag_expand_DATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_build.c.o(.data);
        . = ALIGN(., 4);
        mdec_diag_expand_DATA_END = .;
        mdec_diag_expand_DATA_SIZE = ABSOLUTE(mdec_diag_expand_DATA_END - mdec_diag_expand_DATA_START);
        mdec_diag_expand_BSS_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_build.c.o(.bss);
        . = ALIGN(., 4);
        mdec_diag_expand_BSS_END = .;
        mdec_diag_expand_BSS_SIZE = ABSOLUTE(mdec_diag_expand_BSS_END - mdec_diag_expand_BSS_START);
    }
    __romPos += SIZEOF(.mdec_diag_expand);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_expand_ROM_END = __romPos;
    mdec_diag_expand_VRAM_END = .;

    mdec_diag_decoder_padding_ROM_START = __romPos;
    mdec_diag_decoder_padding_VRAM = ADDR(.mdec_diag_decoder_padding);
    .mdec_diag_decoder_padding (OVERLAY_WINDOW_MDEC_DIAG + 0x138) : AT(mdec_diag_decoder_padding_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_decoder_padding_DATA_START = .;
        build/JP/overlays/mdec_diag/asm/JP/overlays/mdec_diag/data/mdec_diag_decoder_padding.data.s.o(.data);
        . = ALIGN(., 4);
        mdec_diag_decoder_padding_DATA_END = .;
        mdec_diag_decoder_padding_DATA_SIZE = ABSOLUTE(mdec_diag_decoder_padding_DATA_END - mdec_diag_decoder_padding_DATA_START);
    }
    __romPos += SIZEOF(.mdec_diag_decoder_padding);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_decoder_padding_ROM_END = __romPos;
    mdec_diag_decoder_padding_VRAM_END = .;

    mdec_diag_mdec_ROM_START = __romPos;
    mdec_diag_mdec_VRAM = ADDR(.mdec_diag_mdec);
    .mdec_diag_mdec (OVERLAY_WINDOW_MDEC_DIAG + 0x144) : AT(mdec_diag_mdec_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_mdec_RODATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/decdct.c.o(.rodata);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_wait.c.o(.rodata);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_status.c.o(.rodata);
        . = ALIGN(., 4);
        mdec_diag_mdec_RODATA_END = .;
        mdec_diag_mdec_RODATA_SIZE = ABSOLUTE(mdec_diag_mdec_RODATA_END - mdec_diag_mdec_RODATA_START);
        mdec_diag_mdec_TEXT_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/decdct.c.o(.text);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_wait.c.o(.text);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_status.c.o(.text);
        . = ALIGN(., 4);
        mdec_diag_mdec_TEXT_END = .;
        mdec_diag_mdec_TEXT_SIZE = ABSOLUTE(mdec_diag_mdec_TEXT_END - mdec_diag_mdec_TEXT_START);
        mdec_diag_mdec_DATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/decdct.c.o(.data);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_wait.c.o(.data);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_status.c.o(.data);
        . = ALIGN(., 4);
        mdec_diag_mdec_DATA_END = .;
        mdec_diag_mdec_DATA_SIZE = ABSOLUTE(mdec_diag_mdec_DATA_END - mdec_diag_mdec_DATA_START);
        mdec_diag_mdec_BSS_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/decdct.c.o(.bss);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_wait.c.o(.bss);
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/mdec_status.c.o(.bss);
        . = ALIGN(., 4);
        mdec_diag_mdec_BSS_END = .;
        mdec_diag_mdec_BSS_SIZE = ABSOLUTE(mdec_diag_mdec_BSS_END - mdec_diag_mdec_BSS_START);
    }
    __romPos += SIZEOF(.mdec_diag_mdec);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_mdec_ROM_END = __romPos;
    mdec_diag_mdec_VRAM_END = .;

    mdec_diag_vlc_padding_ROM_START = __romPos;
    mdec_diag_vlc_padding_VRAM = ADDR(.mdec_diag_vlc_padding);
    .mdec_diag_vlc_padding (OVERLAY_WINDOW_MDEC_DIAG + 0x7CC) : AT(mdec_diag_vlc_padding_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_vlc_padding_DATA_START = .;
        build/JP/overlays/mdec_diag/asm/JP/overlays/mdec_diag/data/mdec_diag_vlc_padding.data.s.o(.data);
        . = ALIGN(., 4);
        mdec_diag_vlc_padding_DATA_END = .;
        mdec_diag_vlc_padding_DATA_SIZE = ABSOLUTE(mdec_diag_vlc_padding_DATA_END - mdec_diag_vlc_padding_DATA_START);
    }
    __romPos += SIZEOF(.mdec_diag_vlc_padding);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_vlc_padding_ROM_END = __romPos;
    mdec_diag_vlc_padding_VRAM_END = .;

    mdec_diag_vlc_ROM_START = __romPos;
    mdec_diag_vlc_VRAM = ADDR(.mdec_diag_vlc);
    .mdec_diag_vlc (OVERLAY_WINDOW_MDEC_DIAG + 0x7D4) : AT(mdec_diag_vlc_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_vlc_RODATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc.c.o(.rodata);
        . = ALIGN(., 4);
        mdec_diag_vlc_RODATA_END = .;
        mdec_diag_vlc_RODATA_SIZE = ABSOLUTE(mdec_diag_vlc_RODATA_END - mdec_diag_vlc_RODATA_START);
        mdec_diag_vlc_TEXT_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc.c.o(.text);
        . = ALIGN(., 4);
        mdec_diag_vlc_TEXT_END = .;
        mdec_diag_vlc_TEXT_SIZE = ABSOLUTE(mdec_diag_vlc_TEXT_END - mdec_diag_vlc_TEXT_START);
        mdec_diag_vlc_DATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc.c.o(.data);
        . = ALIGN(., 4);
        mdec_diag_vlc_DATA_END = .;
        mdec_diag_vlc_DATA_SIZE = ABSOLUTE(mdec_diag_vlc_DATA_END - mdec_diag_vlc_DATA_START);
        mdec_diag_vlc_BSS_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc.c.o(.bss);
        . = ALIGN(., 4);
        mdec_diag_vlc_BSS_END = .;
        mdec_diag_vlc_BSS_SIZE = ABSOLUTE(mdec_diag_vlc_BSS_END - mdec_diag_vlc_BSS_START);
    }
    __romPos += SIZEOF(.mdec_diag_vlc);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_vlc_ROM_END = __romPos;
    mdec_diag_vlc_VRAM_END = .;

    mdec_diag_vlc_c_padding_ROM_START = __romPos;
    mdec_diag_vlc_c_padding_VRAM = ADDR(.mdec_diag_vlc_c_padding);
    .mdec_diag_vlc_c_padding (OVERLAY_WINDOW_MDEC_DIAG + 0xB68) : AT(mdec_diag_vlc_c_padding_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_vlc_c_padding_DATA_START = .;
        build/JP/overlays/mdec_diag/asm/JP/overlays/mdec_diag/data/mdec_diag_vlc_c_padding.data.s.o(.data);
        . = ALIGN(., 4);
        mdec_diag_vlc_c_padding_DATA_END = .;
        mdec_diag_vlc_c_padding_DATA_SIZE = ABSOLUTE(mdec_diag_vlc_c_padding_DATA_END - mdec_diag_vlc_c_padding_DATA_START);
    }
    __romPos += SIZEOF(.mdec_diag_vlc_c_padding);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_vlc_c_padding_ROM_END = __romPos;
    mdec_diag_vlc_c_padding_VRAM_END = .;

    mdec_diag_vlc_c_ROM_START = __romPos;
    mdec_diag_vlc_c_VRAM = ADDR(.mdec_diag_vlc_c);
    .mdec_diag_vlc_c (OVERLAY_WINDOW_MDEC_DIAG + 0xB74) : AT(mdec_diag_vlc_c_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        mdec_diag_vlc_c_RODATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_c.c.o(.rodata);
        . = ALIGN(., 4);
        mdec_diag_vlc_c_RODATA_END = .;
        mdec_diag_vlc_c_RODATA_SIZE = ABSOLUTE(mdec_diag_vlc_c_RODATA_END - mdec_diag_vlc_c_RODATA_START);
        mdec_diag_vlc_c_TEXT_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_c.c.o(.text);
        . = ALIGN(., 4);
        mdec_diag_vlc_c_TEXT_END = .;
        mdec_diag_vlc_c_TEXT_SIZE = ABSOLUTE(mdec_diag_vlc_c_TEXT_END - mdec_diag_vlc_c_TEXT_START);
        mdec_diag_vlc_c_DATA_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_c.c.o(.data);
        build/JP/overlays/mdec_diag/asm/JP/overlays/mdec_diag/data/mdec_diag_tables.data.s.o(.data);
        . = ALIGN(., 4);
        mdec_diag_vlc_c_DATA_END = .;
        mdec_diag_vlc_c_DATA_SIZE = ABSOLUTE(mdec_diag_vlc_c_DATA_END - mdec_diag_vlc_c_DATA_START);
        mdec_diag_vlc_c_BSS_START = .;
        build/JP/overlays/mdec_diag/src-jp/overlays/mdec_diag/vlc_c.c.o(.bss);
        . = ALIGN(., 4);
        mdec_diag_vlc_c_BSS_END = .;
        mdec_diag_vlc_c_BSS_SIZE = ABSOLUTE(mdec_diag_vlc_c_BSS_END - mdec_diag_vlc_c_BSS_START);
    }
    __romPos += SIZEOF(.mdec_diag_vlc_c);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    mdec_diag_vlc_c_ROM_END = __romPos;
    mdec_diag_vlc_c_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
