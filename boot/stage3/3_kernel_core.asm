; boot/stage3/kernel_core.asm - STAGE3 (32-bit)
[BITS 32]
[ORG 0x10000]

global start

%define KERNEL_SRC_PHYS 0x11000
%define KERNEL_DST      0x00100000
%define KERNEL_SECTORS  10  ; Thay bằng số sector thực tế

start:
    cli
    mov esi, msg_stage3
    call print_string

    ; copy kernel
    pushad
    mov esi, KERNEL_SRC_PHYS
    mov edi, KERNEL_DST
    mov ecx, (KERNEL_SECTORS * 512) / 4
    cld
.copy_loop:
    mov eax, [esi]
    mov [edi], eax
    add esi, 4
    add edi, 4
    dec ecx
    jnz .copy_loop
    popad

    mov esi, msg_kernel_loaded
    call print_string

    ; Thêm delay ngắn để đọc thông báo
    mov ecx, 0x00FFFFFF
.delay:
    dec ecx
    jnz .delay

    jmp KERNEL_DST

; Import các file con
%include "print.asm"
; %include "disk.asm"  ; Tạm thời comment vì chưa cần

section .data
msg_stage3 db "Stage3: copying kernel to 1MB...",0
msg_kernel_loaded db "Stage3: jump to kernel @1MB",0