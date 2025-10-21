#ifndef TEST_H
#define TEST_H

#include <stdint.h>

// Function prototypes for testing
void test_ata_driver(void);
void test_vfs(void);
void test_fat(void);
void test_user_mode(void);
void test_process_creation(void);
void test_elf_loading(void);
void test_syscalls(void);
void test_user_program_execution(void);
void run_tests(void);

#endif // TEST_H