#ifndef ELF_H
#define ELF_H

#include <stdint.h>

// ELF file header
#define EI_NIDENT 16

// ELF identification indices
#define EI_MAG0    0  // File identification
#define EI_MAG1    1  // File identification
#define EI_MAG2    2  // File identification
#define EI_MAG3    3  // File identification
#define EI_CLASS   4  // File class
#define EI_DATA    5  // Data encoding
#define EI_VERSION 6  // File version
#define EI_OSABI   7  // OS/ABI identification
#define EI_ABIVERSION 8  // ABI version
#define EI_PAD     9  // Start of padding bytes

// Magic numbers
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

// File class
#define ELFCLASSNONE 0  // Invalid class
#define ELFCLASS32   1  // 32-bit objects
#define ELFCLASS64   2  // 64-bit objects

// Data encoding
#define ELFDATANONE 0  // Invalid data encoding
#define ELFDATA2LSB 1  // Little endian
#define ELFDATA2MSB 2  // Big endian

// Object file type
#define ET_NONE   0  // No file type
#define ET_REL    1  // Relocatable file
#define ET_EXEC   2  // Executable file
#define ET_DYN    3  // Shared object file
#define ET_CORE   4  // Core file
#define ET_LOOS   0xfe00  // OS-specific range start
#define ET_HIOS   0xfeff  // OS-specific range end
#define ET_LOPROC 0xff00  // Processor-specific range start
#define ET_HIPROC 0xffff  // Processor-specific range end

// ELF header for 64-bit
typedef struct {
    unsigned char e_ident[EI_NIDENT];  // ELF identification
    uint16_t      e_type;              // Object file type
    uint16_t      e_machine;           // Machine type
    uint32_t      e_version;           // Object file version
    uint64_t      e_entry;             // Entry point address
    uint64_t      e_phoff;             // Program header offset
    uint64_t      e_shoff;             // Section header offset
    uint32_t      e_flags;             // Processor-specific flags
    uint16_t      e_ehsize;            // ELF header size
    uint16_t      e_phentsize;         // Size of program header entry
    uint16_t      e_phnum;             // Number of program header entries
    uint16_t      e_shentsize;         // Size of section header entry
    uint16_t      e_shnum;             // Number of section header entries
    uint16_t      e_shstrndx;          // Section name string table index
} Elf64_Ehdr;

// Program header for 64-bit
typedef struct {
    uint32_t      p_type;    // Type of segment
    uint32_t      p_flags;   // Segment attributes
    uint64_t      p_offset;  // Offset in file
    uint64_t      p_vaddr;   // Virtual address in memory
    uint64_t      p_paddr;   // Physical address in memory (ignored)
    uint64_t      p_filesz;  // Size of segment in file
    uint64_t      p_memsz;   // Size of segment in memory
    uint64_t      p_align;   // Alignment of segment
} Elf64_Phdr;

// Segment types
#define PT_NULL    0  // Unused entry
#define PT_LOAD    1  // Loadable segment
#define PT_DYNAMIC 2  // Dynamic linking information
#define PT_INTERP  3  // Interpreter pathname
#define PT_NOTE    4  // Auxiliary information
#define PT_SHLIB   5  // Reserved
#define PT_PHDR    6  // Program header table
#define PT_LOOS    0x60000000  // OS-specific range start
#define PT_HIOS    0x6fffffff  // OS-specific range end
#define PT_LOPROC  0x70000000  // Processor-specific range start
#define PT_HIPROC  0x7fffffff  // Processor-specific range end

// Segment permissions
#define PF_X 0x1  // Execute permission
#define PF_W 0x2  // Write permission
#define PF_R 0x4  // Read permission

#endif // ELF_H