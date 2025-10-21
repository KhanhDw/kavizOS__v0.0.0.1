[BITS 64]

; System call entry point
; Uses SYSCALL/SYSRET instruction pair for fast system calls
; RAX contains the system call number
; RDI, RSI, RDX, R10, R8, R9 contain the arguments (R10 is used instead of RCX for the 4th argument)

; MSR (Model Specific Register) addresses for SYSCALL/SYSRET
IA32_LSTAR equ 0xC0000082
IA32_STAR  equ 0xC0000081
IA32_SFMASK equ 0xC0000084

syscall_entry:
    ; Save registers that might be used as return values
    push rcx    ; Save original return address
    push r11    ; Save original RFLAGS
    
    ; At this point:
    ; RAX = syscall number
    ; RDI = arg1
    ; RSI = arg2
    ; RDX = arg3
    ; RCX = arg4 (will be overwritten by return address)
    ; R8 = arg5
    ; R9 = arg6
    ; R10 = arg4 (we'll use this instead of RCX for the 4th argument)
    
    ; Move arg4 to R10, and arg3 to RCX (so RCX has the original 4th argument)
    mov r10, rdx
    mov rcx, rsi
    
    ; Call syscall_dispatch with arguments
    ; syscall_dispatch(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, 
    ;                 uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6)
    ; RDI = syscall_num (already in RAX, so move it)
    push rdi    ; Save original RDI
    mov rdi, rax    ; syscall_num
    mov rsi, rdi    ; arg1 (was in original RDI)
    pop rdi     ; restore original RDI (arg1)
    mov rdx, rcx    ; arg2 (was in original RSI)
    mov rcx, r8     ; arg3 (was in original RDX) 
    push r9     ; Save arg6 for later
    mov r8, r10   ; arg4 (was in original RCX)
    mov r9, r8    ; arg5 (was in original R8)
    pop r10     ; arg6 (was in original R9)
    
    ; Call the C function syscall_dispatch
    call syscall_dispatch
    
    ; Restore original R11 and RCX (return address)
    pop r11     ; Restore RFLAGS
    pop rcx     ; Restore return address
    
    ; Return to user mode using SYSRET
    ; RAX contains the return value
    ; RCX contains the return address
    ; R11 contains the RFLAGS
    sysretq

; Function to initialize syscall MSR registers
init_syscall_msr:
    ; Set up STAR MSR: Contains CS and SS selectors for SYSCALL
    ; Bits 47:32 = CS selector for syscall (kernel CS)
    ; Bits 31:16 = SS selector for syscall (kernel SS)
    mov ecx, IA32_STAR
    mov edx, 0x00180008  ; CS = 0x08 (kernel code), SS = 0x10 (kernel data), shifted right by 16
    mov eax, 0x00200010  ; CS = 0x20 (user code), SS = 0x28 (user data) - shifted left 16
    wrmsr
    
    ; Set up LSTAR MSR: Contains the linear address of the syscall entry point
    mov ecx, IA32_LSTAR
    lea rax, [syscall_entry]
    mov edx, eax
    shr edx, 32
    wrmsr
    
    ; Set up SFMASK MSR: Contains the mask of RFLAGS bits that are cleared during SYSCALL
    mov ecx, IA32_SFMASK
    mov eax, 0x00000200  ; Clear interrupt flag during SYSCALL
    mov edx, 0x00000000
    wrmsr
    
    ret