# Makefile - Phiên bản đã sửa
ASM=nasm
CC=gcc
CFLAGS=-ffreestanding -nostdlib -m32 -O2 -I./include -fno-pie -c -Wall -Wextra
ASMFLAGS=-f elf32
LD=ld
LDFLAGS=-m elf_i386 -T boot.ld -nostdlib

# Thư mục
BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel
DRIVERS_DIR = $(KERNEL_DIR)/drivers
INCLUDE_DIR = include

# Tên tệp ảnh đĩa
IMAGE_FILE = $(BUILD_DIR)/os-image.img

# Tìm tất cả file nguồn C (LOẠI BỎ kernel_loader.c khỏi wildcard)
C_SOURCES = $(wildcard $(KERNEL_DIR)/*.c $(DRIVERS_DIR)/*.c)
C_SOURCES := $(filter-out $(KERNEL_DIR)/kernel_loader.c, $(C_SOURCES))

# Thêm kernel_loader.c riêng
KERNEL_LOADER_SOURCE = $(KERNEL_DIR)/kernel_loader.c

C_OBJECTS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
KERNEL_LOADER_OBJECT = $(BUILD_DIR)/kernel_loader.o

# Tìm tất cả file nguồn ASM
ASM_SOURCES = $(wildcard $(KERNEL_DIR)/*.asm $(DRIVERS_DIR)/*.asm)
ASM_OBJECTS = $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

# Tạo thư mục build nếu chưa tồn tại
$(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/$(KERNEL_DIR) $(BUILD_DIR)/$(DRIVERS_DIR))

# Target mặc định
all: $(IMAGE_FILE)

# --- QUY TẮC TẠO ẢNH ĐĨA ---
$(IMAGE_FILE): $(BUILD_DIR)/boot_sect.bin $(BUILD_DIR)/main_bootloader.bin $(BUILD_DIR)/kernel.bin
	@echo "🔨 Creating disk image: $@"

	# Tạo ảnh đĩa trống 1.44MB
	dd if=/dev/zero of=$@ bs=512 count=2880 status=none

	# Ghi boot sector (Sector 1)
	dd if=$(BUILD_DIR)/boot_sect.bin of=$@ bs=512 seek=0 conv=notrunc status=none

	# Ghi main bootloader (Sector 2)
	dd if=$(BUILD_DIR)/main_bootloader.bin of=$@ bs=512 seek=1 conv=notrunc status=none

	# Ghi kernel (Bắt đầu từ Sector 10)
	dd if=$(BUILD_DIR)/kernel.bin of=$@ bs=512 seek=9 conv=notrunc status=none

	@echo "✅ Disk image created: $@"

# --- BOOTLOADER BINARIES ---
$(BUILD_DIR)/boot_sect.bin: $(BOOT_DIR)/boot_sector.asm
	@echo "📦 Building boot sector: $@"
	$(ASM) -f bin -o $@ $<

$(BUILD_DIR)/main_bootloader.bin: $(BOOT_DIR)/main_bootloader.asm
	@echo "📦 Building main bootloader: $@"
	$(ASM) -f bin -o $@ $<

# --- KERNEL BINARY ---
$(BUILD_DIR)/kernel.bin: $(KERNEL_LOADER_OBJECT) $(C_OBJECTS) $(ASM_OBJECTS)
	@echo "🔗 Linking kernel: $@"
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel.elf $^
	objcopy -O binary $(BUILD_DIR)/kernel.elf $@
	@echo "📊 Kernel size: `stat -c%s $@` bytes"

# --- KERNEL LOADER (RIÊNG BIỆT) ---
$(KERNEL_LOADER_OBJECT): $(KERNEL_LOADER_SOURCE) $(INCLUDE_DIR)/boot.h
	@echo "🔨 Compiling kernel loader: $@"
	$(CC) $(CFLAGS) -o $@ $<

# --- COMPILE C SOURCES (KHÔNG BAO GỒM KERNEL_LOADER) ---
$(BUILD_DIR)/%.o: %.c
	@echo "🔨 Compiling C: $< -> $@"
	$(CC) $(CFLAGS) -o $@ $<

# --- COMPILE ASSEMBLY SOURCES ---
$(BUILD_DIR)/%.o: %.asm
	@echo "🔨 Assembling: $< -> $@"
	$(ASM) $(ASMFLAGS) -o $@ $<

# --- QEMU TARGETS ---
run: $(IMAGE_FILE)
	@echo "🚀 Starting QEMU..."
	qemu-system-x86_64 -fda $(IMAGE_FILE) -no-reboot -no-shutdown

debug: $(IMAGE_FILE)
	@echo "🐛 Starting QEMU in debug mode..."
	qemu-system-x86_64 -fda $(IMAGE_FILE) -S -s -no-reboot -no-shutdown &
	@echo "💡 Connect with: gdb -ex 'target remote localhost:1234' -ex 'symbol-file build/kernel.elf'"

# --- UTILITY TARGETS ---
clean:
	@echo "🧹 Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	@echo "✅ Clean completed"

disasm: $(BUILD_DIR)/kernel.elf
	@echo "📖 Disassembling kernel..."
	objdump -D -M intel $(BUILD_DIR)/kernel.elf | less

.PHONY: all run debug clean disasm