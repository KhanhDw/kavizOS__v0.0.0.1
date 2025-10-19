; boot/stage3/print.asm
[BITS 32]

section .text
global print_string

print_string:
    pusha
    mov edi, 0xB8000 + 480    ; Dòng thứ 3 (160*3)
.next_char:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0F              ; White on black
    mov [edi], ax
    add edi, 2
    jmp .next_char
.done:
    popa
    ret