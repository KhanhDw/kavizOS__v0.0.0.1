#!/bin/bash
# build.sh - Simple build script for Kaviz OS

# Clean previous build
echo "Cleaning previous build..."
rm -rf build/

# Create build directory
mkdir -p build/kernel/drivers

# Assemble bootloader stage 1
echo "Assembling bootloader stage 1..."
nasm -f bin boot/stage1/1_boot_sector.asm -o build/boot_sect.bin

# Assemble bootloader stage 2
echo "Assembling bootloader stage 2..."
nasm -f bin boot/stage2/2_main_bootloader.asm -o build/main_bootloader.bin

# Compile kernel C files
echo "Compiling kernel C files..."
gcc -ffreestanding -nostdlib -m64 -mcmodel=large -mno-red-zone -O2 -I./include -fno-pie -c -Wall -Wextra kernel/kernel_loader.c -o build/kernel_loader.o
gcc -ffreestanding -nostdlib -m64 -mcmodel=large -mno-red-zone -O2 -I./include -fno-pie -c -Wall -Wextra kernel/memory.c -o build/memory.o
gcc -ffreestanding -nostdlib -m64 -mcmodel=large -mno-red-zone -O2 -I./include -fno-pie -c -Wall -Wextra kernel/interrupt.c -o build/interrupt.o
gcc -ffreestanding -nostdlib -m64 -mcmodel=large -mno-red-zone -O2 -I./include -fno-pie -c -Wall -Wextra kernel/drivers/console.c -o build/console.o

# Assemble kernel assembly files
echo "Assembling kernel assembly files..."
nasm -f elf64 kernel/kernel_entry.asm -o build/kernel_entry.o
nasm -f elf64 kernel/interrupts.asm -o build/interrupts.o

# Link kernel
echo "Linking kernel..."
ld -m elf_x86_64 -T boot.ld -nostdlib build/kernel_entry.o build/kernel_loader.o build/memory.o build/interrupt.o build/interrupts.o build/console.o -o build/kernel.elf

# Convert kernel to binary
echo "Converting kernel to binary..."
objcopy -O binary build/kernel.elf build/kernel.bin

# Create disk image
echo "Creating disk image..."
dd if=/dev/zero of=build/os-image.img bs=512 count=2880
dd if=build/boot_sect.bin of=build/os-image.img bs=512 seek=0 conv=notrunc
dd if=build/main_bootloader.bin of=build/os-image.img bs=512 seek=1 conv=notrunc
dd if=build/kernel.bin of=build/os-image.img bs=512 seek=10 conv=notrunc

echo "Build complete! Disk image created at build/os-image.img"