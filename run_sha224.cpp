#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sha256.h"

#define MD_SIZE 28

void usage() {
    fprintf(stderr, "Usage: run-sha224 string\n");
}

void print_str(const char* str) {
    printf("SHA224 (\"%s\") = ", str);
}

void print_sum(unsigned char sum[MD_SIZE], int upper) {
    for (int i = 0; i < MD_SIZE; i++) {
        if (upper) {
            printf("%02X", sum[i]);
        } else {
            printf("%02x", sum[i]);
        }        
    }
}

void print_result(const char* str, unsigned char sum[MD_SIZE], int mode, int upper) {
    if (mode == 1) {
        print_sum(sum, upper);
    } else {
        print_str(str);
        print_sum(sum, upper);
    }
    printf("\n");
}

int run_sha224(const char* str) {
    if (!str) {
        fprintf(stderr, "Error calculation sha224: String is empty\n");
        return 1;
    }
    int len = strlen(str);
    unsigned char sum[MD_SIZE];
    if (lib_sha256((const unsigned char*) str, len, sum, 1) != 0) { // is224 = 1
        fprintf(stderr, "Error calculation sha224 for string: %s\n", str);
        return 1;
    }
    print_result(str, sum, 1, 0);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }
    char* str = argv[1];
    return run_sha224(str);
}