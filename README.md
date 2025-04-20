# VIbeOS

A very simple operating system kernel. This is a minimal example that demonstrates:

- A multiboot-compliant kernel
- Setting up a basic VGA text mode output
- Setting up serial output
- Implementing a "Hello World" display

## Prerequisites

You'll need the following tools installed:

- GCC compiler (32-bit multilib support)
- QEMU emulator

On Arch Linux, you can install these with:

```bash
sudo pacman -S gcc qemu-system-x86
```

## Directory Structure

- `kernel/` - Contains kernel source code
- `build/` - Build output directory (created during build)

## Building

To build the kernel:

```bash
make all
```

## Running

To run the OS in QEMU:

```bash
make run
```

The OS will boot and output "Hello, World!" both to the screen and to the serial console.

## Cleaning

To clean all build artifacts:

```bash
make clean
```

## How It Works

1. The bootloader loads our kernel (in QEMU we pass it directly)
2. Our multiboot-compliant header tells the bootloader how to handle our kernel
3. The kernel initializes the VGA text mode and serial port
4. The kernel prints "Hello, World!" to both outputs
5. The kernel then halts the CPU in an infinite loop

## Next Steps

Possible enhancements to this minimal OS:

- Add memory management
- Implement a simple file system
- Add process management
- Implement a shell
- Add device drivers 