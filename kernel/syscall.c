#include "syscall.h"
#include "drivers/console.h"
#include "process.h"
#include "fs/vfs.h"
#include <stdint.h>

// System call handlers array
syscall_handler_t syscall_handlers[MAX_SYSCALLS];

// System call handlers
static uint64_t sys_exit(uint64_t status, uint64_t unused1, uint64_t unused2, 
                        uint64_t unused3, uint64_t unused4, uint64_t unused5);
static uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count, 
                         uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count, 
                        uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_open(uint64_t filename, uint64_t flags, uint64_t mode, 
                        uint64_t unused1, uint64_t unused2, uint64_t unused3);
static uint64_t sys_close(uint64_t fd, uint64_t unused1, uint64_t unused2, 
                         uint64_t unused3, uint64_t unused4, uint64_t unused5);
static uint64_t sys_fork(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                        uint64_t unused4, uint64_t unused5, uint64_t unused6);
static uint64_t sys_exec(uint64_t filename, uint64_t unused1, uint64_t unused2, 
                        uint64_t unused3, uint64_t unused4, uint64_t unused5);
static uint64_t sys_wait(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                        uint64_t unused4, uint64_t unused5, uint64_t unused6);
static uint64_t sys_sleep(uint64_t ticks, uint64_t unused1, uint64_t unused2, 
                         uint64_t unused3, uint64_t unused4, uint64_t unused5);
static uint64_t sys_getpid(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                          uint64_t unused4, uint64_t unused5, uint64_t unused6);
static uint64_t sys_yield(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                         uint64_t unused4, uint64_t unused5, uint64_t unused6);

// Initialize system call interface and register handlers
void syscall_init(void) {
    console_write("Initializing system call interface...\n");
    
    // Clear syscall handlers array
    for (int i = 0; i < MAX_SYSCALLS; i++) {
        syscall_handlers[i] = NULL;
    }
    
    // Register system call handlers
    syscall_register(SYSCALL_EXIT, (syscall_handler_t)sys_exit);
    syscall_register(SYSCALL_WRITE, (syscall_handler_t)sys_write);
    syscall_register(SYSCALL_READ, (syscall_handler_t)sys_read);
    syscall_register(SYSCALL_OPEN, (syscall_handler_t)sys_open);
    syscall_register(SYSCALL_CLOSE, (syscall_handler_t)sys_close);
    syscall_register(SYSCALL_FORK, (syscall_handler_t)sys_fork);
    syscall_register(SYSCALL_EXEC, (syscall_handler_t)sys_exec);
    syscall_register(SYSCALL_WAIT, (syscall_handler_t)sys_wait);
    syscall_register(SYSCALL_SLEEP, (syscall_handler_t)sys_sleep);
    syscall_register(SYSCALL_GETPID, (syscall_handler_t)sys_getpid);
    syscall_register(SYSCALL_YIELD, (syscall_handler_t)sys_yield);
    
    console_write("System call interface initialized with core syscalls.\n");
}

// Exit system call
static uint64_t sys_exit(uint64_t status, uint64_t unused1, uint64_t unused2, 
                        uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    
    console_write("System call: exit(");
    // Print status (would need implementation)
    console_write(")\n");
    
    // Get current process and terminate it
    struct process* current = process_get_current();
    if (current) {
        process_exit(current->pid);
    }
    
    // In a real implementation, we would switch to another process
    // For now, just halt the system
    for (;;)
        asm volatile ("hlt");
    
    return 0;
}

// Write system call
static uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count, 
                         uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    
    // For now, only support writing to stdout (fd = 1)
    if (fd != 1) {
        return -1; // Unsupported file descriptor
    }
    
    // Validate buffer pointer
    if (buf == 0) {
        return -1; // Invalid buffer
    }
    
    // For simplicity, we'll assume the buffer is a null-terminated string
    // In a real implementation, we'd need to validate the buffer and copy it properly
    console_write((const char*)buf);
    
    // Return number of bytes written
    return count;
}

// Read system call (placeholder)
static uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count, 
                        uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    
    // For now, just return error as we don't implement reading
    return -1;
}

// Open system call (placeholder)
static uint64_t sys_open(uint64_t filename, uint64_t flags, uint64_t mode, 
                        uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    
    // For now, just return error as we don't implement file opening
    return -1;
}

// Close system call (placeholder)
static uint64_t sys_close(uint64_t fd, uint64_t unused1, uint64_t unused2, 
                         uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    
    // For now, just return success as we don't implement file closing
    return 0;
}

// Fork system call (placeholder)
static uint64_t sys_fork(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                        uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    (void)unused6;
    
    // For now, just return error as we don't implement process forking
    return -1;
}

// Exec system call (placeholder)
static uint64_t sys_exec(uint64_t filename, uint64_t unused1, uint64_t unused2, 
                        uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    
    // For now, just return error as we don't implement process execution
    return -1;
}

// Wait system call (placeholder)
static uint64_t sys_wait(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                        uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    (void)unused6;
    
    // For now, just return error as we don't implement process waiting
    return -1;
}

// Sleep system call
static uint64_t sys_sleep(uint64_t ticks, uint64_t unused1, uint64_t unused2, 
                         uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    
    console_write("System call: sleep(");
    // Print ticks (would need implementation)
    console_write(")\n");
    
    // For now, just return success
    // In a real implementation, we would put the process to sleep
    return 0;
}

// Get PID system call
static uint64_t sys_getpid(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                          uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    (void)unused6;
    
    struct process* current = process_get_current();
    if (current) {
        return current->pid;
    }
    return 0;
}

// Yield system call
static uint64_t sys_yield(uint64_t unused1, uint64_t unused2, uint64_t unused3, 
                         uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    (void)unused1;
    (void)unused2;
    (void)unused3;
    (void)unused4;
    (void)unused5;
    (void)unused6;
    
    console_write("System call: yield()\n");
    
    // For now, just return success
    // In a real implementation, we would yield to the scheduler
    return 0;
}

// Dispatch system call to appropriate handler
uint64_t syscall_dispatch(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, 
                         uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    // Check if syscall number is valid
    if (syscall_num >= MAX_SYSCALLS) {
        console_write("ERROR: Invalid system call number\n");
        return -1;
    }
    
    // Get the handler for this syscall
    syscall_handler_t handler = syscall_handlers[syscall_num];
    if (handler == NULL) {
        console_write("ERROR: No handler for system call\n");
        return -1;
    }
    
    // Call the handler with the provided arguments
    return handler(arg1, arg2, arg3, arg4, arg5, arg6);
}