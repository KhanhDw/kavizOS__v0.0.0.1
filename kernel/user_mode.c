#include "user_mode.h"
#include "drivers/console.h"
#include "memory.h"
#include <stdint.h>

// Assembly function prototypes
extern void switch_to_user_mode(uint64_t user_stack, uint64_t user_function);

// User mode code segment descriptor
// DPL = 3 (user mode), Present = 1, Code = 1, Readable = 1
#define USER_CODE_SEGMENT_DESC 0x0020980000000000

// User mode data segment descriptor
// DPL = 3 (user mode), Present = 1, Data = 1, Writable = 1
#define USER_DATA_SEGMENT_DESC 0x0000920000000000

// Global variables for user mode segments
extern uint64_t gdt_entries[];
extern uint64_t gdt_pointer[];

// Initialize user mode support
void user_mode_init(void) {
    console_write("Initializing user mode support...\n");
    
    // Set up user mode segments in GDT
    setup_user_segments();
    
    console_write("User mode support initialized.\n");
}

// Set up user mode segments in GDT
void setup_user_segments(void) {
    // Add user code segment descriptor to GDT
    gdt_entries[3] = USER_CODE_SEGMENT_DESC;
    
    // Add user data segment descriptor to GDT
    gdt_entries[4] = USER_DATA_SEGMENT_DESC;
    
    console_write("User mode segments added to GDT.\n");
}

// Enter user mode and execute the specified function
void enter_user_mode(void (*user_function)(void)) {
    console_write("Entering user mode...\n");
    
    // Allocate a user stack (8KB)
    uint64_t user_stack_size = 8192;
    uint64_t user_stack_phys = (uint64_t)alloc_physical_page();
    
    // Map user stack to virtual address
    uint64_t user_stack_virt = 0x100000000; // User stack at 4GB
    for (uint64_t i = 0; i < user_stack_size; i += PAGE_SIZE) {
        void* phys_page = alloc_physical_page();
        if (phys_page == NULL) {
            console_write("ERROR: Failed to allocate physical page for user stack\n");
            return;
        }
        if (map_page(user_stack_virt + i, (uint64_t)phys_page, 
                     PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER) != 0) {
            console_write("ERROR: Failed to map user stack page\n");
            return;
        }
    }
    
    // Set up user stack pointer (pointing to the top of the stack)
    uint64_t user_stack_top = user_stack_virt + user_stack_size;
    
    // Switch to user mode
    switch_to_user_mode(user_stack_top, (uint64_t)user_function);
}