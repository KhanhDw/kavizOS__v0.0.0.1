; boot/stage2/print_string_pm.asm
; BITS 32
; In chuỗi ASCII (zero-terminated) lên VGA text mode tại dòng 3 (an toàn)
[BITS 32]
section .text
global print_string_pm

; ESI -> pointer to zero-terminated string (flat address in PM)
print_string_pm:
    pushad
    mov edi, 0xB8000 + 320    ; start of row 3
.next_char:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0F              ; attribute = white on black
    mov [edi], ax             ; write character + attribute
    add edi, 2
    jmp .next_char
.done:
    popad
    ret
