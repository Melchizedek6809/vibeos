#include "../include/string.h"
#include "../include/stdint.h"

/* String length functions */

/* Calculate string length */
size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* Calculate string length with maximum limit */
size_t strnlen(const char* str, size_t maxlen) {
    size_t len = 0;
    while (len < maxlen && str[len] != '\0') {
        len++;
    }
    return len;
}

/* String copy functions */

/* Copy string */
char* strcpy(char* dest, const char* src) {
    char* original_dest = dest;
    while ((*dest++ = *src++) != '\0');
    return original_dest;
}

/* Copy string with length limit */
char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    
    /* Copy up to n characters from src to dest */
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    
    /* Pad dest with null bytes if necessary */
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    
    return dest;
}

/* String comparison functions */

/* Compare two strings */
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/* Compare two strings with length limit */
int strncmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) {
        return 0;
    }
    
    while (n-- > 1 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/* Memory functions */

/* Copy memory area */
void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    /* Simple byte-by-byte copy */
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    
    return dest;
}

/* Copy memory area, handling overlapping regions */
void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    /* If dest is before src or after the end of src, we can copy forward */
    if (d <= s || d >= s + n) {
        return memcpy(dest, src, n);
    }
    
    /* Otherwise, copy backward to handle overlapping regions */
    for (size_t i = n; i > 0; i--) {
        d[i-1] = s[i-1];
    }
    
    return dest;
}

/* Fill memory with a constant byte */
void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    
    return s;
}

/* Compare memory areas */
int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    
    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] - p2[i];
        }
    }
    
    return 0;
}

/* Internal function to reverse a string in place */
void reverse_str(char* start, char* end) {
    char temp;
    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

/* Convert integer to string and return length */
int itoa(int32_t value, char* str, int base) {
    int i = 0;
    int is_negative = 0;
    
    /* Handle 0 explicitly */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    /* Handle negative numbers for base 10 */
    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }
    
    /* Process individual digits */
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        value /= base;
    }
    
    /* Add negative sign if needed */
    if (is_negative)
        str[i++] = '-';
    
    /* Null terminate the string */
    str[i] = '\0';
    
    /* Reverse the string */
    reverse_str(str, &str[i - 1]);
    
    return i;
}

/* Convert unsigned integer to string and return length */
int utoa(uint32_t value, char* str, int base) {
    int i = 0;
    
    /* Handle 0 explicitly */
    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return i;
    }
    
    /* Process individual digits */
    while (value != 0) {
        int remainder = value % base;
        str[i++] = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
        value /= base;
    }
    
    /* Null terminate the string */
    str[i] = '\0';
    
    /* Reverse the string */
    reverse_str(str, &str[i - 1]);
    
    return i;
}

/* Convert a string to an integer */
int atoi(const char* str) {
    int result = 0;
    int sign = 1;
    
    /* Skip leading whitespace */
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') {
        str++;
    }
    
    /* Handle sign */
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    /* Convert digits */
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

/* Find the first occurrence of a character in a string */
char* strchr(const char* str, int c) {
    while (*str != '\0') {
        if (*str == (char)c) {
            return (char*)str;
        }
        str++;
    }
    
    /* Return NULL if we didn't find the character */
    return NULL;
} 