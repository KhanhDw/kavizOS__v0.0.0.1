; boot/stage2/long_mode_transition.asm
; Code to transition from Protected Mode (32-bit) to Long Mode (64-bit)
[BITS 32]

; Check if CPU supports Long Mode
check_long_mode_support:
    ; Check if CPUID is supported
    pushfd
    pushfd
    xor dword [esp], 0x200000  ; Flip ID bit
    popfd
    pushfd
    pop eax
    xor eax, [esp]
    popfd
    test eax, 0x200000
    jz .no_long_mode  ; CPUID not supported

    ; Check CPUID function 0x80000001
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29  ; Check if LM bit is set
    jz .no_long_mode   ; Long Mode not supported

    ; Long Mode is supported
    jmp .long_mode_supported

.no_long_mode:
    ; Print error message
    mov esi, msg_no_long_mode
    call print_string_pm
    jmp halt_system

.long_mode_supported:
    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5  ; Set PAE bit
    mov cr4, eax

    ; Load PML4 table (assuming it's already set up at a known address)
    ; This would typically be done by the kernel or earlier boot stages
    ; For now, we'll assume it's at a fixed address
    mov eax, 0x100000  ; Example PML4 address
    mov cr3, eax

    ; Enable Long Mode (EFER.LME = 1)
    mov ecx, 0xC0000080  ; EFER MSR
    rdmsr
    or eax, 1 << 8       ; Set LME bit
    wrmsr

    ; Enable Paging (CR0.PG = 1)
    mov eax, cr0
    or eax, 1 << 31      ; Set PG bit
    mov cr0, eax

    ; Far jump to 64-bit code segment
    jmp CODE64_SEG:long_mode_entry

; Long Mode entry point
[BITS 64]
long_mode_entry:
    ; Set up 64-bit segment registers
    mov ax, DATA64_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack for 64-bit mode
    mov rsp, 0x200000  ; Example stack address
    mov rbp, rsp

    ; Jump to 64-bit kernel
    ; This would typically be to the kernel entry point
    jmp 0x100000  ; Example kernel address

; Data section
section .data
msg_no_long_mode db "ERROR: CPU does not support Long Mode (64-bit)!", 0x0D, 0x0A, 0