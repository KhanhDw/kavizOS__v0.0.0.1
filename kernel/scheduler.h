// kernel/scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

// Task states
#define TASK_RUNNING  0
#define TASK_READY    1
#define TASK_BLOCKED  2
#define TASK_SLEEPING 3
#define TASK_ZOMBIE   4

// Maximum number of tasks
#define MAX_TASKS 64

// Task Control Block
struct task {
    uint64_t rsp;           // Stack pointer
    uint64_t rip;           // Instruction pointer
    uint64_t rbp;           // Base pointer
    uint64_t rax, rbx, rcx, rdx, rsi, rdi;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t cs, ds, es, fs, gs, ss;
    uint64_t rflags;
    uint32_t id;
    uint32_t state;
    uint32_t priority;
    uint32_t ticks;
    uint32_t sleep_ticks;
};

// Function prototypes
void scheduler_init(void);
void scheduler_add_task(void (*entry_point)(void));
void scheduler_schedule(void);
void scheduler_yield(void);
void scheduler_sleep(uint32_t ticks);
struct task* scheduler_get_current_task(void);

// Assembly functions
extern void context_switch(struct task* current, struct task* next);
extern void save_context(struct task* task);
extern void restore_context(struct task* task);

#endif