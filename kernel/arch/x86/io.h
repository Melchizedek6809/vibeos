#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <stdint.h>

// Write a byte to an I/O port
static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

// Read a byte from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Write a word (16 bits) to an I/O port
static inline void outw(uint16_t port, uint16_t value) {
    asm volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

// Read a word (16 bits) from an I/O port
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Write a double word (32 bits) to an I/O port
static inline void outl(uint16_t port, uint32_t value) {
    asm volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

// Read a double word (32 bits) from an I/O port
static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

#endif // KERNEL_IO_H 