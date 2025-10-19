; boot/stage2/bootloader_init.asm
; Comprehensive bootloader initialization
[BITS 32]

; Function to initialize all bootloader components
bootloader_init:
    ; Detect memory
    call detect_memory

    ; Set up interrupt handlers (if needed)
    ; call setup_idt

    ; Set up timer (if needed)
    ; call setup_timer

    ; Set up keyboard (if needed)
    ; call setup_keyboard

    ; Prepare for kernel handover
    call prepare_boot_info

    ret

; Function to print boot information
print_boot_info:
    ; Print memory information
    mov esi, msg_memory_info
    call print_string_pm

    ; Print memory entry count
    mov eax, [0x9000 + boot_info.memory_entries]
    call print_hex_pm

    ; Print newline
    mov esi, msg_newline
    call print_string_pm

    ret

; Function to validate kernel before handover
validate_kernel:
    ; Check if kernel is loaded at expected address
    ; TODO: Add kernel validation (checksum, signature, etc.)

    ret

; Main bootloader initialization function
main_init:
    ; Print initialization message
    mov esi, msg_init_start
    call print_string_pm

    ; Initialize bootloader components
    call bootloader_init

    ; Print boot information
    call print_boot_info

    ; Validate kernel
    call validate_kernel

    ; Print handover message
    mov esi, msg_handover_start
    call print_string_pm

    ; Perform handover to kernel
    call handover_to_kernel

    ; If we return here, there was an error
    jmp handover_error

; Data section
section .data
msg_init_start db "Initializing bootloader components...", 0
msg_memory_info db "Memory entries detected: ", 0
msg_handover_start db "Preparing for kernel handover...", 0
msg_newline db 0x0D, 0x0A, 0