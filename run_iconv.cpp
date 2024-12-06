#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "iolib.h"
#include "encdef.h"
#include "unimap.h"

void usage() {
    fprintf(stderr, "Usage: run-iconv -f fromcode -t tocode file\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 5;
    if (argc < min_arg + 1) {
        usage();
        return 1;
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
        return 1;
    }

    //if (argc - optind < min_arg) {
    //    fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
    //    usage();
    //    return 0;
    //}

    char* file_name = argv[optind];

    //printf("from_encoding   : %s\n", from_encoding);
    //printf("to_encoding     : %s\n", to_encoding);
    //printf("file_name       : %s\n", file_name);

    int from_encoding_id = lib_enc_get_encoding_id(from_encoding);
    if (from_encoding_id == 0) {
        error = true;
        fprintf(stderr, "Encoding 'from' not found: %s\n", from_encoding);
    }

    int to_encoding_id = lib_enc_get_encoding_id(to_encoding);
    if (to_encoding_id == 0) {
        error = true;
        fprintf(stderr, "Encoding 'to' not found: %s\n", to_encoding);
    }

    if (error) {
        return 1;
    }

    //printf("from_encoding_id: %d\n", from_encoding_id);
    //printf("to_encoding_id  : %d\n", to_encoding_id);

    size_t size = 0;
    char* data = lib_io_read_bytes(file_name, size);

    int ret = lib_unimap_conv_id(from_encoding_id, to_encoding_id, data, size);
    if (ret != 0) {
        fprintf(stderr, "Converting error\n");
        return 1;
    }

    for (int i = 0; i < size; i++) {
        printf("%c", data[i]);
    }

}
