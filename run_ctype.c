#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include <string.h>

/*
// Build-time assertion building block. 
#define BUILD_ASSERT__(EXPR) \
sizeof(struct { unsigned int build_assert_failed : (EXPR) ? 1 : -1; })

// Build-time assertion for use in a declaration context.
#define BUILD_ASSERT_DECL(EXPR) \
extern int (*build_assert(void))[BUILD_ASSERT__(EXPR)]
*/

#define TYPE_IS_INT(TYPE) ((TYPE) 1.5 == (TYPE) 1)
#define TYPE_IS_SIGN(TYPE) ((TYPE) 0 > (TYPE) -1)
#define TYPE_VALUE_BITS(TYPE) (sizeof(TYPE) * CHAR_BIT - TYPE_IS_SIGN(TYPE))

//#define TYPE_MIN(TYPE) (TYPE_IS_SIGN(TYPE) \
//? ~(TYPE)0 << TYPE_VALUE_BITS(TYPE) \
//: 0)

//#define TYPE_MAX(TYPE) (TYPE_IS_SIGN(TYPE) \
//? ~(~(TYPE)0 << TYPE_VALUE_BITS(TYPE)) \
//: (TYPE)-1)

#define TYPE_MIN(TYPE) (TYPE_IS_SIGN(TYPE) \
? (TYPE) 1 << TYPE_VALUE_BITS(TYPE) : 0)

#define TYPE_MAX(TYPE) (TYPE_IS_SIGN(TYPE) \
? ~TYPE_MIN(TYPE) : (TYPE) -1)

/*
// C allows floating-point time_t, but we don't support it.
BUILD_ASSERT_DECL(TYPE_IS_INT(time_t));

// We do try to cater to unsigned time_t, but I want to know about it if we
// ever encounter such a platform.
BUILD_ASSERT_DECL(TYPE_IS_SIGN(time_t));
*/

//#define TIME_MAX TYPE_MAX(time_t)
//#define TIME_MIN TYPE_MIN(time_t)

int main() {

    //time_t TIME_MIN = (time_t) 1 << (sizeof(time_t) * 8 - 1);
    //time_t TIME_MAX = ~TIME_MIN;
    //printf("TIME_MIN: %ld\n", TIME_MIN);
    //printf("TIME_MAX: %ld\n", TIME_MAX);
    
    printf("char        : %2lu bytes  %20d .. %d\n", sizeof(char), CHAR_MIN, CHAR_MAX);

    printf("short       : %2lu bytes  %20d .. %d\n", sizeof(short), SHRT_MIN, SHRT_MAX);
    printf("int         : %2lu bytes  %20d .. %d\n", sizeof(int), INT_MIN, INT_MAX);
    printf("long        : %2lu bytes  %20lld .. %lld\n", sizeof(long), (long long) LONG_MIN, (long long) LONG_MAX);
    printf("long long   : %2lu bytes  %20lld .. %lld\n", sizeof(long long), LLONG_MIN, LLONG_MAX);

    printf("float       : %2lu bytes  %20e .. %e\n", sizeof(float), FLT_MIN, FLT_MAX);
    printf("double      : %2lu bytes  %20e .. %e\n", sizeof(double), DBL_MIN, DBL_MAX);
    printf("long double : %2lu bytes  %20Le .. %Le\n", sizeof(long double), LDBL_MIN, LDBL_MAX);

    printf("\n");
    printf("size_t      : %2lu bytes  %20lu .. %lu\n", sizeof(size_t), 0UL, SIZE_MAX);
    printf("ssize_t     : %2lu bytes  %20lld .. %lld\n", sizeof(ssize_t), (long long) ~SSIZE_MAX, (long long) SSIZE_MAX);
    printf("time_t      : %2lu bytes  %20lld .. %lld\n", sizeof(time_t), (long long) TYPE_MIN(time_t), (long long) TYPE_MAX(time_t));

    return 0;
}
