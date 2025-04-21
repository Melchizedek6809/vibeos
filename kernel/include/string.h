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

/* Integer to string conversion
 * Returns the number of characters written to str (excluding null terminator)
 * str must be large enough to hold the output string
 * For itoa: minimum buffer size = 12 for 32-bit int in base 10 (sign + 10 digits + null)
 * For utoa: minimum buffer size = 11 for 32-bit uint in base 10 (10 digits + null)
 */
size_t itoa(int32_t value, char* str, int base);
size_t utoa(uint32_t value, char* str, int base);

/* String manipulation helper */
void reverse_str(char* start, char* end);

/* String scanning functions
 * strspn: Returns length of initial segment containing only chars from accept
 * strcspn: Returns length of initial segment containing no chars from reject
 */
size_t strspn(const char* str, const char* accept);
size_t strcspn(const char* str, const char* reject);

/* String tokenization
 * Breaks string into sequence of tokens using the delimiters
 * First call should have str != NULL, subsequent calls should have str == NULL
 * Returns NULL when no more tokens are found
 * Note: modifies the original string by replacing delimiters with null bytes
 */
char* strtok(char* str, const char* delimiters);

/* String search functions
 * Returns pointer to the found character or NULL if not found
 * For strchr: first occurrence from start
 * For strrchr: last occurrence from end
 */
char* strchr(const char* str, int c);
char* strrchr(const char* str, int c);

/* String to number conversion
 * Skips leading whitespace
 * Handles optional +/- sign
 * Stops at first non-digit character
 */
int atoi(const char* str);

#endif /* _VIBEOS_STRING_H */ 