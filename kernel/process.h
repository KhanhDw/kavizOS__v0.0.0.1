#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "scheduler.h"
#include "registers.h"

// Process states
#define PROCESS_RUNNING    0
#define PROCESS_READY      1
#define PROCESS_BLOCKED    2
#define PROCESS_TERMINATED 3

// Process ID type
typedef uint32_t pid_t;

// Maximum number of processes
#define MAX_PROCESSES 64

// Process control block structure
struct process {
    pid_t pid;                      // Process ID
    uint32_t state;                 // Process state
    uint64_t cr3;                   // Page directory base address
    uint64_t user_stack;            // User stack pointer
    uint64_t kernel_stack;          // Kernel stack pointer
    struct registers context;       // Saved context for context switching
    uint64_t entry_point;           // Entry point of the process
    uint64_t heap_start;            // Start of heap
    uint64_t heap_end;              // End of heap
    uint64_t heap_max;              // Maximum heap address
    char name[32];                  // Process name
    uint32_t parent_pid;            // Parent process ID
    uint32_t child_count;           // Number of child processes
};

// Function prototypes
void process_init(void);
pid_t process_create(void (*entry_point)(void), const char* name);
void process_exit(pid_t pid);
struct process* process_get_current(void);
struct process* process_get_by_pid(pid_t pid);
void process_switch(struct process* old_process, struct process* new_process);
void process_yield(void);
void process_sleep(uint32_t ticks);

#endif // PROCESS_H