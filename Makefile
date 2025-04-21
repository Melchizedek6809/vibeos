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
BUILD_DIR = build

# Dynamically find source files
C_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c")
ASM_SOURCES = $(shell find $(KERNEL_DIR) -name "*.c")

# Object files
C_OBJECTS = $(C_SOURCES:%.c=$(BUILD_DIR)/%.o)
ASM_OBJECTS = $(ASM_SOURCES:%.S=$(BUILD_DIR)/%.o)
OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS = $(C_OBJECTS:.o=.d)

# Target files
KERNEL = $(BUILD_DIR)/kernel.bin
KERNEL_RAW = $(BUILD_DIR)/kernel.raw

# Version info
VERSION = 0.1.0
BUILD_DATE = $(shell date +"%Y-%m-%d")

.PHONY: all clean run debug iso help version info size sections

all: prepare $(KERNEL)
	@echo "Build complete!"

# Include dependency files
-include $(DEPS)

# Prepare build environment
prepare:
	@echo "Preparing build directories..."
	@mkdir -p $(BUILD_DIR)
	@find $(KERNEL_DIR) -type d | sed "s|$(KERNEL_DIR)|$(BUILD_DIR)/$(KERNEL_DIR)|" | xargs -I{} mkdir -p {}

# Compile C files with dependency generation
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Assemble assembly files
$(BUILD_DIR)/%.o: %.S
	@echo "Assembling $<"
	@mkdir -p $(dir $@)
	@$(AS) $(ASFLAGS) -c $< -o $@

# Link the kernel
$(KERNEL): $(OBJECTS)
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Creating binary copy..."
	@objcopy -O binary $(KERNEL) $(KERNEL_RAW)
	@echo "Kernel size: $$(stat -c %s $(KERNEL)) bytes"

# Run the OS with QEMU
run: all
	@echo "Starting QEMU..."
	@$(QEMU) $(QEMUFLAGS)

# Debug with GDB
debug: all
	@echo "Starting QEMU with GDB..."
	@echo "Connect with GDB using: target remote localhost:1234"
	@$(QEMU) $(QEMUFLAGS) -S -s

# Build an ISO image with GRUB (alternative)
iso: all
	@echo "Building ISO image with GRUB..."
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	@cp $(KERNEL) $(BUILD_DIR)/iso/boot/
	@echo "set timeout=0" > $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "set default=0" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "menuentry \"VibeOS $(VERSION)\" {" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "  multiboot /boot/kernel.bin" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "  boot" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@echo "}" >> $(BUILD_DIR)/iso/boot/grub/grub.cfg
	@grub-mkrescue -o $(BUILD_DIR)/vibeos.iso $(BUILD_DIR)/iso 2>/dev/null
	@echo "ISO created at $(BUILD_DIR)/vibeos.iso"

# Run using ISO image
run-iso: iso
	@echo "Starting QEMU with ISO..."
	@$(QEMU) -cdrom $(BUILD_DIR)/vibeos.iso -serial stdio -no-reboot

# Print version information
version:
	@echo "VibeOS $(VERSION) ($(BUILD_DATE))"

# Print build information
info:
	@echo "VibeOS Build Information:"
	@echo "C Source Files: $(words $(C_SOURCES))"
	@echo "ASM Source Files: $(words $(ASM_SOURCES))"
	@echo "Total Objects: $(words $(OBJECTS))"
	@echo "Compiler: $(shell $(CC) --version | head -n1)"
	@echo "Build Date: $(BUILD_DATE)"

# Show kernel size information
size: $(KERNEL)
	@echo "Kernel Size Information:"
	@size $(KERNEL)

# Show section information
sections: $(KERNEL)
	@echo "Kernel Section Information:"
	@objdump -h $(KERNEL)

# Print helpful information
help:
	@echo "VibeOS Makefile Help:"
	@echo "make       - Build the kernel"
	@echo "make run   - Build and run in QEMU"
	@echo "make debug - Build and run with GDB debugging"
	@echo "make iso   - Build bootable ISO image"
	@echo "make run-iso - Build ISO and run in QEMU"
	@echo "make clean - Remove build artifacts"
	@echo "make info  - Show build information"
	@echo "make size  - Show kernel size information"
	@echo "make version - Show version information"
	@echo "make help  - Show this help message"

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR) qemu.log 