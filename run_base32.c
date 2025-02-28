#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "getopt.h"
#include "clilib.h"

#include "base32.h"
#include "iolib.h"

static void _file_error(const char* file_name) {
    fprintf(stderr, "%s: %s: %s\n", prog_name, file_name, strerror(errno));
}

static void _encode_error() {
    fprintf(stderr, "%s: %s\n", prog_name, "Encoding error");
}

static void _decode_error() {
    fprintf(stderr, "%s: %s\n", prog_name, "Invalid character in input stream");
}

int run_base32_encode_data(int type, const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    size_t osize = 0;
    char* odata = lib_base32_encode_type(type, data, size, &osize);
    if (!odata) {
        _encode_error();
        return 1;
    }
    for (size_t i = 0; i < osize; i++) {
        fprintf(stdout, "%c", odata[i]);
    }
    fprintf(stdout, "\n");
    free(odata);
    return 0;
}

int run_base32_decode_data(int type, const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    size_t osize = 0;
    char* odata = lib_base32_decode_type(type, data, size, &osize);
    if (!odata) {
        _decode_error();
        return 1;
    }
    //fprintf(stdout, "%s", odata);
    for (size_t i = 0; i < osize; i++) {
        fprintf(stdout, "%c", odata[i]);
    }
    //fprintf(stdout, "\n");
    free(odata);
    return 0;
}

int run_base32_encode_file(int type, const char* file_name) {
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
        _file_error(file_name);
        return 1;
    }
    retval = run_base32_encode_data(type, data, size);
    if (data) {
        free(data);
    }
    return retval;
}

int run_base32_decode_file(int type, const char* file_name) {
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
        _file_error(file_name);
        return 1;
    }
    retval = run_base32_decode_data(type, data, size);
    if (data) {
        free(data);
    }
    return retval;
}

void usage() {
    fprintf(stderr, "Usage: run-base32 [-base32 | -base32u | -base32l | zbase32 | base32hex | cbase32] [-D] -s string | file\n");
}

int main(int argc, char* argv[]) {

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;
    int long_ind;

    bool flag_decode = false;
    bool flag_string = false;
    char* data       = NULL;
    int type         = LIB_BASE32;

    if (argc < 2) {
        usage();
        return 1;
    }

    static lib_option long_options[] = {
          {"base32",    no_argument, 0, 1000},
          {"base32u",   no_argument, 0, 1001},
          {"base32l",   no_argument, 0, 1002},
          {"zbase32",   no_argument, 0, 1003},
          {"base32hex", no_argument, 0, 1004},
          {"cbase32",   no_argument, 0, 1005},
          {NULL,        0,           0, 0}
    };

    while ((opt = lib_getopt_long(argc, argv, "Ds:", long_options, &long_ind)) != -1) {
    //while ((opt = lib_getopt(argc, argv, "Ds:")) != -1) {
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
        case 1000:
            type = LIB_BASE32;
            break;
        case 1001:
            type = LIB_BASE32U;
            break;
        case 1002:
            type = LIB_BASE32L;
            break;
        case 1003:
            type = LIB_ZBASE32;
            break;
        case 1004:
            type = LIB_BASE32HEX;
            break;
       case 1005:
            type = LIB_CBASE32;
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

    #ifdef _WIN32
    int result = _setmode(_fileno(stdout), _O_BINARY);
    if (result == -1) {
        fprintf(stderr, "%s: %s\n", prog_name, "Cannot set binary mode in stdout");
    }
    #endif

    if (flag_string) {
        if (!data) {
            fprintf(stderr, "%s: %s\n", prog_name, "Empty input string");
            return 1;
        }
        if (flag_decode) {
            return run_base32_decode_data(type, data, strlen(data));
        } else {
            return run_base32_encode_data(type, data, strlen(data));
        }        
    } else {
        if (optind >= argc) {
            usage();
            return 1;
        }
        char* file_name = argv[optind];
        //fprintf(stderr, "file: %s\n", file_name);
        if (flag_decode) {
            return run_base32_decode_file(type, file_name);
        } else {
            return run_base32_encode_file(type, file_name);
        }        
    }
    return 0;
}   
 