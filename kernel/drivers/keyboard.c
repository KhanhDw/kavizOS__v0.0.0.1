// kernel/drivers/keyboard.c
#include "keyboard.h"
#include "../drivers/port_io.h"
#include "../drivers/console.h"
#include "../interrupt.h"
#include "../apic.h"
#include <stdint.h>

// Keyboard input buffer
#define KEYBOARD_BUFFER_SIZE 256
static char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
static uint32_t buffer_head = 0;
static uint32_t buffer_tail = 0;

// Scancode to ASCII mapping for US keyboard layout (simplified)
static const char scancode_to_ascii[] = {
    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
};

// Check if there's input in the buffer
int keyboard_has_input(void) {
    return (buffer_head != buffer_tail);
}

// Get a character from the keyboard buffer
char keyboard_getchar(void) {
    if (!keyboard_has_input()) {
        return 0;
    }
    
    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Keyboard interrupt handler
void keyboard_handler(void) {
    // Read scancode from PS/2 data port
    uint8_t scancode = inb(PS2_DATA_PORT);
    
    // Only handle key press events (not key release)
    if (!(scancode & 0x80)) {
        // Convert scancode to ASCII
        if (scancode < sizeof(scancode_to_ascii)) {
            char c = scancode_to_ascii[scancode];
            if (c != 0) {
                // Add to buffer
                uint32_t next_head = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
                if (next_head != buffer_tail) {  // Buffer not full
                    keyboard_buffer[buffer_head] = c;
                    buffer_head = next_head;
                }
            }
        }
    }
    
    // Send EOI to APIC
    apic_eoi();
}

// Initialize keyboard
void keyboard_init(void) {
    console_write("Initializing keyboard...\n");
    
    // The keyboard should already be initialized by the BIOS
    // In a more complete implementation, we might want to:
    // 1. Disable interrupts
    // 2. Flush the output buffer
    // 3. Set the keyboard controller configuration
    // 4. Enable interrupts
    
    console_write("Keyboard initialized.\n");
}