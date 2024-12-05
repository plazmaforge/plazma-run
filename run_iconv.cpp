#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "iolib.h"
#include "unimap.h"

void usage() {
    fprintf(stderr, "Usage: run-iconv -f fromcode -t tocode file\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 5;
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    bool error = false;
    int opt;

    char* from_encoding = NULL;
    char* to_encoding   = NULL;

    while ((opt = getopt(argc, argv, "f:t:")) != -1) {
        switch (opt) {
        case 'f':
            if (optarg) {
                from_encoding = optarg;
            } else {
                error = true;
            }
            break;
        case 't':
            if (optarg) {
                to_encoding = optarg;
            } else {
                error = true;
            }
            break;
        case '?':
            error = true;
            break;
        case ':':
            error = true;
            break;
        }
    }

    if (error) {
        usage();
        return 0;
    }

    //if (argc - optind < min_arg) {
    //    fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
    //    usage();
    //    return 0;
    //}

    char* file_name = argv[optind];

    //printf("from_encoding: %s\n", from_encoding);
    //printf("to_encoding  : %s\n", to_encoding);
    //printf("file_name    : %s\n", file_name);

    size_t size = 0;
    char* data = lib_io_read_bytes(file_name, size);

    int ret = lib_unimap_conv_id(866, 1251, data, size);

    for (int i = 0; i < size; i++) {
        printf("%c", data[i]);
    }

}
