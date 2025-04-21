#include <string.h>
#include <stdint.h>
#include <limits.h>

/* String length functions */

/* Calculate string length */
size_t strlen(const char* str) {
    const char* s = str;
    while (*s) s++;
    return s - str;
}

/* Calculate string length with maximum limit */
size_t strnlen(const char* str, size_t maxlen) {
    const char* s = str;
    while (maxlen > 0 && *s) {
        s++;
        maxlen--;
    }
    return s - str;
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
    char* original_dest = dest;
    size_t i;

    /* Copy from src */
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }

    /* Fill remainder with nulls */
    for (; i < n; i++) {
        dest[i] = '\0';
    }

    return original_dest;
}

/* String comparison functions */

/* Compare two strings */
int strcmp(const char* s1, const char* s2) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    
    while (*p1 && *p1 == *p2) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}

/* Compare two strings with length limit */
int strncmp(const char* s1, const char* s2, size_t n) {
    if (n == 0) return 0;

    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;

    while (n > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        if (*p1 == '\0') {
            return 0;
        }
        p1++;
        p2++;
        n--;
    }
    return 0;
}

/* Memory functions */

/* Copy memory area */
void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    
    /* Copy in chunks of size_t if possible */
    size_t* dw;
    const size_t* sw;
    
    /* Align to size_t boundary */
    while (n && ((size_t)d & (sizeof(size_t)-1))) {
        *d++ = *s++;
        n--;
    }
    
    /* Copy size_t chunks */
    if (n >= sizeof(size_t)) {
        dw = (size_t*)d;
        sw = (const size_t*)s;
        while (n >= sizeof(size_t)) {
            *dw++ = *sw++;
            n -= sizeof(size_t);
        }
        d = (unsigned char*)dw;
        s = (const unsigned char*)sw;
    }
    
    /* Copy remaining bytes */
    while (n--) {
        *d++ = *s++;
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
    d += n;
    s += n;
    
    /* Copy in chunks of size_t if possible */
    size_t* dw;
    const size_t* sw;
    
    /* Copy remaining bytes */
    while (n && ((size_t)d & (sizeof(size_t)-1))) {
        *--d = *--s;
        n--;
    }
    
    /* Copy size_t chunks */
    if (n >= sizeof(size_t)) {
        dw = (size_t*)d;
        sw = (const size_t*)s;
        while (n >= sizeof(size_t)) {
            *--dw = *--sw;
            n -= sizeof(size_t);
        }
        d = (unsigned char*)dw;
        s = (const unsigned char*)sw;
    }
    
    /* Copy remaining bytes */
    while (n--) {
        *--d = *--s;
    }
    
    return dest;
}

/* Fill memory with a constant byte */
void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    unsigned char byte = (unsigned char)c;
    
    /* Fill in chunks of size_t if possible */
    size_t* pw;
    size_t word;
    
    /* Create word-sized pattern */
    word = byte;
    word |= word << 8;
    word |= word << 16;
#if SIZE_MAX > 0xFFFFFFFF  /* Check if size_t is 64-bit */
    word |= ((size_t)word) << 32;
#endif
    
    /* Align to size_t boundary */
    while (n && ((size_t)p & (sizeof(size_t)-1))) {
        *p++ = byte;
        n--;
    }
    
    /* Fill size_t chunks */
    if (n >= sizeof(size_t)) {
        pw = (size_t*)p;
        while (n >= sizeof(size_t)) {
            *pw++ = word;
            n -= sizeof(size_t);
        }
        p = (unsigned char*)pw;
    }
    
    /* Fill remaining bytes */
    while (n--) {
        *p++ = byte;
    }
    
    return s;
}

/* Compare memory areas */
int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    
    /* Compare in chunks of size_t if possible */
    const size_t* w1;
    const size_t* w2;
    
    /* Align to size_t boundary */
    while (n && ((size_t)p1 & (sizeof(size_t)-1))) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
        n--;
    }
    
    /* Compare size_t chunks */
    if (n >= sizeof(size_t)) {
        w1 = (const size_t*)p1;
        w2 = (const size_t*)p2;
        while (n >= sizeof(size_t)) {
            if (*w1 != *w2) {
                p1 = (const unsigned char*)w1;
                p2 = (const unsigned char*)w2;
                while (*p1 == *p2) {
                    p1++;
                    p2++;
                }
                return *p1 - *p2;
            }
            w1++;
            w2++;
            n -= sizeof(size_t);
        }
        p1 = (const unsigned char*)w1;
        p2 = (const unsigned char*)w2;
    }
    
    /* Compare remaining bytes */
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    
    return 0;
}

/* Internal function to reverse a string in place */
void reverse_str(char* start, char* end) {
    while (start < end) {
        char temp = *start;
        *start++ = *end;
        *end-- = temp;
    }
}

/* Convert integer to string and return length */
size_t itoa(int32_t value, char* str, int base) {
    size_t i = 0;
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
size_t utoa(uint32_t value, char* str, int base) {
    size_t i = 0;
    
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
        /* Check for overflow */
        if (result > INT32_MAX / 10 || 
            (result == INT32_MAX / 10 && (*str - '0') > INT32_MAX % 10)) {
            return sign > 0 ? INT32_MAX : INT32_MIN;
        }
        result = result * 10 + (*str - '0');
        str++;
    }
    
    return sign * result;
}

/* Find the first occurrence of a character in a string */
char* strchr(const char* str, int c) {
    const unsigned char ch = (unsigned char)c;
    const unsigned char* s = (const unsigned char*)str;
    
    while (*s != ch) {
        if (!*s) return NULL;
        s++;
    }
    return (char*)s;
}

/* Find the last occurrence of a character in a string */
char* strrchr(const char* str, int c) {
    const unsigned char ch = (unsigned char)c;
    const unsigned char* s = (const unsigned char*)str;
    const unsigned char* last = NULL;
    
    while (*s) {
        if (*s == ch) last = s;
        s++;
    }
    if (*s == ch) last = s;  /* Check null terminator */
    return (char*)last;
}

/* Static variable for strtok's state */
static char* strtok_ptr = NULL;

/* String tokenization */
char* strtok(char* str, const char* delimiters) {
    char* token_start;
    char* token_end;

    /* If str is NULL, continue with previous string */
    if (!str) {
        str = strtok_ptr;
    }
    if (!str) {
        return NULL;
    }

    /* Skip leading delimiters */
    str += strspn(str, delimiters);
    if (*str == '\0') {
        strtok_ptr = NULL;
        return NULL;
    }

    /* Find end of token */
    token_start = str;
    token_end = str + strcspn(str, delimiters);
    if (*token_end == '\0') {
        strtok_ptr = NULL;
    } else {
        *token_end = '\0';
        strtok_ptr = token_end + 1;
    }

    return token_start;
}

/* Helper function: get length of initial segment matching chars in accept */
size_t strspn(const char* str, const char* accept) {
    const char* s = str;
    const char* a;

    while (*s) {
        for (a = accept; *a; a++) {
            if (*s == *a) {
                break;
            }
        }
        if (!*a) {
            break;
        }
        s++;
    }
    return s - str;
}

/* Helper function: get length of initial segment not matching chars in reject */
size_t strcspn(const char* str, const char* reject) {
    const char* s = str;
    const char* r;

    while (*s) {
        for (r = reject; *r; r++) {
            if (*s == *r) {
                return s - str;
            }
        }
        s++;
    }
    return s - str;
} 