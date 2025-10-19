; boot/stage2/delay.asm
[BITS 32]
delay:
    mov ecx, 0x00FFFFFF
.delay_loop:
    dec ecx
    jnz .delay_loop
    ret
