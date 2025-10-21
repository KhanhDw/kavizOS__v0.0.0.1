#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

// Forward declaration
struct process;

// Function prototypes for ELF loading
int elf_load(const char* filename, struct process* proc);

#endif // LOADER_H