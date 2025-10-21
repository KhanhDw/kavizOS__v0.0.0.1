[BITS 64]

; Function to switch from kernel mode to user mode
; Parameters:
;   RDI = user stack pointer
;   RSI = user function address

switch_to_user_mode:
    ; Set up user data segments
    mov ax, 0x20  ; User data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up user stack
    mov rsp, rdi
    
    ; Push user data segment onto stack for IRET
    push 0x20           ; User data segment selector (SS)
    push rdi            ; User stack pointer (RSP)
    push 0x202          ; RFLAGS with interrupts enabled
    push 0x18           ; User code segment selector (CS)
    push rsi            ; User function address (RIP)
    
    ; Perform interrupt return to user mode
    iretq