/* boot.ld */
OUTPUT_FORMAT("elf32-i386")
ENTRY(main32)

SECTIONS
{
    . = 0x10000;  /* Stage3 load address - QUAN TRỌNG */

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .rodata : {
        *(.rodata)
    }

    .bss : {
        *(.bss)
    }
}