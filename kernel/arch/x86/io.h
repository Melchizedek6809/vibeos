#ifndef IO_H
#define IO_H

#include "../../include/stdint.h"

/* Read a byte from a port */
uint8_t inb(uint16_t port);

/* Write a byte to a port */
void outb(uint16_t port, uint8_t value);

#endif /* IO_H */ 