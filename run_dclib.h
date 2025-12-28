#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

static uint8_t lib_chartobit(char value, int i) {
    return ( (value & (1 << i)) ? 1 : 0 );
}

static char lib_chartobin(char value, int i) {
    return ( (value & (1 << i)) ? '1' : '0' );
}

static uint8_t lib_uint32tobit(uint32_t value, int i) {
    return ( (value & (1 << i)) ? 1 : 0 );
}

static char lib_uint32tobin(uint32_t value, int i) {
    return ( (value & (1 << i)) ? '1' : '0' );
}

static uint8_t lib_uint64tobit(uint64_t value, int i) {
    return ( (value & (1L << i)) ? 1 : 0 );
}

static char lib_uint64tobin(uint64_t value, int i) {
    return ( (value & (1L << i)) ? '1' : '0' );
}

/**
 * Calculate count of bits in char value
 */
static int lib_uint64bit_count(uint64_t value) {
    int count = 64;
    int n = count;
    bool start = true;
    for (int i = 0; i < count; i++) {

        int bit = lib_uint64tobit(value, count - i - 1);
        //fprintf(stderr, "bit[%02d]: %d\n", i, bit);

        if (!start) {
            continue;
        }

        //if (lib_uint64tobit(value, i) == 0) {
        if (bit == 0) {
            n--;
        } else {
            start = false;
        }
    }
    return n;
}

static int lib_uint32bit_count(uint32_t value) {
    return lib_uint64bit_count(value);
}

/**
 * Calculate count of bits in char value
 */
static int lib_charbit_count(char value) {
    uint8_t u = (uint8_t) value;
    return lib_uint64bit_count(u);
}

static void lib_chartobin_buf(char* buf, char value, int count) {
    int n = count - 1;
    for (int i = n; i >= 0; --i) {
        buf[n - i] = lib_chartobin(value, i);
    }
}

static void lib_uint64tobin_buf(char* buf, uint64_t value, int count) {
    int n = count - 1;
    for (int i = n; i >= 0; --i) {
        buf[n - i] = lib_uint64tobin(value, i);
    }
}

static void lib_chartobin8(char* buf, char c) {
    lib_chartobin_buf(buf, c, 8);
}


static int lib_fprint_bin(FILE* file, uint64_t value) {

    // Calculate count of bits
    int count = lib_uint64bit_count(value);
    //fprintf(stderr, "count: %d\n", count);

    if (count == 0) {
        count = 1;
    }

    char buf[count + 1];
    buf[count] = '\0';
    lib_uint64tobin_buf(buf, value, count);

    fprintf(stdout, "%s", buf);

    return count;
}

static void _shift_bin_count(char* buf, int count) {
    int mid = count / 2;
    int n = count - 1;
    for (int i = n; i >= mid; --i) {
        buf[i + 1] = buf[i];
    }
    buf[mid] = ' ';
}

static int lib_str2num(const char* str, char** endptr, int base, uint64_t* num) {
    *num  = 0;
    errno = 0;
    unsigned long long value = strtoull(str, endptr, base);

    //fprintf(stderr, "errno: %d\n", errno);

    if (errno == EINVAL) {
       // Invalid value
       return EINVAL;
    }
    
    if (errno == ERANGE || value > UINT64_MAX) {
       // Out of range
       return ERANGE;
    }

    if (*endptr == str) {
        // No digits
        return -1;
    }

    if (**endptr != '\0') {
       // No digits after number
       return -2;
    }

    if (errno != 0) {
        // Unknown error
        return errno;
    }

    *num = (uint64_t) value;
    return 0;

}

static int run_str2num(const char* str, int base, uint64_t* num) {
    char* endptr;
    int error = lib_str2num(str, &endptr, base, num);
    if (error == 0) {
        return 0;
    }

    if (error == EINVAL) {
       fprintf(stderr, "Invalid value\n");
       return 1;
    }
    if (error == ERANGE) {
       fprintf(stderr, "Value out of range\n");
       return 1;
    }
    if (error == -1) {
        fprintf(stderr, "No digits were found in the string\n");
        return 1;
    }
    if (error == -2) {
       fprintf(stderr, "Further characters after number: %s\n", endptr);
       return 1;
    }

    fprintf(stderr, "Unknown error\n");
    return 1;
}

