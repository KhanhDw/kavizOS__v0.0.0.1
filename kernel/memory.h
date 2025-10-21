// kernel/memory.h
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

// E820 memory map entry structure
struct e820_entry {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed));

// Memory types
#define E820_RAM        1
#define E820_RESERVED   2
#define E820_ACPI       3
#define E820_NVS        4
#define E820_UNUSABLE   5

// Boot info structure
struct boot_info {
    uint32_t signature;
    uint64_t memory_map;
    uint32_t memory_entries;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t cmdline_size;
} __attribute__((packed));

// Physical memory manager
#define MAX_MEMORY_MAP_ENTRIES 128
#define PAGE_SIZE 4096

// Memory region structure for tracking
struct memory_region {
    uint32_t base;
    uint32_t length;
    uint32_t type;
};

// Physical memory manager structure
struct pmm_info {
    struct memory_region regions[MAX_MEMORY_MAP_ENTRIES];
    uint32_t region_count;
    uint32_t total_memory;
    uint32_t free_memory;
};

// Page table entry flags
#define PAGE_PRESENT    0x01
#define PAGE_WRITABLE   0x02
#define PAGE_USER       0x04
#define PAGE_WRITE_THROUGH  0x08
#define PAGE_CACHE_DISABLE  0x10
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40
#define PAGE_GLOBAL     0x100

// Virtual address components for 64-bit paging (4-level)
// Only using 48 bits of virtual address as per x86-64 specification
#define PML4_INDEX(vaddr) (((vaddr) >> 39) & 0x1FF)
#define PDPT_INDEX(vaddr) (((vaddr) >> 30) & 0x1FF)
#define PD_INDEX(vaddr) (((vaddr) >> 21) & 0x1FF)
#define PT_INDEX(vaddr) (((vaddr) >> 12) & 0x1FF)
#define PAGE_OFFSET(vaddr) ((vaddr) & 0xFFF)

// Page table entry for 64-bit paging
typedef uint64_t page_entry_t;

// Virtual Memory Manager structure
struct vmm_info {
    page_entry_t* pml4;          // Current PML4 table
    uint64_t heap_start;         // Start of kernel heap
    uint64_t heap_end;           // End of kernel heap
    uint64_t heap_max;           // Maximum heap address
};

// Function prototypes
void memory_init(void);
void* kmalloc(size_t size);
void kfree(void* ptr);
void print_memory_map(void);
void init_pmm(void);

// VMM function prototypes
void init_vmm(void);
void load_page_directory(void);
int map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
int unmap_page(uint64_t virtual_addr);
int set_page_flags(uint64_t virtual_addr, uint64_t flags);
void* alloc_physical_page(void);
void free_physical_page(uint64_t physical_addr);

// Test function
void test_memory_management(void);

// External boot info
extern struct boot_info* boot_params;
extern struct vmm_info vmm;

#endif