#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha512.h"

#define MD_SIZE 64

void usage() {
    fprintf(stderr, "Usage: run-sha512 <str>\n");
}

void run_print_str(const char* str) {
    printf("SHA512 (\"%s\") = ", str);
}

void run_print_sum(unsigned char sum[MD_SIZE], int upper) {
    for (int i = 0; i < MD_SIZE; i++) {
        if (upper) {
            printf("%02X", sum[i]);
        } else {
            printf("%02x", sum[i]);
        }        
    }
}

void run_print_result(const char* str, unsigned char sum[MD_SIZE], int mode, int upper) {
    if (mode == 1 || mode == 2) {
        run_print_sum(sum, upper);
    } else {
        run_print_str(str);
        run_print_sum(sum, upper);
    }
    printf("\n");
}

int run_sha512(const char* str) {
    if (!str) {
        return -1;
    }
    int len = strlen(str);
    unsigned char sum[MD_SIZE];
    if (lib_sha512((const unsigned char*) str, len, sum, 0) != 0) {
        fprintf(stderr, "Error run sha512 for string: %s\n", str);
        return -1;
    }
    run_print_result(str, sum, 1, 0);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    char* str = argv[1];
    run_sha512(str);
    return 0;
}