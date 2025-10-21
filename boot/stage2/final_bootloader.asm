; boot/stage2/final_bootloader.asm
; Final integrated bootloader with all components
[BITS 16]
[ORG 0x7E00]

; Boot drive address
BOOT_DRIVE_ADDRESS equ 0x7DF0

; Main entry point
stage2_start:
    ; Initialize segment registers
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Print stage2 loading message
    mov si, msg_stage2
    call print_string

    ; Load stage3 from disk
    call load_stage3
    jc stage3_load_error

    ; Enable A20 line
    call enable_a20
    jc a20_enable_error

    ; Load Global Descriptor Table
    call load_gdt

    ; Transition to Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    
    ; Far jump to Protected Mode code
    jmp CODE32_SEG:init_pm

[BITS 32]
init_pm:
    ; Initialize segment registers for Protected Mode
    mov ax, DATA32_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Print Protected Mode ready message
    mov esi, msg_pm_loaded
    call print_string_pm

    ; Initialize bootloader components
    call main_init

    ; If we reach here, something went wrong
    mov eax, ERROR_HANDOVER
    call halt_with_error

; Error handlers
stage3_load_error:
    mov eax, ERROR_DISK_READ
    call halt_with_error

a20_enable_error:
    mov eax, ERROR_A20_ENABLE
    call halt_with_error

; Halt system
halt_system:
    cli
    hlt
    jmp halt_system

; Include all components
%include "print_string.asm"
%include "print_hex.asm"
%include "disk_io.asm"
%include "enable_a20.asm"
%include "improved_gdt.asm"
%include "delay.asm"
%include "print_string_pm.asm"
%include "improved_data.asm"
%include "long_mode_transition.asm"
%include "kernel_handover.asm"
%include "memory_detection.asm"
%include "bootloader_init.asm"
%include "error_handling.asm"
%include "paging_setup.asm"