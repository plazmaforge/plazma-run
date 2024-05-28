#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"

#define MD_SIZE 16

void usage() {
    fprintf(stderr, "Usage: run-md5 string\n");
}

void print_str(const char* str) {
    printf("MD5 (\"%s\") = ", str);
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

int run_md5(const char* str) {
    if (!str) {
        fprintf(stderr, "Error calculation md5: String is empty\n");
        return 1;
    }
    int len = strlen(str);
    unsigned char sum[MD_SIZE];
    if (lib_md5((const unsigned char*) str, len, sum) != 0) {
        fprintf(stderr, "Error calculation md5 for string: %s\n", str);
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
    return run_md5(str);
}