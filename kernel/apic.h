// kernel/apic.h
#ifndef APIC_H
#define APIC_H

#include <stdint.h>

// APIC Base Address MSR
#define IA32_APIC_BASE_MSR 0x1B

// APIC Registers (offsets from base address)
#define APIC_ID             0x20
#define APIC_VERSION        0x30
#define APIC_TASK_PRIORITY  0x80
#define APIC_ARBITRATION    0x90
#define APIC_PROCESSOR_PRIORITY 0xA0
#define APIC_EOI            0xB0
#define APIC_LOGICAL_DEST   0xD0
#define APIC_DEST_FORMAT    0xE0
#define APIC_SPURIOUS_INT   0xF0
#define APIC_ISR_BASE       0x100
#define APIC_TMR_BASE       0x180
#define APIC_IRR_BASE       0x200
#define APIC_ERROR_STATUS   0x280
#define APIC_ICR_LOW        0x300
#define APIC_ICR_HIGH       0x310
#define APIC_LVT_TIMER      0x320
#define APIC_LVT_THERMAL    0x330
#define APIC_LVT_PERF       0x340
#define APIC_LVT_LINT0      0x350
#define APIC_LVT_LINT1      0x360
#define APIC_LVT_ERROR      0x370
#define APIC_TIMER_INITIAL  0x380
#define APIC_TIMER_CURRENT  0x390
#define APIC_TIMER_DIVIDE   0x3E0

// APIC Base Address MSR bits
#define APIC_BASE_MSR_ENABLE 0x800

// Spurious Interrupt Vector Register bits
#define APIC_SPURIOUS_ENABLE 0x100

// LVT bits
#define APIC_LVT_MASKED     0x10000

// ICR bits
#define APIC_ICR_INIT       0x500
#define APIC_ICR_STARTUP    0x600
#define APIC_ICR_DELIVERY_STATUS 0x1000
#define APIC_ICR_LEVEL_ASSERT 0x4000
#define APIC_ICR_TRIGGER_LEVEL 0x8000
#define APIC_ICR_DEST_SELF  0x40000
#define APIC_ICR_DEST_ALL   0x80000
#define APIC_ICR_DEST_ALL_BUT_SELF 0xC0000

// IOAPIC Registers
#define IOAPIC_ID           0x00
#define IOAPIC_VERSION      0x01
#define IOAPIC_ARB          0x02
#define IOAPIC_REDTBL_BASE  0x10
#define IOAPIC_REDTBL_SIZE  0x17

// Function prototypes
void apic_init(void);
void lapic_init(void);
void ioapic_init(void);
void apic_eoi(void);
void apic_write(uint32_t reg, uint32_t value);
uint32_t apic_read(uint32_t reg);
void ioapic_write(uint32_t reg, uint32_t value);
uint32_t ioapic_read(uint32_t reg);
void ioapic_set_irq_redirect(uint8_t irq, uint8_t vector, uint32_t flags);

#endif