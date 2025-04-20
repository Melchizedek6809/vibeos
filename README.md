# VibeOS

A minimalist operating system kernel written in C.

## Overview

VibeOS is a simple educational operating system kernel that demonstrates basic OS concepts:
- Bootloader integration
- Kernel initialization
- Terminal output (VGA text mode)
- Serial port communication
- Basic I/O operations
- Custom standard library implementation

## Project Structure

```
kernel/
├── boot.S             # Multiboot-compliant assembly entry point
├── kernel.c/h         # Main kernel code and entry point
├── io.c/h             # I/O port operations
├── serial.c/h         # Serial port communication
├── vga.c/h            # VGA text mode display
├── util.c/h           # Utility functions
├── stdio.c            # Standard I/O functionality
├── linker.ld          # Linker script for the kernel
└── include/           # Custom standard headers
    ├── stdint.h       # Integer type definitions
    ├── stddef.h       # Common type definitions
    └── stdarg.h       # Variable argument support
```

## Features

- **Modular Design**: Code is organized into logical modules with clear responsibilities
- **Multiboot Support**: Compatible with GRUB and other multiboot-compliant bootloaders
- **Dual Output**: All kernel messages are displayed on both VGA console and serial port
- **Custom Standard Library**: Independent implementation of common C headers
- **Formatted Output**: Support for formatted string output with snprintf

## Building

The project uses a simple Makefile system:

```bash
# Clean and build the kernel
make clean && make all

# Run in QEMU
make run

# Run with GDB debugging support
make debug

# Build ISO image (requires GRUB tools)
make iso
```

## Requirements

- GCC cross-compiler for i386 targets
- GNU Make
- QEMU for testing
- (Optional) GRUB tools for ISO creation

## Implementation Details

The kernel currently provides:

- Basic terminal driver for text output using VGA mode
- Serial port driver for debugging output
- Helper functions for I/O port access
- Minimal standard library implementation
- Simple formatted string output through snprintf

## Future Improvements

- Memory management
- Interrupt handling
- Process scheduling
- File system support
- User mode execution
- Networking capabilities

## License

This project is released as open source software.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 