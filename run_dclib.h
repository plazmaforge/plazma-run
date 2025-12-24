#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static int lib_str2num(const char* str, int base, uint64_t* num) {
    char* endptr;
    //uint64_t value;
    //errno = 0;
    unsigned long long value = strtoull(str, &endptr, base);

    //if (errno == ERANGE) {
    //    fprintf(stderr, "Value out of range\n");
    //    return 1;
    //}

    if (endptr == str) {
        fprintf(stderr, "No digits were found in the string\n");
        return 1;
    }

    //if (*endptr == '\0') {
    //    fprintf(stderr, "Further characters after number: %s\n", endptr);
    //    return 1;
    //}


    *num = (uint64_t) value;
    return 0;

}


