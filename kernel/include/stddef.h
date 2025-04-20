#ifndef _VIBEOS_STDDEF_H
#define _VIBEOS_STDDEF_H

/* Null pointer constant */
#define NULL ((void*)0)

/* Offset of a member in a struct */
#define offsetof(type, member) __builtin_offsetof(type, member)

/* Unsigned integer type that can hold the result of sizeof */
typedef unsigned int size_t;

/* Signed integer type that can hold the result of pointer subtraction */
typedef int ptrdiff_t;

#endif /* _VIBEOS_STDDEF_H */ 