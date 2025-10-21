#include "elf.h"
#include "loader.h"
#include "drivers/console.h"
#include "memory.h"
#include "fs/vfs.h"
#include "process.h"
#include <stdint.h>
#include <string.h>

// Load an ELF executable from a file
int elf_load(const char* filename, struct process* proc) {
    console_write("Loading ELF executable: ");
    console_write(filename);
    console_write("\n");
    
    // Open the file
    struct vfs_file file;
    if (!vfs_open(&file, filename, VFS_MODE_READ)) {
        console_write("ERROR: Failed to open file\n");
        return 0;
    }
    
    // Read ELF header
    Elf64_Ehdr ehdr;
    uint32_t bytes_read;
    if (!vfs_read(&file, &ehdr, sizeof(Elf64_Ehdr), &bytes_read) || bytes_read != sizeof(Elf64_Ehdr)) {
        console_write("ERROR: Failed to read ELF header\n");
        vfs_close(&file);
        return 0;
    }
    
    // Verify ELF magic numbers
    if (ehdr.e_ident[EI_MAG0] != ELFMAG0 || 
        ehdr.e_ident[EI_MAG1] != ELFMAG1 || 
        ehdr.e_ident[EI_MAG2] != ELFMAG2 || 
        ehdr.e_ident[EI_MAG3] != ELFMAG3) {
        console_write("ERROR: Invalid ELF magic numbers\n");
        vfs_close(&file);
        return 0;
    }
    
    // Verify ELF class (64-bit)
    if (ehdr.e_ident[EI_CLASS] != ELFCLASS64) {
        console_write("ERROR: Not a 64-bit ELF file\n");
        vfs_close(&file);
        return 0;
    }
    
    // Verify file type (executable)
    if (ehdr.e_type != ET_EXEC) {
        console_write("ERROR: Not an executable ELF file\n");
        vfs_close(&file);
        return 0;
    }
    
    // Set entry point
    proc->entry_point = ehdr.e_entry;
    
    // Read program headers
    if (ehdr.e_phnum == 0) {
        console_write("ERROR: No program headers\n");
        vfs_close(&file);
        return 0;
    }
    
    // Seek to program header table
    if (!vfs_seek(&file, ehdr.e_phoff, 0)) {
        console_write("ERROR: Failed to seek to program headers\n");
        vfs_close(&file);
        return 0;
    }
    
    // Read all program headers
    uint32_t ph_size = ehdr.e_phnum * ehdr.e_phentsize;
    Elf64_Phdr* phdrs = (Elf64_Phdr*)kmalloc(ph_size);
    if (!phdrs) {
        console_write("ERROR: Failed to allocate memory for program headers\n");
        vfs_close(&file);
        return 0;
    }
    
    if (!vfs_read(&file, phdrs, ph_size, &bytes_read) || bytes_read != ph_size) {
        console_write("ERROR: Failed to read program headers\n");
        kfree(phdrs);
        vfs_close(&file);
        return 0;
    }
    
    // Process each program header
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf64_Phdr* phdr = &phdrs[i];
        
        // Only process loadable segments
        if (phdr->p_type != PT_LOAD) {
            continue;
        }
        
        // Calculate number of pages needed
        uint64_t start_page = phdr->p_vaddr & ~(PAGE_SIZE - 1);
        uint64_t end_page = (phdr->p_vaddr + phdr->p_memsz + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        uint64_t num_pages = (end_page - start_page) / PAGE_SIZE;
        
        // Allocate and map pages
        for (uint64_t page = 0; page < num_pages; page++) {
            void* phys_page = alloc_physical_page();
            if (!phys_page) {
                console_write("ERROR: Failed to allocate physical page\n");
                kfree(phdrs);
                vfs_close(&file);
                return 0;
            }
            
            uint64_t virt_addr = start_page + (page * PAGE_SIZE);
            
            // Determine page flags
            uint64_t flags = PAGE_PRESENT;
            if (phdr->p_flags & PF_W) {
                flags |= PAGE_WRITABLE;
            }
            if (virt_addr >= 0x100000000) { // User space address
                flags |= PAGE_USER;
            }
            
            // Map the page
            if (map_page(virt_addr, (uint64_t)phys_page, flags) != 0) {
                console_write("ERROR: Failed to map page\n");
                kfree(phdrs);
                vfs_close(&file);
                return 0;
            }
        }
        
        // Seek to segment data in file
        if (!vfs_seek(&file, phdr->p_offset, 0)) {
            console_write("ERROR: Failed to seek to segment data\n");
            kfree(phdrs);
            vfs_close(&file);
            return 0;
        }
        
        // Read segment data
        if (phdr->p_filesz > 0) {
            // We need to read directly into the virtual memory
            // For simplicity, we'll read into a temporary buffer and copy
            uint8_t* buffer = (uint8_t*)kmalloc(phdr->p_filesz);
            if (!buffer) {
                console_write("ERROR: Failed to allocate buffer for segment data\n");
                kfree(phdrs);
                vfs_close(&file);
                return 0;
            }
            
            if (!vfs_read(&file, buffer, phdr->p_filesz, &bytes_read) || bytes_read != phdr->p_filesz) {
                console_write("ERROR: Failed to read segment data\n");
                kfree(buffer);
                kfree(phdrs);
                vfs_close(&file);
                return 0;
            }
            
            // Copy data to virtual memory
            // This is a simplified implementation - in a real OS, we'd need to access
            // the virtual memory through the process's page table
            memcpy((void*)phdr->p_vaddr, buffer, phdr->p_filesz);
            
            kfree(buffer);
        }
        
        // Zero out BSS section (if memsz > filesz)
        if (phdr->p_memsz > phdr->p_filesz) {
            uint64_t bss_start = phdr->p_vaddr + phdr->p_filesz;
            uint64_t bss_size = phdr->p_memsz - phdr->p_filesz;
            memset((void*)bss_start, 0, bss_size);
        }
    }
    
    // Clean up
    kfree(phdrs);
    vfs_close(&file);
    
    console_write("ELF executable loaded successfully\n");
    return 1;
}