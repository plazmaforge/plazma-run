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

#define TYPE_IS_INTEGER(TYPE) ((TYPE) 1.5 == (TYPE) 1)
#define TYPE_IS_SIGNED(TYPE) ((TYPE) 0 > (TYPE) -1)
#define TYPE_VALUE_BITS(TYPE) (sizeof(TYPE) * CHAR_BIT - TYPE_IS_SIGNED(TYPE))
#define TYPE_MIN(TYPE) (TYPE_IS_SIGNED(TYPE) \
? ~(TYPE)0 << TYPE_VALUE_BITS(TYPE) \
: 0)
#define TYPE_MAX(TYPE) (TYPE_IS_SIGNED(TYPE) \
? ~(~(TYPE)0 << TYPE_VALUE_BITS(TYPE)) \
: (TYPE)-1)


// C allows floating-point time_t, but we don't support it.
BUILD_ASSERT_DECL(TYPE_IS_INTEGER(time_t));

// We do try to cater to unsigned time_t, but I want to know about it if we
// ever encounter such a platform.
BUILD_ASSERT_DECL(TYPE_IS_SIGNED(time_t));


#define TIME_MAX TYPE_MAX(time_t)
#define TIME_MIN TYPE_MIN(time_t)
*/

void setmax(void* ptr, size_t size) {
    char* p = (char*) ptr;    
    memset(p, 0xFF, size - 1);
    memset(p + size - 1, 0x7F, 1);
}

int main() {

    time_t TIME_MAX;
    setmax(&TIME_MAX, sizeof(time_t));
    //printf("time: %ld\n", TIME_MAX);
    
    printf("char        : %2lu bytes\t %d .. %d\n", sizeof(char), CHAR_MIN, CHAR_MAX);

    printf("short       : %2lu bytes\t %d .. %d\n", sizeof(short), SHRT_MIN, SHRT_MAX);
    printf("int         : %2lu bytes\t %d .. %d\n", sizeof(int), INT_MIN, INT_MAX);
    printf("long        : %2lu bytes\t %lld .. %lld\n", sizeof(long), (long long) LONG_MIN, (long long) LONG_MAX);
    printf("long long   : %2lu bytes\t %lld .. %lld\n", sizeof(long long), LLONG_MIN, LLONG_MAX);

    printf("float       : %2lu bytes\t %e .. %e\n", sizeof(float), FLT_MIN, FLT_MAX);
    printf("double      : %2lu bytes\t %e .. %e\n", sizeof(double), DBL_MIN, DBL_MAX);
    printf("long double : %2lu bytes\t %Le .. %Le\n", sizeof(long double), LDBL_MIN, LDBL_MAX);

    printf("\n");
    printf("size_t      : %2lu bytes\t %d .. %lu\n", sizeof(size_t), 0, SIZE_MAX);
    printf("ssize_t     : %2lu bytes\t %ld .. %ld\n", sizeof(ssize_t), (SSIZE_MAX * -1) - 1, SSIZE_MAX);
    //printf("time_t      : %2lu bytes\t %lld .. %lld\n", sizeof(time_t), (long long) TYPE_MIN(time_t), (long long) TIME_MAX);
    printf("time_t      : %2lu bytes\t %lld .. %lld\n", sizeof(time_t), (long long) ((TIME_MAX * -1) - 1), (long long) TIME_MAX);

    return 0;
}
