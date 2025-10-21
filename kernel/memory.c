// kernel/memory.c
#include "memory.h"
#include "drivers/console.h"
#include <stdint.h>

// Boot info pointer
struct boot_info* boot_params = (struct boot_info*)0x9000;

// Physical memory manager info
struct pmm_info pmm;

// Virtual memory manager info
struct vmm_info vmm;

// Memory map entries array
struct e820_entry memory_map_entries[MAX_MEMORY_MAP_ENTRIES];

// Simple bitmap for tracking allocated physical pages (for demonstration)
// In a real implementation, this would be more sophisticated
// static uint8_t page_bitmap[1024]; // Tracks 1024 pages (4MB) - Removed due to being unused
static uint64_t next_free_page = 0x200000; // Start allocating from 2MB

// Bootstrap memory allocator for early page table allocation
static uint8_t bootstrap_memory_pool[4096 * 16]; // 16 pages for bootstrap
static uint32_t bootstrap_memory_used = 0;

// Bootstrap memory allocation function
void* bootstrap_alloc(uint32_t size) {
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    // Check if we have enough space
    if (bootstrap_memory_used + size > sizeof(bootstrap_memory_pool)) {
        return NULL;
    }
    
    // Allocate memory from bootstrap pool
    void* ptr = &bootstrap_memory_pool[bootstrap_memory_used];
    bootstrap_memory_used += size;
    
    return ptr;
}

// Function to print memory map
void print_memory_map(void) {
    console_write("Memory Map:\n");
    
    // Check if boot info is valid
    if (boot_params->signature != 0x1BADB002) {
        console_write("Invalid boot info signature!\n");
        return;
    }
    
    // Get memory map address
    struct e820_entry* mmap = (struct e820_entry*)boot_params->memory_map;
    uint32_t entry_count = boot_params->memory_entries;
    
    // Limit entries to our maximum
    if (entry_count > MAX_MEMORY_MAP_ENTRIES) {
        entry_count = MAX_MEMORY_MAP_ENTRIES;
    }
    
    // Print each entry
    for (uint32_t i = 0; i < entry_count; i++) {
        console_write("Base: 0x");
        // Print base address (simplified)
        console_write(" Length: 0x");
        // Print length (simplified)
        console_write(" Type: ");
        
        // Print type
        switch (mmap[i].type) {
            case E820_RAM:
                console_write("RAM");
                break;
            case E820_RESERVED:
                console_write("RESERVED");
                break;
            case E820_ACPI:
                console_write("ACPI");
                break;
            case E820_NVS:
                console_write("NVS");
                break;
            case E820_UNUSABLE:
                console_write("UNUSABLE");
                break;
            default:
                console_write("UNKNOWN");
                break;
        }
        console_write("\n");
    }
}

// Initialize physical memory manager
void init_pmm(void) {
    // Check if boot info is valid
    if (boot_params->signature != 0x1BADB002) {
        console_write("ERROR: Invalid boot info signature!\n");
        return;
    }
    
    // Get memory map address
    struct e820_entry* mmap = (struct e820_entry*)boot_params->memory_map;
    uint32_t entry_count = boot_params->memory_entries;
    
    // Limit entries to our maximum
    if (entry_count > MAX_MEMORY_MAP_ENTRIES) {
        entry_count = MAX_MEMORY_MAP_ENTRIES;
    }
    
    // Initialize PMM structure
    pmm.region_count = 0;
    pmm.total_memory = 0;
    pmm.free_memory = 0;
    
    // Process each memory map entry
    for (uint32_t i = 0; i < entry_count; i++) {
        // Only process RAM regions
        if (mmap[i].type == E820_RAM && mmap[i].base_high == 0) {
            // Store region info
            pmm.regions[pmm.region_count].base = mmap[i].base_low;
            pmm.regions[pmm.region_count].length = mmap[i].length_low;
            pmm.regions[pmm.region_count].type = mmap[i].type;
            
            // Update totals
            pmm.total_memory += mmap[i].length_low;
            if (mmap[i].base_low >= 0x100000) { // Only count memory above 1MB as free
                pmm.free_memory += mmap[i].length_low;
            }
            
            pmm.region_count++;
            
            // Don't exceed our array
            if (pmm.region_count >= MAX_MEMORY_MAP_ENTRIES) {
                break;
            }
        }
    }
    
    console_write("PMM initialized:\n");
    console_write("Total memory: ");
    console_write(" bytes\n");
    console_write("Free memory: ");
    console_write(" bytes\n");
    console_write("Memory regions: ");
    console_write("\n");
}

