// kernel/apic.c
#include "apic.h"
#include "drivers/port_io.h"
#include "drivers/console.h"
#include "memory.h"
#include <stdint.h>

// LAPIC and IOAPIC base addresses (these will be detected during initialization)
static uint64_t lapic_base = 0;
static uint64_t ioapic_base = 0;

// Function to read from LAPIC register
uint32_t apic_read(uint32_t reg) {
    return *((volatile uint32_t*)(lapic_base + reg));
}

// Function to write to LAPIC register
void apic_write(uint32_t reg, uint32_t value) {
    *((volatile uint32_t*)(lapic_base + reg)) = value;
}

// Function to read from IOAPIC register
uint32_t ioapic_read(uint32_t reg) {
    // Write register index to IOAPIC index register
    *((volatile uint32_t*)ioapic_base) = reg;
    // Read value from IOAPIC data register
    return *((volatile uint32_t*)(ioapic_base + 0x10));
}

// Function to write to IOAPIC register
void ioapic_write(uint32_t reg, uint32_t value) {
    // Write register index to IOAPIC index register
    *((volatile uint32_t*)ioapic_base) = reg;
    // Write value to IOAPIC data register
    *((volatile uint32_t*)(ioapic_base + 0x10)) = value;
}

// Function to send End of Interrupt to LAPIC
void apic_eoi(void) {
    apic_write(APIC_EOI, 0);
}

// Function to detect and initialize LAPIC
void lapic_init(void) {
    console_write("Initializing LAPIC...\n");
    
    // Read APIC base address from MSR
    uint32_t msr_low, msr_high;
    asm volatile("rdmsr" : "=a"(msr_low), "=d"(msr_high) : "c"(IA32_APIC_BASE_MSR));
    
    // Extract base address
    lapic_base = ((uint64_t)(msr_high & 0xF) << 32) | (msr_low & 0xFFFFF000);
    
    console_write("LAPIC base address: 0x");
    // Print base address (simplified)
    console_write("\n");
    
    // Enable LAPIC by setting bit 8 in Spurious Interrupt Vector Register
    apic_write(APIC_SPURIOUS_INT, apic_read(APIC_SPURIOUS_INT) | APIC_SPURIOUS_ENABLE | 0xFF);
    
    // Set task priority to 0 (accept all interrupts)
    apic_write(APIC_TASK_PRIORITY, 0);
    
    console_write("LAPIC initialized.\n");
}

// Function to initialize IOAPIC
void ioapic_init(void) {
    console_write("Initializing IOAPIC...\n");
    
    // For now, we'll use a common default address for IOAPIC
    // In a real implementation, this should be detected from the MADT table
    ioapic_base = 0xFEC00000;
    
    console_write("IOAPIC base address: 0x");
    // Print base address (simplified)
    console_write("\n");
    
    // Read IOAPIC ID and version
    // uint32_t ioapic_id = ioapic_read(IOAPIC_ID);
    // uint32_t ioapic_version = ioapic_read(IOAPIC_VERSION);
    
    console_write("IOAPIC ID: 0x");
    // Print ID (simplified)
    console_write("\n");
    console_write("IOAPIC Version: 0x");
    // Print version (simplified)
    console_write("\n");
    
    console_write("IOAPIC initialized.\n");
}

// Function to set up IRQ redirection in IOAPIC
void ioapic_set_irq_redirect(uint8_t irq, uint8_t vector, uint32_t flags) {
    // Calculate register offset for redirection table entry
    uint32_t reg = IOAPIC_REDTBL_BASE + irq * 2;
    
    // Read current values
    uint32_t low = ioapic_read(reg);
    uint32_t high = ioapic_read(reg + 1);
    
    // Set interrupt vector
    low &= ~0xFF;
    low |= vector;
    
    // Set flags
    low &= ~0x8F00;  // Clear previous flags
    low |= flags;    // Set new flags
    
    // Set target APIC ID (for now, send to BSP)
    high &= ~0xFF000000;
    high |= 0;  // BSP APIC ID
    
    // Write back values
    ioapic_write(reg, low);
    ioapic_write(reg + 1, high);
}

// Main APIC initialization function
void apic_init(void) {
    console_write("Initializing APIC...\n");
    
    // Initialize LAPIC
    lapic_init();
    
    // Initialize IOAPIC
    ioapic_init();
    
    // Set up IRQ redirections
    // IRQ 0 (PIT) -> Vector 32
    ioapic_set_irq_redirect(0, 32, 0);
    
    // IRQ 1 (Keyboard) -> Vector 33
    ioapic_set_irq_redirect(1, 33, 0);
    
    console_write("APIC initialization complete.\n");
}