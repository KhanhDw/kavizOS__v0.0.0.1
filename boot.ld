/* boot.ld */
ENTRY(_start)

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
        _bss_start = .;
        *(.bss)
        _bss_end = .;
    }
    
    /DISCARD/ : {
        *(.note.GNU-stack)
    }
}