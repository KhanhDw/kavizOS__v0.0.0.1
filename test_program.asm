; Test user program that will be loaded and executed by the OS
; This is a simple "Hello World" program that uses system calls

[BITS 64]

section .text
    global _start

_start:
    ; Write "Hello, World!" to stdout
    mov rax, 1          ; sys_write syscall number
    mov rdi, 1          ; stdout file descriptor
    mov rsi, msg        ; message to write
    mov rdx, msg_len    ; message length
    syscall             ; invoke system call

    ; Exit the program
    mov rax, 0          ; sys_exit syscall number
    mov rdi, 0          ; exit status
    syscall             ; invoke system call

section .data
    msg db 'Hello, World!', 10  ; message with newline
    msg_len equ $ - msg         ; message length