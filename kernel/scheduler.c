// kernel/scheduler.c
#include "scheduler.h"
#include "drivers/console.h"
#include "memory.h"
#include <stdint.h>

// Task array
static struct task tasks[MAX_TASKS];
static uint32_t task_count = 0;
static uint32_t current_task = 0;

// Get current task
struct task* scheduler_get_current_task(void) {
    return &tasks[current_task];
}

// Initialize scheduler
void scheduler_init(void) {
    console_write("Initializing scheduler...\n");
    
    // Initialize task array
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].id = 0;
        tasks[i].state = TASK_ZOMBIE;
    }
    
    task_count = 0;
    current_task = 0;
    
    console_write("Scheduler initialized.\n");
}

// Add a new task
void scheduler_add_task(void (*entry_point)(void)) {
    if (task_count >= MAX_TASKS) {
        console_write("ERROR: Maximum number of tasks reached!\n");
        return;
    }
    
    // Find a free task slot
    uint32_t task_id = task_count;
    
    // Initialize task
    tasks[task_id].id = task_id;
    tasks[task_id].state = TASK_READY;
    tasks[task_id].priority = 1;
    tasks[task_id].ticks = 0;
    tasks[task_id].sleep_ticks = 0;
    
    // Set up initial registers
    tasks[task_id].rip = (uint64_t)entry_point;
    tasks[task_id].cs = 0x08;  // Kernel code segment
    tasks[task_id].rflags = 0x202;  // Interrupts enabled
    tasks[task_id].rsp = 0xFFFF800000000000 + (task_id + 1) * 0x10000;  // Allocate stack space
    tasks[task_id].ss = 0x10;  // Kernel data segment
    
    // Zero out other registers
    tasks[task_id].rax = 0;
    tasks[task_id].rbx = 0;
    tasks[task_id].rcx = 0;
    tasks[task_id].rdx = 0;
    tasks[task_id].rsi = 0;
    tasks[task_id].rdi = 0;
    tasks[task_id].rbp = 0;
    tasks[task_id].r8 = 0;
    tasks[task_id].r9 = 0;
    tasks[task_id].r10 = 0;
    tasks[task_id].r11 = 0;
    tasks[task_id].r12 = 0;
    tasks[task_id].r13 = 0;
    tasks[task_id].r14 = 0;
    tasks[task_id].r15 = 0;
    tasks[task_id].ds = 0x10;
    tasks[task_id].es = 0x10;
    tasks[task_id].fs = 0x10;
    tasks[task_id].gs = 0x10;
    
    // Map stack pages
    for (uint64_t addr = tasks[task_id].rsp - 0x10000; addr < tasks[task_id].rsp; addr += 0x1000) {
        void* phys_page = alloc_physical_page();
        if (phys_page != NULL) {
            map_page(addr, (uint64_t)phys_page, 0x07);  // Present, writable, kernel
        }
    }
    
    task_count++;
    
    console_write("Task added. Task count: ");
    // Print task count (simplified)
    console_write("\n");
}

// Yield to next task
void scheduler_yield(void) {
    scheduler_schedule();
}

// Sleep for specified ticks
void scheduler_sleep(uint32_t ticks) {
    if (task_count == 0) return;
    
    tasks[current_task].state = TASK_SLEEPING;
    tasks[current_task].sleep_ticks = ticks;
    scheduler_schedule();
}

// Schedule next task
void scheduler_schedule(void) {
    if (task_count <= 1) return;
    
    // Update current task
    uint32_t old_task = current_task;
    
    // Find next task
    do {
        current_task = (current_task + 1) % task_count;
        
        // Handle sleeping tasks
        if (tasks[current_task].state == TASK_SLEEPING) {
            if (tasks[current_task].sleep_ticks > 0) {
                tasks[current_task].sleep_ticks--;
            }
            if (tasks[current_task].sleep_ticks == 0) {
                tasks[current_task].state = TASK_READY;
            }
        }
    } while (tasks[current_task].state != TASK_READY && current_task != old_task);
    
    // If we're switching to a different task, perform context switch
    if (current_task != old_task) {
        context_switch(&tasks[old_task], &tasks[current_task]);
    }
}