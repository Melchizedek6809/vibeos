#ifndef KERNEL_H
#define KERNEL_H

#include "include/stdint.h"
#include "include/stddef.h"

/* Multiboot header magic values */
#define MULTIBOOT_MAGIC 0x1BADB002

/* Kernel entry point - called from boot.S */
void kernel_main(uint32_t multiboot_magic, uint32_t multiboot_addr);

#endif /* KERNEL_H */ 