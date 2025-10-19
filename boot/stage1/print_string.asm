
; print_string.asm - Hàm in chuỗi ra màn hình (BIOS int 10h)
[BITS 16]
global print_string

print_string:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret
