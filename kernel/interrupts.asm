; kernel/interrupts.asm - Interrupt Service Routine handlers for 64-bit
[BITS 64]

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; Common ISR handler macro for 64-bit
%macro ISR_NOERRCODE 1
isr%1:
    cli
    push 0          ; Push dummy error code
    push %1         ; Push interrupt number
    jmp isr_common_stub
%endmacro

; ISR handler macro with error code for 64-bit
%macro ISR_ERRCODE 1
isr%1:
    cli
    push %1         ; Push interrupt number
    jmp isr_common_stub
%endmacro

; Define ISRs 0-31
ISR_NOERRCODE 0    ; Division by zero exception
ISR_NOERRCODE 1    ; Debug exception
ISR_NOERRCODE 2    ; Non-maskable interrupt
ISR_NOERRCODE 3    ; Breakpoint exception
ISR_NOERRCODE 4    ; Overflow exception
ISR_NOERRCODE 5    ; Bound range exceeded exception
ISR_NOERRCODE 6    ; Invalid opcode exception
ISR_NOERRCODE 7    ; Device not available exception
ISR_ERRCODE   8    ; Double fault exception
ISR_NOERRCODE 9    ; Coprocessor segment overrun
ISR_ERRCODE   10   ; Invalid TSS exception
ISR_ERRCODE   11   ; Segment not present
ISR_ERRCODE   12   ; Stack segment fault
ISR_ERRCODE   13   ; General protection fault
ISR_ERRCODE   14   ; Page fault exception
ISR_NOERRCODE 15   ; Reserved exception
ISR_NOERRCODE 16   ; x87 floating point exception
ISR_NOERRCODE 17   ; Alignment check exception
ISR_NOERRCODE 18   ; Machine check exception
ISR_NOERRCODE 19   ; SIMD floating point exception
ISR_NOERRCODE 20   ; Virtualization exception
ISR_NOERRCODE 21   ; Reserved
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Reserved
ISR_NOERRCODE 29   ; Reserved
ISR_NOERRCODE 30   ; Reserved
ISR_NOERRCODE 31   ; Reserved

; Common ISR stub for 64-bit
isr_common_stub:
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

    ; Call C handler
    mov rdi, rsp    ; Pass pointer to registers structure
    call isr_handler

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
extern isr_handler

section .note.GNU-stack noalloc noexec nowrite progbits