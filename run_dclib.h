#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

static int lib_str2num(const char* str, char** endptr, int base, uint64_t* num) {
    *num  = 0;
    errno = 0;
    unsigned long long value = strtoull(str, endptr, base);

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

