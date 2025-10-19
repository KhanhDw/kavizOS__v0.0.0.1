; boot/stage2/print_hex.asm
[BITS 16]

print_hex:
    push ax
    mov ah, 0x0E
    mov cl, al
    shr cl, 4
    and cl, 0x0F
    call .print_digit
    mov cl, al
    and cl, 0x0F
    call .print_digit
    pop ax
    ret

.print_digit:
    cmp cl, 9
    jbe .digit
    add cl, 7
.digit:
    add cl, '0'
    mov al, cl
    int 0x10
    ret
