#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "iolib.h"
#include "encdef.h"
#include "enclib.h"
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

    const char* prog_name = getopt_prog_name(argv);
    bool error = false;
    int opt;

    char* from_code = NULL;
    char* to_code   = NULL;

    while ((opt = getopt(argc, argv, "f:t:")) != -1) {
        switch (opt) {
        case 'f':
            if (optarg) {
                from_code = optarg;
            } else {
                error = true;
            }
            break;
        case 't':
            if (optarg) {
                to_code = optarg;
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
    //    fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
    //    usage();
    //    return 0;
    //}

    char* file_name = argv[optind];

    //printf("from_code   : %s\n", from_code);
    //printf("to_code     : %s\n", to_code);
    //printf("file_name   : %s\n", file_name);

    int from_id = lib_enc_get_encoding_id(from_code);
    int to_id = lib_enc_get_encoding_id(to_code);

    if (from_id == 0) {
        error = true;
        fprintf(stderr, "%s: Encoding from %s not found\n", prog_name, from_code);
    }
    if (to_id == 0) {
        error = true;
        fprintf(stderr, "%s: Encoding to %s not found\n", prog_name, to_code);
    }

    //if (!lib_fs_exists(file_name)) {
    //    error = true;
    //    fprintf(stderr, "File not found           : %s\n", file_name);
    //}

    if (error) {
        return 1;
    }

    //printf("from_id : %d\n", from_id);
    //printf("to_id   : %d\n", to_id);

    size_t size = 0;
    char* data = lib_io_read_bytes(file_name, size);
    if (!data) {
        fprintf(stderr, "%s: Empty data\n", prog_name);
        return 1;
    }

    bool is_b2b = true;
    char* to_data = NULL;
    size_t to_size = 0;
    if (lib_unimap_supports_map(from_id) && to_id == LIB_ENC_UTF_ID) {
        is_b2b = false;        
        to_data = lib_enc_conv_to_utf8_by_id(from_id, data, size, to_size);
        if (!to_data) {
            fprintf(stderr, "%s: Empty output data\n", prog_name);
            free(data);
            return 1;
        }
    } else {
        int ret = lib_unimap_conv_by_id(from_id, to_id, data, size);
        if (ret != 0) {
            if (ret == LIB_UNIMAP_ERR_MAP_FROM_USUPPORTED) {
                fprintf(stderr, "%s: Conversion from %s unsupported\n", prog_name, from_code);
            } else if (ret == LIB_UNIMAP_ERR_MAP_TO_USUPPORTED) {
                fprintf(stderr, "%s: Conversion to %s unsupported\n", prog_name, to_code);
            } else if (ret == LIB_UNIMAP_ERR_MAP_ALL_USUPPORTED) {
                fprintf(stderr, "%s: Conversion from %s to %s unsupported\n", prog_name, from_code, to_code);
            } else {
                fprintf(stderr, "%s: Conversion error\n", prog_name);
            }
            free(data);
            return 1;
        }
        to_data = data;
        to_size = size;
    }

    for (int i = 0; i < to_size; i++) {
        printf("%c", to_data[i]);
    }

}
