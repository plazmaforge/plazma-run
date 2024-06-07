#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "getopt.h"
#include "iolib.h"
#include "md5.h"

#define MD_SIZE 16

void usage() {
    fprintf(stderr, "Usage: run-md5 -s string\n");
    fprintf(stderr, "       run-md5 file ...\n");
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

int run_md5_by_string(const char* str) {
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

int run_md5_by_file(const char* data, size_t size) {
    if (!data) {
        fprintf(stderr, "Error calculation md5: Data is empty\n");
        return 1;
    }
    unsigned char sum[MD_SIZE];
    if (lib_md5((const unsigned char*) data, size, sum) != 0) {
        fprintf(stderr, "Error calculation md5 for file\n");
        return 1;
    }
    print_result(data, sum, 1, 0);
    return 0;
}

int run_md5_by_files(const char* data, size_t size) {
    if (!data) {
        fprintf(stderr, "Error calculation md5: Data is empty\n");
        return 1;
    }
    unsigned char sum[MD_SIZE];
    if (lib_md5((const unsigned char*) data, size, sum) != 0) {
        fprintf(stderr, "Error calculation md5 for file(s)\n");
        return 1;
    }
    print_result(data, sum, 1, 0);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 1;
    }

    bool error = false;
    int opt;
    bool is_str = false;
    char* str = NULL;

    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            is_str = true;
            str = optarg;
            break;
        case '?':
            error = true;
            break;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    if (is_str) {
        return run_md5_by_string(str);
    } else {

        const char* file_name;
        char* data;
        size_t size;
        error = 0;

        for (int i = 1; i < argc; i++) {
            file_name = argv[i];
            size = 0;
            errno = 0;
            data = lib_io_read_bytes(file_name, size);
            if (errno != 0) {
                error = 1;
                fprintf(stderr, "run-md5: %s: %s\n", file_name, strerror(errno));                
                continue;
            }
            if (run_md5_by_file(data, size) != 0) {
                error = 1;
            }
        }

        return error;
    }

}