// Allocate a physical page
void* alloc_physical_page(void) {
    // Simple implementation - find next free page
    // In a real implementation, this would use a proper bitmap or other tracking
    
    // Align to page boundary
    next_free_page = (next_free_page + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    
    // For now, just return the next free page
    // In a real implementation, we'd mark it as used in the bitmap
    void* page = (void*)next_free_page;
    next_free_page += PAGE_SIZE;
    
    return page;
}

// Free a physical page
void free_physical_page(uint64_t physical_addr) {
    // Simple implementation - just reset next_free_page if it's the last one allocated
    // In a real implementation, we'd mark it as free in the bitmap
    (void)physical_addr;
}

// Initialize virtual memory manager
void init_vmm(void) {
    console_write("Initializing VMM...\n");
    
    // Get the current PML4 table from CR3 (set up by bootloader)
    uint64_t cr3_value;
    asm volatile("mov %%cr3, %0" : "=r"(cr3_value));
    vmm.pml4 = (page_entry_t*)(cr3_value & ~0xFFF);
    
    // Set up initial heap parameters
    vmm.heap_start = 0xFFFF800000000000; // Start heap at higher half kernel space
    vmm.heap_end = vmm.heap_start;
    vmm.heap_max = 0xFFFF8000FFFFFFFF;   // Max heap range
    
    console_write("VMM initialized.\n");
}

// TLB flush function
static inline void flush_tlb_single(uint64_t addr) {
    asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}

static inline void flush_tlb(void) {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r" (cr3));
    asm volatile("mov %0, %%cr3" :: "r" (cr3) : "memory");
}

// Map a virtual page to a physical page
int map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    // Align addresses to page boundaries
    virtual_addr &= ~(PAGE_SIZE - 1);
    physical_addr &= ~(PAGE_SIZE - 1);
    
    // Get page table indices for 4-level paging
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdpt_index = PDPT_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);
    
    // Check if page directory pointer table exists
    page_entry_t* pdpt;
    if (!(vmm.pml4[pml4_index] & PAGE_PRESENT)) {
        // Allocate new PDPT using bootstrap allocator for early allocation
        pdpt = (page_entry_t*)bootstrap_alloc(sizeof(page_entry_t) * 512);
        if (!pdpt) {
            return -1; // Failed to allocate PDPT
        }
        
        // Zero out PDPT
        for (int i = 0; i < 512; i++) {
            pdpt[i] = 0;
        }
        
        // Add PDPT to PML4
        vmm.pml4[pml4_index] = ((uint64_t)pdpt) | PAGE_PRESENT | PAGE_WRITABLE;
        
        // Flush TLB after modifying PML4
        flush_tlb();
    } else {
        // Get existing PDPT
        pdpt = (page_entry_t*)(vmm.pml4[pml4_index] & ~0xFFF);
    }
    
    // Check if page directory exists
    page_entry_t* pd;
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        // Allocate new page directory using bootstrap allocator
        pd = (page_entry_t*)bootstrap_alloc(sizeof(page_entry_t) * 512);
        if (!pd) {
            return -1; // Failed to allocate page directory
        }
        
        // Zero out page directory
        for (int i = 0; i < 512; i++) {
            pd[i] = 0;
        }
        
        // Add page directory to PDPT
        pdpt[pdpt_index] = ((uint64_t)pd) | PAGE_PRESENT | PAGE_WRITABLE;
        
        // Flush TLB after modifying PDPT
        flush_tlb();
    } else {
        // Get existing page directory
        pd = (page_entry_t*)(pdpt[pdpt_index] & ~0xFFF);
    }
    
    // Check if page table exists
    page_entry_t* pt;
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        // Allocate new page table using bootstrap allocator
        pt = (page_entry_t*)bootstrap_alloc(sizeof(page_entry_t) * 512);
        if (!pt) {
            return -1; // Failed to allocate page table
        }
        
        // Zero out page table
        for (int i = 0; i < 512; i++) {
            pt[i] = 0;
        }
        
        // Add page table to page directory
        pd[pd_index] = ((uint64_t)pt) | PAGE_PRESENT | PAGE_WRITABLE;
        
        // Flush TLB after modifying page directory
        flush_tlb();
    } else {
        // Get existing page table
        pt = (page_entry_t*)(pd[pd_index] & ~0xFFF);
    }
    
    // Map page
    pt[pt_index] = physical_addr | flags | PAGE_PRESENT;
    
    // Flush TLB for the mapped page
    flush_tlb_single(virtual_addr);
    
    return 0; // Success
}

