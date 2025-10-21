// kernel/kernel_loader.c
#include "boot.h"
#include "drivers/console.h"
#include "interrupt.h"
#include "memory.h"
#include "timer.h"
#include "drivers/keyboard.h"
#include "scheduler.h"
#include "test.h"

// External symbols for BSS section
extern unsigned int _bss_start;
extern unsigned int _bss_end;

// Test tasks
void task1(void) {
    for (;;) {
        console_write("1");
        scheduler_sleep(10);
    }
}

void task2(void) {
    for (;;) {
        console_write("2");
        scheduler_sleep(15);
    }
}

// Function to zero out the BSS section
void zero_bss(void) {
    unsigned int *bss_start = &_bss_start;
    unsigned int *bss_end = &_bss_end;
    
    while (bss_start < bss_end) {
        *bss_start++ = 0;
    }
}

void kernel_main(uint64_t boot_info_addr, uint64_t magic) {
    // Zero out the BSS section
    zero_bss();
    
    // Initialize console
    console_initialize();

    // Print welcome message
    console_write("Welcome to Kaviz OS!\n");
    console_write("Console System: READY\n");
    
    // Verify boot parameters
    if (magic != 0x1BADB002) {
        console_write("ERROR: Invalid boot magic number!\n");
    } else {
        console_write("Boot parameters verified.\n");
    }

    // Use the boot_info_addr parameter to avoid warning
    if (boot_info_addr != 0x9000) {
        // This is just to use the parameter and avoid warning
        // In a real implementation, we would use this address
    }

    // Initialize memory management
    memory_init();
    
    // Run memory management tests
    test_memory_management();
    
    // Initialize interrupt system
    idt_init();
    
    // Initialize scheduler
    scheduler_init();
    
    // Initialize timer
    timer_init();
    
    // Initialize keyboard
    keyboard_init();
    
    // Add test tasks
    scheduler_add_task(task1);
    scheduler_add_task(task2);
    
    // Run comprehensive tests
    run_tests();

    // Example allocation
    console_write("Attempting memory allocation...\n");
    void* test_ptr = kmalloc(1024);
    if (test_ptr != NULL) {
        console_write("Memory allocation successful!\n");
        kfree(test_ptr);
        console_write("Memory deallocation successful!\n");
    } else {
        console_write("Memory allocation failed!\n");
    }
    
    // Print timer info
    console_write("Timer initialized. Current tick count: ");
    // Print tick count (simplified)
    console_write("\n");
    
    // Print final message
    console_write("Kernel initialization complete.\n");

    console_write("> ");

    // Main loop
    for(;;) {
        // Check for keyboard input
        if (keyboard_has_input()) {
            char c = keyboard_getchar();
            console_putchar(c);
        }
        
        asm volatile("hlt");
    }
}