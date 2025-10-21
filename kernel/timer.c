// kernel/timer.c
#include "timer.h"
#include "drivers/console.h"
#include "drivers/port_io.h"
#include "interrupt.h"
#include "apic.h"
#include "scheduler.h"
#include <stdint.h>

// Tick counter
static volatile uint32_t tick_count = 0;

// PIT constants
#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND      0x43
#define PIT_FREQUENCY    1193182

// Get tick count
uint32_t get_tick_count(void) {
    return tick_count;
}

// Sleep function
void sleep(uint32_t milliseconds) {
    uint32_t start = get_tick_count();
    uint32_t ticks = milliseconds / (1000 / TIMER_FREQUENCY);
    
    while ((get_tick_count() - start) < ticks) {
        // Wait
        asm volatile("hlt");
    }
}

// Timer callback function
void timer_callback(void) {
    tick_count++;
    
    // Call scheduler every few ticks
    if (tick_count % 5 == 0) {
        scheduler_schedule();
    }
    
    // Send EOI to APIC
    apic_eoi();
}

// Initialize PIT
void pit_init(void) {
    console_write("Initializing PIT...\n");
    
    // Calculate divisor
    uint32_t divisor = PIT_FREQUENCY / TIMER_FREQUENCY;
    
    // Send command byte
    outb(PIT_COMMAND, 0x36);  // Channel 0, low/high byte, mode 3 (square wave), binary
    
    // Send divisor
    outb(PIT_CHANNEL0_DATA, divisor & 0xFF);         // Low byte
    outb(PIT_CHANNEL0_DATA, (divisor >> 8) & 0xFF);  // High byte
    
    console_write("PIT initialized.\n");
}

// Initialize APIC timer
void apic_timer_init(void) {
    console_write("Initializing APIC timer...\n");
    
    // Calculate divisor for 100Hz frequency
    // This is a simplified calculation and would need to be calibrated in a real implementation
    uint32_t divisor = 1000000;  // This value would need calibration
    
    // Set timer initial count
    apic_write(APIC_TIMER_INITIAL, divisor);
    
    // Set timer divide configuration
    apic_write(APIC_TIMER_DIVIDE, 0x03);  // Divide by 16
    
    // Set timer LVT entry
    apic_write(APIC_LVT_TIMER, 32 | 0x20000);  // Vector 32, periodic mode
    
    console_write("APIC timer initialized.\n");
}

// Main timer initialization function
void timer_init(void) {
    console_write("Initializing timer...\n");
    
    // Initialize PIT
    pit_init();
    
    // Initialize APIC timer
    apic_timer_init();
    
    console_write("Timer initialization complete.\n");
}