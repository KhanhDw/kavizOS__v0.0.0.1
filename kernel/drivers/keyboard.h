// kernel/drivers/keyboard.h
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// PS/2 Keyboard constants
#define PS2_DATA_PORT   0x60
#define PS2_STATUS_PORT 0x64
#define PS2_COMMAND_PORT 0x64

// Function prototypes
void keyboard_init(void);
void keyboard_handler(void);
char keyboard_getchar(void);
int keyboard_has_input(void);

#endif