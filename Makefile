NASM = nasm
GCC = gcc
LD = ld
QEMU = qemu-system-x86_64
RM = rm -f
MKDIR = mkdir -p

BUILD_DIR = build

CFLAGS = -m64 -ffreestanding -nostdlib -fno-stack-protector -fno-pie
CFLAGS += -I. -I./drivers -I./drivers/VGA -I./kernel

all: build run

$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

build: $(BUILD_DIR) $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/os-image.bin
	@echo "Build complete!"

$(BUILD_DIR)/bootloader.bin: boot/bootloader.asm
	$(MKDIR) $(BUILD_DIR)
	@echo "Building bootloader..."
	$(NASM) -f bin $< -o $@
	@echo "Bootloader built: $@"

$(BUILD_DIR)/kernel_asm.o: kernel/main/kernel.asm
	$(MKDIR) $(BUILD_DIR)
	@echo "Building kernel assembly..."
	$(NASM) -f elf64 $< -o $@
	@echo "Kernel assembly built: $@"

$(BUILD_DIR)/vga.o: drivers/VGA/vga.c
	$(MKDIR) $(BUILD_DIR)
	@echo "Building VGA driver..."
	$(GCC) $(CFLAGS) -c $< -o $@
	@echo "VGA driver built: $@"

$(BUILD_DIR)/kernel.o: kernel/main/kernel.c
	$(MKDIR) $(BUILD_DIR)
	@echo "Building kernel C..."
	$(GCC) $(CFLAGS) -c $< -o $@
	@echo "Kernel C built: $@"

# Link ELF file (добавь vga.o)
$(BUILD_DIR)/kernel.elf: $(BUILD_DIR)/kernel_asm.o $(BUILD_DIR)/kernel.o $(BUILD_DIR)/vga.o
	@echo "Linking kernel..."
	$(LD) -T kernel/linker.ld -nostdlib -no-pie $^ -o $@
	@echo "Kernel linked: $@"

# Extract raw binary from ELF for bootloading
$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	@echo "Extracting raw kernel binary..."
	objcopy -O binary $< $@
	@echo "Kernel binary extracted: $@"

# Create floppy disk image
$(BUILD_DIR)/os-image.bin: $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.bin
	@echo "Creating OS image..."
	dd if=/dev/zero of=$@ bs=512 count=2880 2>/dev/null
	dd if=$(BUILD_DIR)/bootloader.bin of=$@ conv=notrunc 2>/dev/null
	dd if=$(BUILD_DIR)/kernel.bin of=$@ seek=1 conv=notrunc 2>/dev/null
	@echo "OS image created: $@"

run: build
	@echo "Starting QEMU..."
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin -m 256M

run-debug: build
	@echo "Starting QEMU with debug..."
	$(QEMU) -drive format=raw,file=$(BUILD_DIR)/os-image.bin -m 256M -s -S

clean:
	@echo "Cleaning up..."
	$(RM) -r $(BUILD_DIR)
	@echo "Clean complete!"

check:
	@echo "Checking bootloader..."
	$(NASM) -f bin boot/bootloader.asm -o /dev/null
	@echo "Bootloader syntax OK"
	@echo "Checking kernel assembly..."
	$(NASM) -f elf64 kernel/main/kernel.asm -o /dev/null
	@echo "Kernel assembly syntax OK"

size: build
	@echo "File sizes:"
	@ls -lh $(BUILD_DIR)/bootloader.bin $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/os-image.bin

help:
	@echo "=========================================="
	@echo "DV OS Makefile commands:"
	@echo "=========================================="
	@echo "  make build     - Build everything"
	@echo "  make run       - Build and run in QEMU"
	@echo "  make run-debug - Build and run with debug (GDB)"
	@echo "  make clean     - Delete all built files"
	@echo "  make check     - Check syntax only"
	@echo "  make size      - Show file sizes"
	@echo "  make help      - Show this help"
	@echo "=========================================="

.PHONY: all build run run-debug clean check size help