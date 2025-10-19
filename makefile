# =========================
# Kaviz OS - Makefile
# =========================

ASM         = nasm
CC          = gcc
LD          = ld
OBJCOPY     = objcopy

CFLAGS      = -ffreestanding -nostdlib -m32 -O2 -I./include -fno-pie -c -Wall -Wextra
ASMFLAGS    = -f elf32
LDFLAGS     = -m elf_i386 -T boot.ld -nostdlib

BUILD_DIR   = build
BOOT_DIR    = boot
KERNEL_DIR  = kernel
DRIVERS_DIR = $(KERNEL_DIR)/drivers
INCLUDE_DIR = include

IMAGE_FILE  = $(BUILD_DIR)/os-image.img

# =========================
# Source files
# =========================

C_SOURCES   = $(wildcard $(KERNEL_DIR)/*.c $(DRIVERS_DIR)/*.c)
C_SOURCES   := $(filter-out $(KERNEL_DIR)/kernel_loader.c, $(C_SOURCES))
ASM_SOURCES = $(wildcard $(KERNEL_DIR)/*.asm $(DRIVERS_DIR)/*.asm)

KERNEL_LOADER_SOURCE = $(KERNEL_DIR)/kernel_loader.c

C_OBJECTS           = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS         = $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))
KERNEL_LOADER_OBJECT = $(BUILD_DIR)/kernel_loader.o

$(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/$(KERNEL_DIR) $(BUILD_DIR)/$(DRIVERS_DIR))

# =========================
# Default target
# =========================

all: $(IMAGE_FILE)

# =========================
# Disk image build
# =========================

$(IMAGE_FILE): $(BUILD_DIR)/boot_sect.bin $(BUILD_DIR)/main_bootloader.bin $(BUILD_DIR)/kernel.bin
	@echo "[🧱] Creating disk image..."
	dd if=/dev/zero of=$@ bs=512 count=2880 status=none
	dd if=$(BUILD_DIR)/boot_sect.bin       of=$@ bs=512 seek=0 conv=notrunc status=none
	dd if=$(BUILD_DIR)/main_bootloader.bin of=$@ bs=512 seek=1 conv=notrunc status=none
	dd if=$(BUILD_DIR)/kernel.bin          of=$@ bs=512 seek=10 conv=notrunc status=none
	@echo "[✅] Disk image ready: $@"

# =========================
# Bootloader build
# =========================

$(BUILD_DIR)/boot_sect.bin: $(BOOT_DIR)/stage1/1_boot_sector.asm
	@echo "[ASM] Boot sector"
	$(ASM) -f bin -I $(BOOT_DIR)/stage1 -o $@ $<


$(BUILD_DIR)/main_bootloader.bin: $(BOOT_DIR)/stage2/2_main_bootloader.asm
	@echo "[ASM] Main bootloader"
	$(ASM) -f bin -I $(BOOT_DIR)/stage2 -o $@ $<


# =========================
# Kernel build
# =========================

$(BUILD_DIR)/kernel.bin: $(KERNEL_LOADER_OBJECT) $(C_OBJECTS) $(ASM_OBJECTS)
	@echo "[LD ] Linking kernel..."
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.elf $^
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel.elf $@
	@echo "[SIZE] Kernel: `stat -c%s $@` bytes"

$(KERNEL_LOADER_OBJECT): $(KERNEL_LOADER_SOURCE) $(INCLUDE_DIR)/boot.h
	@echo "[CC ] Kernel loader"
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.c
	@echo "[CC ] $<"
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: %.asm
	@echo "[ASM] $<"
	$(ASM) $(ASMFLAGS) -o $@ $<

# =========================
# Run / Debug / Clean
# =========================

run: $(IMAGE_FILE)
	@echo "[🚀] Running in QEMU..."
	qemu-system-x86_64 -fda $(IMAGE_FILE) -no-reboot -no-shutdown

debug: $(IMAGE_FILE)
	@echo "[🐞] QEMU debug mode..."
	qemu-system-x86_64 -fda $(IMAGE_FILE) -S -s -no-reboot -no-shutdown &
	@echo "[💡] Connect with: gdb -ex 'target remote localhost:1234' -ex 'symbol-file build/kernel.elf'"

clean:
	@echo "[🧹] Cleaning build..."
	rm -rf $(BUILD_DIR)
	@echo "[✅] Clean done."

disasm: $(BUILD_DIR)/kernel.elf
	objdump -D -M intel $(BUILD_DIR)/kernel.elf | less

.PHONY: all run debug clean disasm
