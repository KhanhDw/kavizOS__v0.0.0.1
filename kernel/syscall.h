#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

// System call numbers
#define SYSCALL_EXIT     0
#define SYSCALL_WRITE    1
#define SYSCALL_READ     2
#define SYSCALL_OPEN     3
#define SYSCALL_CLOSE    4
#define SYSCALL_FORK     5
#define SYSCALL_EXEC     6
#define SYSCALL_WAIT     7
#define SYSCALL_SLEEP    8
#define SYSCALL_GETPID   9
#define SYSCALL_YIELD    10

// Maximum number of system calls
#define MAX_SYSCALLS 128

// System call handler function pointer
typedef uint64_t (*syscall_handler_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

// Function prototypes
void syscall_init(void);
void syscall_register(uint64_t syscall_num, syscall_handler_t handler);
uint64_t syscall_dispatch(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, 
                         uint64_t arg3, uint64_t arg4, uint64_t arg5, uint64_t arg6);

#endif // SYSCALL_H