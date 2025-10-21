#ifndef USER_MODE_H
#define USER_MODE_H

#include <stdint.h>

// User mode segment selectors
#define USER_CODE_SEGMENT 0x18  // User code segment selector
#define USER_DATA_SEGMENT 0x20  // User data segment selector

// RPL (Requested Privilege Level) bits
#define RPL_USER 0x03  // User mode RPL
#define RPL_KERNEL 0x00  // Kernel mode RPL

// Function prototypes
void user_mode_init(void);
void enter_user_mode(void (*user_function)(void));
void setup_user_segments(void);

#endif // USER_MODE_H