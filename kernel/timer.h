// kernel/timer.h
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

// Timer frequency (Hz)
#define TIMER_FREQUENCY 100

// Function prototypes
void timer_init(void);
void pit_init(void);
void timer_callback(void);
uint32_t get_tick_count(void);
void sleep(uint32_t milliseconds);

// For APIC timer (to be implemented later)
void apic_timer_init(void);

#endif