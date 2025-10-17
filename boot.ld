/* boot.ld */
ENTRY(kernel_main)  /* SỬA THÀNH kernel_main */

SECTIONS
{
    . = 0x10000;  /* Địa chỉ nạp kernel */

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