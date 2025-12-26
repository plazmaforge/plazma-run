#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

static int lib_str2num(const char* str, char** endptr, int base, uint64_t* num) {
    unsigned long long value = strtoull(str, endptr, base);
    errno = 0;

    if (errno == ERANGE) {
       *num = 0;
       // Out of range
       return ERANGE;
    }

    if (*endptr == str) {
        *num = 0;
        // No digits
        return -1;
    }

    if (**endptr != '\0') {
       *num = 0;
       // No digits after number
       return -2;
    }

    *num = (uint64_t) value;
    return 0;

}

static int run_str2num(const char* str, int base, uint64_t* num) {
    char* endptr;
    int error = lib_str2num(str, &endptr, base, num);

    if (error == ERANGE) {
       fprintf(stderr, "Value out of range\n");
       return 1;
    } else if (error == -1) {
        fprintf(stderr, "No digits were found in the string\n");
        return 1;
    } else if (error == -2) {
       fprintf(stderr, "Further characters after number: %s\n", endptr);
       return 1;
    }

    return 0;
}

