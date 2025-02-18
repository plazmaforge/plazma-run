#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"

#include "base32.h"
#include "iolib.h"

int run_base32_encode_data(const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    size_t osize = 0;
    char* odata = lib_base32_encode(data, size, &osize);
    if (!odata) {
        fprintf(stderr, "Empty output data\n");
        return 1;
    }
    for (size_t i = 0; i < osize; i++) {
        fprintf(stdout, "%c", odata[i]);
    }
    fprintf(stdout, "\n");
    free(odata);
    return 0;
}

int run_base32_decode_data(const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    size_t osize = 0;
    char* odata = lib_base32_decode(data, size, &osize);
    if (!odata) {
        fprintf(stderr, "Empty output data\n");
        return 1;
    }
    fprintf(stdout, "%s", odata);
    //for (size_t i = 0; i < osize; i++) {
    //    fprintf(stdout, "%c", odata[i]);
    //}
    fprintf(stdout, "\n");
    free(odata);
    return 0;
}

int run_base32_encode_file(const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* data = NULL;
    size_t size = 0;
    int retval = lib_io_read_all_bytes(file_name, &data, &size);
    if (retval < 0) {

        // error
        if (data) {
            free(data);
        }
        //error = 1;
        //_file_error(config, file_name);
        return 1;
    }
    retval = run_base32_encode_data(data, size);
    if (data) {
        free(data);
    }
    return retval;
}

int run_base32_decode_file(const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* data = NULL;
    size_t size = 0;
    int retval = lib_io_read_all_bytes(file_name, &data, &size);
    if (retval < 0) {

        // error
        if (data) {
            free(data);
        }
        //error = 1;
        //_file_error(config, file_name);
        return 1;
    }
    retval = run_base32_decode_data(data, size);
    if (data) {
        free(data);
    }
    return retval;
}

void usage() {
    fprintf(stderr, "Usage: run-base32 [-D] -s string | file\n");
}

int main(int argc, char* argv[]) {

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;

    bool flag_decode = false;
    bool flag_string = false;
    char* data       = NULL;

    if (argc < 2) {
        usage();
        return 1;
    }

    while ((opt = lib_getopt(argc, argv, "Ds:")) != -1) {
        switch (opt) {
        case 'D':
            /* Set decode flag */
            flag_decode = true;
            break;
        case 's':
            /* Set mode by string */
            flag_string = true;
            data = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    //printf("argc  : %i\n", argc);
    //printf("optind: %i\n", optind);

    if (flag_string) {
        if (!data) {
            fprintf(stderr, "Empty input string\n");
            return 1;
        }
        if (flag_decode) {
            return run_base32_decode_data(data, strlen(data));
        } else {
            return run_base32_encode_data(data, strlen(data));
        }        
    } else {
        if (optind >= argc) {
            usage();
            return 1;
        }
        char* file_name = argv[optind];
        //fprintf(stderr, "file: %s\n", file_name);
        if (flag_decode) {
            return run_base32_decode_file(file_name);
        } else {
            return run_base32_encode_file(file_name);
        }        
    }
    return 0;
}   
 