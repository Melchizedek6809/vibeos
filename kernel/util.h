#ifndef UTIL_H
#define UTIL_H

/* Simple delay function - spins CPU */
void delay(int count);

/* Write a string to both terminal and serial */
void puts(const char* str);

#endif /* UTIL_H */ 