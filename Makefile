CC = gcc
AS = gcc
LD = ld
QEMU = qemu-system-i386

# Compiler and linker flags
CFLAGS = -Wall -Wextra -O0 -g -ffreestanding -m32 \
         -fno-stack-protector -fno-pie -no-pie \
         -nostdinc -Ikernel/include
ASFLAGS = -m32 -nostdlib
LDFLAGS = -m elf_i386 -T kernel/linker.ld -nostdlib

# QEMU configuration with multiboot support
QEMUFLAGS = -kernel build/kernel.bin \
            -serial stdio \
            -display sdl \
            -no-reboot \
            -d int,cpu_reset \
            -D qemu.log \
            -machine type=pc-i440fx-3.1 \
            -m 128M

# Directories
SRC_DIR = kernel
STDLIB_DIR = kernel/stdlib
BUILD_DIR = build

# Source files
C_SOURCES = $(SRC_DIR)/idt.c \
            $(SRC_DIR)/io.c \
            $(SRC_DIR)/isr.c \
            $(SRC_DIR)/kernel.c \
            $(SRC_DIR)/serial.c \
            $(SRC_DIR)/util.c \
            $(SRC_DIR)/vga.c \
            $(SRC_DIR)/keyboard.c \
            $(SRC_DIR)/pic.c

STDLIB_SOURCES = $(STDLIB_DIR)/stdio.c \
                 $(STDLIB_DIR)/string.c

ASM_SOURCES = $(SRC_DIR)/boot.S \
              $(SRC_DIR)/interrupts.S

# Object files
OBJECTS = $(C_SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) \
          $(STDLIB_SOURCES:$(STDLIB_DIR)/%.c=$(BUILD_DIR)/stdlib_%.o) \
          $(ASM_SOURCES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

# Target files
KERNEL = $(BUILD_DIR)/kernel.bin
KERNEL_RAW = $(BUILD_DIR)/kernel.raw

.PHONY: all clean run debug iso

all: prepare $(KERNEL)

# Prepare build environment
prepare:
	@mkdir -p $(BUILD_DIR)

# Compile C files from main kernel directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C files from stdlib directory
$(BUILD_DIR)/stdlib_%.o: $(STDLIB_DIR)/%.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	@echo "Assembling $<"
	$(AS) $(ASFLAGS) -c $< -o $@

# Link the kernel
$(KERNEL): $(OBJECTS)
	@echo "Linking kernel"
	$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Creating a binary copy"
	objcopy -O binary $(KERNEL) $(KERNEL_RAW)

# Run the OS with QEMU
run: all
	@echo "Starting QEMU..."
	$(QEMU) $(QEMUFLAGS)

# Debug with GDB
debug: all
	@echo "Starting QEMU with GDB..."
	$(QEMU) $(QEMUFLAGS) -S -s

# Build an ISO image with GRUB (alternative)
iso: all
	@echo "Building ISO image with GRUB..."
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@cp $(KERNEL) $(BUILD_DIR)/iso/boot/
	@echo "set timeout=0" > $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "set default=0" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "menuentry \"VibeOS\" {" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "  multiboot /boot/kernel.bin" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "  boot" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "}" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@grub-mkrescue -o $(BUILD_DIR)/vibeos.iso $(BUILD_DIR)/iso 2>/dev/null
	@echo "ISO created at $(BUILD_DIR)/vibeos.iso"

# Run using ISO image
run-iso: iso
	@echo "Starting QEMU with ISO..."
	$(QEMU) -cdrom $(BUILD_DIR)/vibeos.iso -serial stdio -no-reboot

# Clean build files
clean:
	@echo "Cleaning build files"
	@rm -rf $(BUILD_DIR) qemu.log 