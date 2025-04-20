#ifndef _VIBEOS_STRING_H
#define _VIBEOS_STRING_H

#include "stddef.h"
#include "stdint.h"

/* String length functions */
size_t strlen(const char* str);
size_t strnlen(const char* str, size_t maxlen);

/* String copy functions */
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);

/* String comparison functions */
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);

/* Memory functions */
void* memcpy(void* dest, const void* src, size_t n);
void* memmove(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);

/* Integer to string conversion */
int itoa(int32_t value, char* str, int base);
int utoa(uint32_t value, char* str, int base);

/* String manipulation helper */
void reverse_str(char* start, char* end);

/* String tokenization */
char* strtok(char* str, const char* delimiters);

/* String search functions */
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);

/* String to number conversion */
int atoi(const char* str);

#endif /* _VIBEOS_STRING_H */ 