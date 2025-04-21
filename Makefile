CC = gcc
AS = gcc
LD = ld
QEMU = qemu-system-i386

# Compiler and linker flags
CFLAGS = -Wall -Wextra -O0 -g -ffreestanding -m32 \
         -fno-stack-protector -fno-pie -no-pie \
         -nostdinc -Ikernel -Ikernel/include
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
KERNEL_DIR = kernel
INCLUDE_DIR = kernel/include
DRIVERS_DIR = kernel/drivers
ARCH_DIR = kernel/arch/x86
STDLIB_DIR = kernel/stdlib
MM_DIR = kernel/mm
BUILD_DIR = build

# Source files by category
KERNEL_SOURCES = $(KERNEL_DIR)/kernel.c \
                 $(KERNEL_DIR)/util.c

ARCH_SOURCES = $(ARCH_DIR)/io.c \
               $(ARCH_DIR)/idt.c \
               $(ARCH_DIR)/isr.c \
               $(ARCH_DIR)/pic.c

DRIVER_SOURCES = $(DRIVERS_DIR)/vga.c \
                 $(DRIVERS_DIR)/serial.c \
                 $(DRIVERS_DIR)/keyboard.c

STDLIB_SOURCES = $(STDLIB_DIR)/stdio.c \
                 $(STDLIB_DIR)/string.c

# Assembly sources
ASM_SOURCES = $(ARCH_DIR)/boot.S \
              $(ARCH_DIR)/interrupts.S

# Combine all sources
C_SOURCES = $(KERNEL_SOURCES) $(ARCH_SOURCES) $(DRIVER_SOURCES) $(STDLIB_SOURCES)

# Object files
OBJECTS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o) \
          $(ASM_SOURCES:%.S=$(BUILD_DIR)/%.o)

# Target files
KERNEL = $(BUILD_DIR)/kernel.bin
KERNEL_RAW = $(BUILD_DIR)/kernel.raw

.PHONY: all clean run debug iso

all: prepare $(KERNEL)

# Prepare build environment
prepare:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(KERNEL_DIR)
	@mkdir -p $(BUILD_DIR)/$(STDLIB_DIR)
	@mkdir -p $(BUILD_DIR)/$(DRIVERS_DIR)
	@mkdir -p $(BUILD_DIR)/$(ARCH_DIR)

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assemble assembly files
$(BUILD_DIR)/%.o: %.S
	@echo "Assembling $<"
	@mkdir -p $(dir $@)
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