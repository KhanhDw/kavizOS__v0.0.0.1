; kernel/kernel_entry.asm
[BITS 64]

extern kernel_main
extern _bss_start
extern _bss_end

global _start

; Stack size for the kernel (16KB)
KERNEL_STACK_SIZE equ 16384

section .text
_start:
    ; Disable interrupts
    cli

    ; Zero out the BSS section
    mov rdi, _bss_start
    mov rcx, _bss_end
    sub rcx, rdi
    xor rax, rax
    rep stosb

    ; Set up the kernel stack
    mov rsp, stack_top

    ; Align stack to 16-byte boundary as required by some compilers
    and rsp, 0xFFFFFFFFFFFFFFF0

    ; The bootloader passes:
    ; RDI = Boot info structure address (0x9000)
    ; RSI = Magic number (0x1BADB002)
    
    ; Boot info is already in RDI, magic number in RSI
    ; No need to move them

    ; Call the main kernel function
    call kernel_main

    ; Halt the system if kernel_main returns
.halt:
    hlt
    jmp .halt

section .bss
align 16
stack_bottom:
    resb KERNEL_STACK_SIZE
stack_top:

section .note.GNU-stack noalloc noexec nowrite progbits