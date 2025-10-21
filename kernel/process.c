#include "process.h"
#include "drivers/console.h"
#include "memory.h"
#include "user_mode.h"
#include <stdint.h>

// Global process array
static struct process processes[MAX_PROCESSES];
static uint32_t process_count = 0;

static pid_t current_pid = 0;

// Initialize process management
void process_init(void) {
    console_write("Initializing process management...\n");
    
    // Initialize process array
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].state = PROCESS_TERMINATED;
    }
    
    process_count = 0;
    current_pid = 0;
    
    console_write("Process management initialized.\n");
}

// Get current process
struct process* process_get_current(void) {
    return &processes[current_pid];
}

// Get process by PID
struct process* process_get_by_pid(pid_t pid) {
    if (pid >= MAX_PROCESSES) {
        return NULL;
    }
    if (processes[pid].state == PROCESS_TERMINATED) {
        return NULL;
    }
    return &processes[pid];
}

// Create a new process
pid_t process_create(void (*entry_point)(void), const char* name) {
    if (process_count >= MAX_PROCESSES) {
        console_write("ERROR: Maximum number of processes reached!\n");
        return 0;
    }
    
    // Find a free process slot
    pid_t pid = 0;
    for (int i = 1; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_TERMINATED) {
            pid = i;
            break;
        }
    }
    
    if (pid == 0) {
        console_write("ERROR: No free process slots!\n");
        return 0;
    }
    
    // Initialize process
    processes[pid].pid = pid;
    processes[pid].state = PROCESS_READY;
    processes[pid].entry_point = (uint64_t)entry_point;
    processes[pid].parent_pid = current_pid;
    processes[pid].child_count = 0;
    
    // Set process name
    int i;
    for (i = 0; i < 31 && name[i] != '\0'; i++) {
        processes[pid].name[i] = name[i];
    }
    processes[pid].name[i] = '\0';
    
    // Allocate kernel stack (8KB)
    uint64_t kernel_stack_size = 8192;
    processes[pid].kernel_stack = 0xFFFF800000000000 + (pid + 1) * 0x100000; // Allocate stack space
    
    // Map kernel stack pages
    for (uint64_t addr = processes[pid].kernel_stack - kernel_stack_size; 
         addr < processes[pid].kernel_stack; addr += PAGE_SIZE) {
        void* phys_page = alloc_physical_page();
        if (phys_page != NULL) {
            map_page(addr, (uint64_t)phys_page, 0x07); // Present, writable, kernel
        }
    }
    
    // Allocate user stack (8KB)
    uint64_t user_stack_size = 8192;
    processes[pid].user_stack = 0x100000000 + pid * 0x100000; // User stack allocation
    
    // Map user stack pages
    for (uint64_t addr = processes[pid].user_stack - user_stack_size; 
         addr < processes[pid].user_stack; addr += PAGE_SIZE) {
        void* phys_page = alloc_physical_page();
        if (phys_page != NULL) {
            map_page(addr, (uint64_t)phys_page, 0x07 | PAGE_USER); // Present, writable, user
        }
    }
    
    // Set up initial context
    // Zero out registers
    processes[pid].context.rax = 0;
    processes[pid].context.rbx = 0;
    processes[pid].context.rcx = 0;
    processes[pid].context.rdx = 0;
    processes[pid].context.rsi = 0;
    processes[pid].context.rdi = 0;
    processes[pid].context.rbp = 0;
    processes[pid].context.r8 = 0;
    processes[pid].context.r9 = 0;
    processes[pid].context.r10 = 0;
    processes[pid].context.r11 = 0;
    processes[pid].context.r12 = 0;
    processes[pid].context.r13 = 0;
    processes[pid].context.r14 = 0;
    processes[pid].context.r15 = 0;
    
    // Set up initial register values for user mode entry
    processes[pid].context.rip = (uint64_t)entry_point;
    processes[pid].context.cs = USER_CODE_SEGMENT | RPL_USER;  // User code segment with RPL
    processes[pid].context.rflags = 0x202;  // Interrupts enabled
    processes[pid].context.rsp = processes[pid].user_stack;  // User stack pointer
    processes[pid].context.ss = USER_DATA_SEGMENT | RPL_USER;  // User data segment with RPL
    
    
    // Increment process count
    process_count++;
    
    // Update parent's child count
    if (current_pid != 0) {
        processes[current_pid].child_count++;
    }
    
    console_write("Process created. PID: ");
    // Print PID (would need implementation)
    console_write(", Name: ");
    console_write(name);
    console_write("\n");
    
    return pid;
}

// Exit a process
void process_exit(pid_t pid) {
    if (pid >= MAX_PROCESSES || processes[pid].state == PROCESS_TERMINATED) {
        return;
    }
    
    // Mark process as terminated
    processes[pid].state = PROCESS_TERMINATED;
    
    // Decrement process count
    process_count--;
    
    console_write("Process exited. PID: ");
    // Print PID (would need implementation)
    console_write("\n");
}

// Switch between processes
void process_switch(struct process* old_process, struct process* new_process) {
    // Save current process state
    current_pid = new_process->pid;
    
    // Perform context switch
    context_switch((struct task*)&old_process->context, (struct task*)&new_process->context);
}

// Yield to next process
void process_yield(void) {
    // For now, just call scheduler_yield
    // In a more complete implementation, this would interact with the scheduler
    // scheduler_yield();
}

// Sleep for specified ticks
void process_sleep(uint32_t ticks) {
    // For now, just call scheduler_sleep
    // In a more complete implementation, this would interact with the scheduler
    // scheduler_sleep(ticks);
    (void)ticks; // Suppress unused parameter warning
}