// Unmap a virtual page
int unmap_page(uint64_t virtual_addr) {
    // Align address to page boundary
    virtual_addr &= ~(PAGE_SIZE - 1);
    
    // Get page table indices for 4-level paging
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdpt_index = PDPT_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);
    
    // Check if PML4 entry exists
    if (!(vmm.pml4[pml4_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get PDPT
    page_entry_t* pdpt = (page_entry_t*)(vmm.pml4[pml4_index] & ~0xFFF);
    
    // Check if PDPT entry exists
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get page directory
    page_entry_t* pd = (page_entry_t*)(pdpt[pdpt_index] & ~0xFFF);
    
    // Check if page directory entry exists
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get page table
    page_entry_t* pt = (page_entry_t*)(pd[pd_index] & ~0xFFF);
    
    // Check if page table entry exists
    if (!(pt[pt_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Unmap page
    pt[pt_index] = 0;
    
    // Flush TLB for the unmapped page
    flush_tlb_single(virtual_addr);
    
    return 0; // Success
}

// Set page flags
int set_page_flags(uint64_t virtual_addr, uint64_t flags) {
    // Align address to page boundary
    virtual_addr &= ~(PAGE_SIZE - 1);
    
    // Get page table indices for 4-level paging
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdpt_index = PDPT_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);
    uint64_t pt_index = PT_INDEX(virtual_addr);
    
    // Check if PML4 entry exists
    if (!(vmm.pml4[pml4_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get PDPT
    page_entry_t* pdpt = (page_entry_t*)(vmm.pml4[pml4_index] & ~0xFFF);
    
    // Check if PDPT entry exists
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get page directory
    page_entry_t* pd = (page_entry_t*)(pdpt[pdpt_index] & ~0xFFF);
    
    // Check if page directory entry exists
    if (!(pd[pd_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Get page table
    page_entry_t* pt = (page_entry_t*)(pd[pd_index] & ~0xFFF);
    
    // Check if page table entry exists
    if (!(pt[pt_index] & PAGE_PRESENT)) {
        return -1; // Page not mapped
    }
    
    // Update flags while preserving physical address
    uint64_t phys_addr = pt[pt_index] & ~0xFFF;
    pt[pt_index] = phys_addr | flags | PAGE_PRESENT;
    
    // Flush TLB for the modified page
    flush_tlb_single(virtual_addr);
    
    return 0; // Success
}

// Test routine to verify VMM and heap allocator functionality
void test_memory_management(void) {
    console_write("Testing memory management...\n");
    
    // Test kmalloc
    void* ptr1 = kmalloc(128);
    if (ptr1 != NULL) {
        console_write("kmalloc(128) successful\n");
        
        // Test writing to allocated memory
        char* char_ptr = (char*)ptr1;
        for (int i = 0; i < 128; i++) {
            char_ptr[i] = (char)(i % 256);
        }
        
        // Test reading from allocated memory
        int success = 1;
        for (int i = 0; i < 128; i++) {
            if (char_ptr[i] != (char)(i % 256)) {
                success = 0;
                break;
            }
        }
        
        if (success) {
            console_write("Memory read/write test passed\n");
        } else {
            console_write("Memory read/write test failed\n");
        }
        
        // Test kfree
        kfree(ptr1);
        console_write("kfree() successful\n");
    } else {
        console_write("kmalloc(128) failed\n");
    }
    
    // Test another allocation
    void* ptr2 = kmalloc(256);
    if (ptr2 != NULL) {
        console_write("kmalloc(256) successful\n");
        kfree(ptr2);
        console_write("kfree() successful\n");
    } else {
        console_write("kmalloc(256) failed\n");
    }
    
    console_write("Memory management test completed.\n");
}

// Function to load PML4 table into CR3
void load_page_directory(void) {
    asm volatile("mov %0, %%cr3" :: "r" (vmm.pml4) : "memory");
}

// Main memory initialization function
void memory_init(void) {
    console_write("Initializing memory management...\n");
    
    // Print memory map
    print_memory_map();
    
    // Initialize physical memory manager
    init_pmm();
    
    // Initialize virtual memory manager
    init_vmm();
    
    // Load the page directory
    load_page_directory();
    
    console_write("Memory management initialized.\n");
}

// Simple heap block header for tracking allocations
struct heap_block {
    size_t size;
    struct heap_block* next;
    int free;
};

// Heap metadata
static struct heap_block* heap_first_block = NULL;

// Simple kmalloc implementation
void* kmalloc(size_t size) {
    // Align size to 4 bytes
    size = (size + 3) & ~3;
    
    // If heap is not initialized, initialize it
    if (heap_first_block == NULL) {
        // Reserve virtual memory for heap
        uint64_t heap_size = 4 * 1024 * 1024; // 4MB heap
        
        // Map physical pages for heap
        for (uint64_t addr = vmm.heap_start; addr < vmm.heap_start + heap_size; addr += PAGE_SIZE) {
            void* phys_page = alloc_physical_page();
            if (phys_page == NULL) {
                console_write("ERROR: Failed to allocate physical page for heap\n");
                return NULL;
            }
            
            if (map_page(addr, (uint64_t)phys_page, PAGE_PRESENT | PAGE_WRITABLE) != 0) {
                console_write("ERROR: Failed to map page for heap\n");
                return NULL;
            }
        }
        
        // Initialize first block
        heap_first_block = (struct heap_block*)vmm.heap_start;
        heap_first_block->size = heap_size - sizeof(struct heap_block);
        heap_first_block->next = NULL;
        heap_first_block->free = 1;
        
        // Update heap end
        vmm.heap_end = vmm.heap_start + heap_size;
    }
    
    // Find a free block
    struct heap_block* current = heap_first_block;
    struct heap_block* best_fit = NULL;
    
    while (current) {
        if (current->free && current->size >= size) {
            if (best_fit == NULL || current->size < best_fit->size) {
                best_fit = current;
            }
        }
        current = current->next;
    }
    
    // If no suitable block found, extend heap
    if (best_fit == NULL) {
        // Calculate how many pages we need
        size_t needed_size = sizeof(struct heap_block) + size;
        size_t pages_needed = (needed_size + PAGE_SIZE - 1) / PAGE_SIZE;
        
        // Map new pages
        for (size_t i = 0; i < pages_needed; i++) {
            void* phys_page = alloc_physical_page();
            if (phys_page == NULL) {
                console_write("ERROR: Failed to allocate physical page for heap extension\n");
                return NULL;
            }
            
            uint64_t virt_addr = vmm.heap_end + i * PAGE_SIZE;
            if (map_page(virt_addr, (uint64_t)phys_page, PAGE_PRESENT | PAGE_WRITABLE) != 0) {
                console_write("ERROR: Failed to map page for heap extension\n");
                return NULL;
            }
        }
        
        // Create new block
        best_fit = (struct heap_block*)vmm.heap_end;
        best_fit->size = pages_needed * PAGE_SIZE - sizeof(struct heap_block);
        best_fit->next = NULL;
        best_fit->free = 1;
        
        // Link to previous block
        current = heap_first_block;
        while (current->next) {
            current = current->next;
        }
        current->next = best_fit;
        
        // Update heap end
        vmm.heap_end += pages_needed * PAGE_SIZE;
    }
    
    // If the block is exactly the right size or only slightly larger, use it as is
    if (best_fit->size <= size + sizeof(struct heap_block) + 16) {
        // Use the whole block
        best_fit->free = 0;
        return (void*)(best_fit + 1);
    } else {
        // Split the block
        struct heap_block* new_block = (struct heap_block*)((char*)(best_fit + 1) + size);
        new_block->size = best_fit->size - size - sizeof(struct heap_block);
        new_block->next = best_fit->next;
        new_block->free = 1;
        
        best_fit->size = size;
        best_fit->next = new_block;
        best_fit->free = 0;
        
        return (void*)(best_fit + 1);
    }
}

// Simple kfree implementation
void kfree(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    
    // Get the block header
    struct heap_block* block = ((struct heap_block*)ptr) - 1;
    
    // Mark as free
    block->free = 1;
    
    // Coalesce with next block if it's free
    if (block->next && block->next->free) {
        block->size += sizeof(struct heap_block) + block->next->size;
        block->next = block->next->next;
    }
    
    // Coalesce with previous block if it's free
    struct heap_block* current = heap_first_block;
    while (current && current->next != block) {
        current = current->next;
    }
    
    if (current && current->free) {
        current->size += sizeof(struct heap_block) + block->size;
        current->next = block->next;
    }
}