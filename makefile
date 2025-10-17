# Makefile - Phiên bản đã sửa lỗi và hoàn thiện
ASM=nasm
CC=gcc
# Đảm bảo bạn có cross-compiler i686-elf-gcc để có kết quả tốt nhất
# CC=i686-elf-gcc
CFLAGS=-ffreestanding -nostdlib -m32 -O2 -I./include -fno-pie -c

LD=ld
# LD=i686-elf-ld
LDFLAGS=-m elf_i386 -T boot.ld -nostdlib

# Thư mục
BUILD_DIR = build
BOOT_DIR = boot
KERNEL_DIR = kernel
INCLUDE_DIR = include

# Tên tệp ảnh đĩa
IMAGE_FILE = $(BUILD_DIR)/os-image.img

# Tạo thư mục build nếu chưa tồn tại
$(shell mkdir -p $(BUILD_DIR))

all: $(IMAGE_FILE)

# --- QUY TẮC TẠO ẢNH ĐĨA ĐÃ SỬA LẠI ---
# Tạo ảnh đĩa mềm 1.44MB và ghi các thành phần vào đúng vị trí
$(IMAGE_FILE): $(BUILD_DIR)/boot_sect.bin $(BUILD_DIR)/main_bootloader.bin $(BUILD_DIR)/kernel_loader.bin
	@echo "Creating disk image: $@"
	# Tạo một ảnh đĩa trống 1.44MB (2880 sectors)
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null

	# Ghi boot sector vào Sector 1 (seek=0)
	dd if=$(BUILD_DIR)/boot_sect.bin of=$@ seek=0 conv=notrunc

	# Ghi main bootloader vào Sector 2 (seek=1)
	dd if=$(BUILD_DIR)/main_bootloader.bin of=$@ seek=1 conv=notrunc

	# Ghi kernel loader vào Sector 10 (seek=9)
	dd if=$(BUILD_DIR)/kernel_loader.bin of=$@ seek=9 conv=notrunc

	@echo "Disk image created successfully!"

# Boot sector
$(BUILD_DIR)/boot_sect.bin: $(BOOT_DIR)/boot_sector.asm
	$(ASM) -f bin -o $@ $<

# main_bootloader
$(BUILD_DIR)/main_bootloader.bin: $(BOOT_DIR)/main_bootloader.asm
	$(ASM) -f bin -o $@ $<

# kernel_core
$(BUILD_DIR)/kernel_core.bin: $(BOOT_DIR)/kernel_core.asm
	$(ASM) -f bin -o $@ $<

# Kernel loader object
$(BUILD_DIR)/kernel_loader.o: $(KERNEL_DIR)/kernel_loader.c $(INCLUDE_DIR)/boot.h
	$(CC) $(CFLAGS) -o $@ $<

# Kernel loader binary
$(BUILD_DIR)/kernel_loader.bin: $(BUILD_DIR)/kernel_loader.o
	# Giả sử bạn có file boot.ld để định vị kernel tại 0x10000
	# Nếu không, bạn cần thêm các tùy chọn linker phù hợp.
	$(LD) $(LDFLAGS) -o $(BUILD_DIR)/kernel_loader.elf $^
	objcopy -O binary $(BUILD_DIR)/kernel_loader.elf $@

# Run với QEMU
run: $(IMAGE_FILE)
	@echo "Starting QEMU..."
	qemu-system-x86_64 -fda $(IMAGE_FILE)

# Debug
debug: $(IMAGE_FILE)
	@echo "Starting QEMU in debug mode..."
	qemu-system-x86_64 -fda $(IMAGE_FILE) -S -gdb tcp::1234

clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned $(BUILD_DIR) directory"

.PHONY: all run debug clean