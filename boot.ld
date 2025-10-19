/* boot.ld */
ENTRY(kernel_main)

SECTIONS
{
    . = 0x100000;  /* Địa chỉ nạp kernel (1MB) */

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}