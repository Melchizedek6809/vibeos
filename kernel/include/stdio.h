#ifndef _VIBEOS_STDIO_H
#define _VIBEOS_STDIO_H

#include "stddef.h"
#include "stdint.h"

/* Write a string to both terminal and serial outputs */
void puts(const char* str);

/* Format a string and store it in the buffer */
int snprintf(char* buffer, size_t size, const char* format, ...);

#endif /* _VIBEOS_STDIO_H */ 