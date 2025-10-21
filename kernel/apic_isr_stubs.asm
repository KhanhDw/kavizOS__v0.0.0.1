; kernel/apic_isr_stubs.asm - APIC Interrupt Service Routine handlers for 64-bit
[BITS 64]

global apic_isr32
global apic_isr33
global apic_isr34
global apic_isr35
global apic_isr36
global apic_isr37
global apic_isr38
global apic_isr39
global apic_isr40
global apic_isr41
global apic_isr42
global apic_isr43
global apic_isr44
global apic_isr45
global apic_isr46
global apic_isr47

; External C handlers
extern timer_callback
extern keyboard_handler

; Common APIC ISR handler macro for 64-bit
%macro APIC_ISR_NOERRCODE 2
apic_isr%1:
    cli
    push 0          ; Push dummy error code
    push %1         ; Push interrupt number
    jmp %2
%endmacro

; Define APIC ISRs 32-47
APIC_ISR_NOERRCODE 32, apic_timer_handler
APIC_ISR_NOERRCODE 33, apic_keyboard_handler
APIC_ISR_NOERRCODE 34, apic_isr_common_stub
APIC_ISR_NOERRCODE 35, apic_isr_common_stub
APIC_ISR_NOERRCODE 36, apic_isr_common_stub
APIC_ISR_NOERRCODE 37, apic_isr_common_stub
APIC_ISR_NOERRCODE 38, apic_isr_common_stub
APIC_ISR_NOERRCODE 39, apic_isr_common_stub
APIC_ISR_NOERRCODE 40, apic_isr_common_stub
APIC_ISR_NOERRCODE 41, apic_isr_common_stub
APIC_ISR_NOERRCODE 42, apic_isr_common_stub
APIC_ISR_NOERRCODE 43, apic_isr_common_stub
APIC_ISR_NOERRCODE 44, apic_isr_common_stub
APIC_ISR_NOERRCODE 45, apic_isr_common_stub
APIC_ISR_NOERRCODE 46, apic_isr_common_stub
APIC_ISR_NOERRCODE 47, apic_isr_common_stub

; Timer specific handler
apic_timer_handler:
    ; Save general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Call C handler
    call timer_callback

    ; Restore general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Clean up error code and interrupt number
    add rsp, 16

    ; Return from interrupt
    iretq

; Keyboard specific handler
apic_keyboard_handler:
    ; Save general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Call C handler
    call keyboard_handler

    ; Restore general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Clean up error code and interrupt number
    add rsp, 16

    ; Return from interrupt
    iretq

; Common APIC ISR stub for 64-bit (for other interrupts)
apic_isr_common_stub:
    ; Save general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Load kernel data segment
    mov ax, 0x10    ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Call C handler (generic handler that just sends EOI)
    mov rdi, rsp    ; Pass pointer to registers structure
    call apic_isr_handler

    ; Restore data segment registers
    mov ax, 0x10    ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Restore general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    ; Clean up error code and interrupt number
    add rsp, 16

    ; Return from interrupt
    iretq

; C handler function (needs to be implemented in C)
extern apic_isr_handler

section .note.GNU-stack noalloc noexec nowrite progbits