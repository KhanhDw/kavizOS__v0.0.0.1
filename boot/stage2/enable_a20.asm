; boot/stage2/enable_a20.asm
[BITS 16]
enable_a20:
    in al, 0x92
    or al, 2
    out 0x92, al
    ret
