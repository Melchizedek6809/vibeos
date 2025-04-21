#ifndef _VIBEOS_LIMITS_H
#define _VIBEOS_LIMITS_H

#include <stdint.h>

/* Integer limits */
#define CHAR_BIT    8
#define SCHAR_MIN   (-128)
#define SCHAR_MAX   127
#define UCHAR_MAX   255
#define CHAR_MIN    SCHAR_MIN
#define CHAR_MAX    SCHAR_MAX

#define SHRT_MIN    (-32768)
#define SHRT_MAX    32767
#define USHRT_MAX   65535

#define INT_MIN     (-2147483648)
#define INT_MAX     2147483647
#define UINT_MAX    4294967295U

#define LONG_MIN    (-2147483648L)
#define LONG_MAX    2147483647L
#define ULONG_MAX   4294967295UL

/* Size limits */
#define SIZE_MAX    UINT_MAX
#define SSIZE_MAX   INT_MAX

/* Pointer limits */
#define PTRDIFF_MIN INT_MIN
#define PTRDIFF_MAX INT_MAX

#endif /* _VIBEOS_LIMITS_H */ 