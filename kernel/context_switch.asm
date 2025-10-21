; kernel/context_switch.asm - Context switching routines for 64-bit
[BITS 64]

global context_switch
global save_context
global restore_context

; Context switch function
; Parameters:
;   RDI = current task TCB pointer
;   RSI = next task TCB pointer
context_switch:
    ; Save current task context
    call save_context
    
    ; Restore next task context
    mov rdi, rsi
    call restore_context
    
    ; Return
    ret

; Save current context to TCB
; Parameters:
;   RDI = TCB pointer
save_context:
    ; Save general-purpose registers
    mov [rdi + 0x00], rsp
    mov [rdi + 0x08], r15
    mov [rdi + 0x10], r14
    mov [rdi + 0x18], r13
    mov [rdi + 0x20], r12
    mov [rdi + 0x28], r11
    mov [rdi + 0x30], r10
    mov [rdi + 0x38], r9
    mov [rdi + 0x40], r8
    mov [rdi + 0x48], rbp
    mov [rdi + 0x50], rdi
    mov [rdi + 0x58], rsi
    mov [rdi + 0x60], rdx
    mov [rdi + 0x68], rcx
    mov [rdi + 0x70], rbx
    mov [rdi + 0x78], rax
    
    ; Save segment registers
    mov [rdi + 0x80], cs
    mov [rdi + 0x88], ds
    mov [rdi + 0x90], es
    mov [rdi + 0x98], fs
    mov [rdi + 0xA0], gs
    mov [rdi + 0xA8], ss
    
    ; Save RFLAGS
    pushfq
    pop rax
    mov [rdi + 0xB0], rax
    
    ; Save RIP (this is tricky - we need to get the return address)
    mov rax, [rsp]
    mov [rdi + 0xB8], rax
    
    ret

; Restore context from TCB
; Parameters:
;   RDI = TCB pointer
restore_context:
    ; Restore general-purpose registers
    mov rsp, [rdi + 0x00]
    mov r15, [rdi + 0x08]
    mov r14, [rdi + 0x10]
    mov r13, [rdi + 0x18]
    mov r12, [rdi + 0x20]
    mov r11, [rdi + 0x28]
    mov r10, [rdi + 0x30]
    mov r9, [rdi + 0x38]
    mov r8, [rdi + 0x40]
    mov rbp, [rdi + 0x48]
    mov rsi, [rdi + 0x58]
    mov rdx, [rdi + 0x60]
    mov rcx, [rdi + 0x68]
    mov rbx, [rdi + 0x70]
    mov rax, [rdi + 0x78]
    
    ; Restore segment registers
    mov ds, [rdi + 0x88]
    mov es, [rdi + 0x90]
    mov fs, [rdi + 0x98]
    mov gs, [rdi + 0xA0]
    
    ; Restore RFLAGS
    push qword [rdi + 0xB0]
    popfq
    
    ; Restore RIP and jump
    push qword [rdi + 0x80]  ; CS
    push qword [rdi + 0xB8]  ; RIP
    retfq

section .note.GNU-stack noalloc noexec nowrite progbits