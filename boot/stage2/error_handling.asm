; boot/stage2/error_handling.asm
; Comprehensive error handling for the bootloader
[BITS 16]
[BITS 32]

; Error codes
ERROR_NONE              equ 0
ERROR_DISK_READ         equ 1
ERROR_A20_ENABLE        equ 2
ERROR_GDT_LOAD          equ 3
ERROR_PM_TRANSITION     equ 4
ERROR_KERNEL_LOAD       equ 5
ERROR_MEMORY_DETECTION  equ 6
ERROR_LONG_MODE         equ 7
ERROR_HANDOVER          equ 8
ERROR_INVALID_KERNEL    equ 9

; Function to print error code
print_error_code:
    push eax
    push edx
    
    ; Print error prefix
    mov esi, msg_error_prefix
    call print_string_pm
    
    ; Print error number
    call print_hex_pm
    
    ; Print newline
    mov esi, msg_newline
    call print_string_pm
    
    pop edx
    pop eax
    ret

; Function to halt system with error
halt_with_error:
    ; Print error code
    call print_error_code
    
    ; Halt system
    cli
    hlt
    jmp halt_with_error

; Function to recover from non-fatal errors
recover_from_error:
    ; For now, just print error and continue
    call print_error_code
    ret

; Disk read error handler
handle_disk_error:
    mov eax, ERROR_DISK_READ
    call halt_with_error

; A20 enable error handler
handle_a20_error:
    mov eax, ERROR_A20_ENABLE
    call halt_with_error

; GDT load error handler
handle_gdt_error:
    mov eax, ERROR_GDT_LOAD
    call halt_with_error

; Protected mode transition error handler
handle_pm_error:
    mov eax, ERROR_PM_TRANSITION
    call halt_with_error

; Kernel load error handler
handle_kernel_error:
    mov eax, ERROR_KERNEL_LOAD
    call halt_with_error

; Memory detection error handler
handle_memory_error:
    mov eax, ERROR_MEMORY_DETECTION
    call recover_from_error  ; Non-fatal, use fallback
    ret

; Long mode error handler
handle_long_mode_error:
    mov eax, ERROR_LONG_MODE
    call halt_with_error

; Handover error handler
handle_handover_error:
    mov eax, ERROR_HANDOVER
    call halt_with_error

; Invalid kernel error handler
handle_invalid_kernel:
    mov eax, ERROR_INVALID_KERNEL
    call halt_with_error

; Generic fatal error handler
fatal_error:
    ; Print fatal error message
    mov esi, msg_fatal_error
    call print_string_pm
    
    ; Halt system
    cli
    hlt
    jmp fatal_error

; Function to print hex number (32-bit)
print_hex_pm:
    pushad
    
    ; Print 0x prefix
    mov esi, msg_hex_prefix
    call print_string_pm
    
    ; Print each nibble
    mov ecx, 8
    mov ebx, eax  ; Save original value
    
.print_nibble:
    rol ebx, 4
    mov eax, ebx
    and eax, 0xF
    
    cmp eax, 10
    jl .decimal_digit
    add eax, 7  ; A-F
    
.decimal_digit:
    add eax, '0'
    mov [hex_buffer], al
    mov esi, hex_buffer
    call print_string_pm
    
    loop .print_nibble
    
    popad
    ret

; Data section
section .data
msg_error_prefix db "ERROR [", 0
msg_hex_prefix db "0x", 0
msg_newline db 0x0D, 0x0A, 0
msg_fatal_error db "FATAL ERROR: System halted", 0x0D, 0x0A, 0

; Buffer for hex printing
hex_buffer db 2 dup(0)