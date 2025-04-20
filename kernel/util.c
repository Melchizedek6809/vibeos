#include "include/stddef.h"
#include "util.h"

/* Simple delay function - spins CPU */
void delay(int count) {
    for (int i = 0; i < count * 10000; i++)
        asm volatile ("nop");
} 