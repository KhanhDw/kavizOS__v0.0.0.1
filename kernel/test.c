#include "test.h"
#include "drivers/console.h"
#include "drivers/ata.h"
#include "fs/vfs.h"
#include "fs/fat.h"
#include "user_mode.h"
#include "process.h"
#include "elf.h"
#include "syscall.h"
#include "memory.h"
#include <stdint.h>

// Test ATA driver functionality
void test_ata_driver(void) {
    console_write("=== Testing ATA Driver ===\n");
    
    // Initialize ATA subsystem
    ata_init();
    
    // Try to read a sector
    uint8_t buffer[512];
    if (ata_read_sector(ATA_DEVICE_PRIMARY_MASTER, 0, buffer)) {
        console_write("Successfully read sector 0\n");
        
        // Print first 16 bytes of the sector
        console_write("First 16 bytes: ");
        for (int i = 0; i < 16; i++) {
            // Would need hex printing function
            console_write(" ");
        }
        console_write("\n");
    } else {
        console_write("Failed to read sector 0\n");
    }
    
    console_write("=== ATA Driver Test Complete ===\n\n");
}

// Test VFS functionality
void test_vfs(void) {
    console_write("=== Testing VFS ===\n");
    
    // Initialize VFS
    if (vfs_init() == 0) {
        console_write("VFS initialized successfully\n");
    } else {
        console_write("Failed to initialize VFS\n");
        return;
    }
    
    console_write("=== VFS Test Complete ===\n\n");
}

// Test FAT filesystem functionality
void test_fat(void) {
    console_write("=== Testing FAT Filesystem ===\n");
    
    // Create FAT filesystem structure
    struct fat_filesystem fat_fs;
    
    // Mount FAT filesystem
    if (fat_mount((struct vfs_filesystem*)&fat_fs, NULL)) {
        console_write("FAT filesystem mounted successfully\n");
        
        // Try to open a file
        struct vfs_file file;
        if (vfs_open(&file, "TEST.TXT", VFS_MODE_READ)) {
            console_write("Successfully opened TEST.TXT\n");
            vfs_close(&file);
        } else {
            console_write("Failed to open TEST.TXT\n");
        }
        
        // Unmount FAT filesystem
        fat_unmount((struct vfs_filesystem*)&fat_fs);
        console_write("FAT filesystem unmounted\n");
    } else {
        console_write("Failed to mount FAT filesystem\n");
    }
    
    console_write("=== FAT Filesystem Test Complete ===\n\n");
}

// Test user mode transition
void test_user_mode(void) {
    console_write("=== Testing User Mode Transition ===\n");
    
    // Initialize user mode support
    user_mode_init();
    
    console_write("User mode support initialized\n");
    console_write("=== User Mode Transition Test Complete ===\n\n");
}

// Test process creation
void test_process_creation(void) {
    console_write("=== Testing Process Creation ===\n");
    
    // Initialize process management
    process_init();
    
    // Create a test process
    pid_t pid = process_create(NULL, "test_process");
    if (pid != 0) {
        console_write("Successfully created test process\n");
    } else {
        console_write("Failed to create test process\n");
    }
    
    console_write("=== Process Creation Test Complete ===\n\n");
}

// Test ELF loading
void test_elf_loading(void) {
    console_write("=== Testing ELF Loading ===\n");
    
    // Initialize VFS
    if (vfs_init() != 0) {
        console_write("Failed to initialize VFS\n");
        return;
    }
    
    // Create a dummy process for testing
    struct process dummy_proc;
    
    // Try to load a test ELF file
    if (elf_load("TEST.ELF", &dummy_proc)) {
        console_write("Successfully loaded TEST.ELF\n");
    } else {
        console_write("Failed to load TEST.ELF\n");
    }
    
    console_write("=== ELF Loading Test Complete ===\n\n");
}

// Test system calls
void test_syscalls(void) {
    console_write("=== Testing System Calls ===\n");
    
    // Initialize system call interface
    syscall_init();
    
    console_write("System call interface initialized\n");
    
    // Test getpid syscall
    uint64_t pid = syscall_dispatch(SYSCALL_GETPID, 0, 0, 0, 0, 0, 0);
    console_write("getpid() returned: ");
    // Would need to print the PID
    
    console_write("\n=== System Calls Test Complete ===\n\n");
}

// Test user program execution
void test_user_program_execution(void) {
    console_write("=== Testing User Program Execution ===\n");
    
    // This would involve:
    // 1. Loading an ELF file from disk
    // 2. Creating a process for it
    // 3. Setting up the process's memory space
    // 4. Entering user mode to execute the program
    // 5. Handling system calls from the user program
    
    console_write("User program execution test placeholder\n");
    console_write("In a complete implementation, this would load and execute a user program\n");
    
    console_write("=== User Program Execution Test Complete ===\n\n");
}

// Run all tests
void run_tests(void) {
    console_write("=== Running All Tests ===\n\n");
    
    test_ata_driver();
    test_vfs();
    test_fat();
    test_user_mode();
    test_process_creation();
    test_elf_loading();
    test_syscalls();
    test_user_program_execution();
    
    console_write("=== All Tests Completed ===\n\n");
}