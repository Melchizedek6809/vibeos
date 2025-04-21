# VibeOS - LLM-Assisted OS Development

## Project Description
VibeOS is an educational project aimed at building a small Unix-like operating system kernel with the assistance of LLM tools. The goal is to explore OS development concepts in a structured, modular way while leveraging AI assistance for implementation and problem-solving.

This project serves as both a learning platform for OS fundamentals and an experiment in LLM-assisted software development for complex low-level systems.

## Project Structure

```
vibeos/
├── kernel/              # Core kernel code
│   ├── arch/            # Architecture-specific code
│   │   └── x86/         # x86 architecture implementation
│   ├── drivers/         # Hardware drivers
│   ├── include/         # Header files and standard library headers
│   ├── mm/              # Memory management
│   └── stdlib/          # Custom standard library implementations
├── build/               # Build output directory
└── Makefile             # Build configuration
```

## General Guidelines

### Development Approach
- **Incremental Progress**: Build the system piece by piece, starting with the basics
- **Learning-Focused**: Prioritize clear, well-commented code over optimization
- **LLM Collaboration**: Use AI tools to help design, implement, and debug components
- **Hands-On Learning**: Implement core OS concepts from scratch rather than using libraries

### Architecture Guidelines
- **32-bit x86**: Target the i386 architecture for simplicity and educational value
- **Modular Design**: Separate functionality into cohesive, well-defined modules
- **Unix Inspiration**: Follow Unix-like design principles where appropriate
- **Self-Contained**: Implement custom versions of standard libraries rather than using existing ones

### Code Style
- **Clear Organization**: Maintain a consistent, logical file structure
- **Descriptive Naming**: Use explicit names for functions, variables, and constants
- **Comprehensive Comments**: Document the purpose of functions and complex logic
- **Consistent Styling**: Follow established formatting patterns throughout the codebase

### Implementation Priorities
1. **Core Kernel**: Basic initialization, I/O, and terminal functionality
2. **Memory Management**: Paging, allocation, and virtual memory
3. **Process Management**: Task scheduling and execution
4. **File Systems**: Basic file operations and storage
5. **User Space**: Applications and system calls
6. **Networking**: Basic communication capabilities

## Building and Running

### Requirements
- GCC with i386 multilib support
- NASM or GAS assembler
- GNU Make
- QEMU for emulation

### Building
```bash
make
```

### Running
```bash
make run
```

### Debugging
```bash
make debug
```

## Features

- **Modular Design**: Code is organized into logical modules with clear responsibilities
- **Multiboot Support**: Compatible with GRUB and other multiboot-compliant bootloaders
- **Dual Output**: All kernel messages are displayed on both VGA console and serial port
- **Custom Standard Library**: Independent implementation of common C headers
- **Formatted Output**: Support for formatted string output with snprintf
- **String Utilities**: Complete suite of string and memory manipulation functions

## License

This project is released as open source software.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 