#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "iolib.h"
#include "enclib.h"

void usage() {
    fprintf(stderr, "Usage: run-iconv -f fromcode -t tocode file\n");
}

void _conv_error(int errno, const char* prog_name, char* from_code, char* to_code) {
    if (errno == LIB_ENC_ERR_CONV_FROM_USUPPORTED || errno == LIB_UNIMAP_ERR_CONV_FROM_USUPPORTED) {
        fprintf(stderr, "%s: Conversion from %s unsupported\n", prog_name, from_code);
        return;
    }    
    if (errno == LIB_ENC_ERR_CONV_TO_USUPPORTED || errno == LIB_UNIMAP_ERR_CONV_TO_USUPPORTED) {
        fprintf(stderr, "%s: Conversion to %s unsupported\n", prog_name, to_code);
        return;
    }    
    if (errno == LIB_ENC_ERR_CONV_USUPPORTED || errno == LIB_UNIMAP_ERR_CONV_USUPPORTED) {
        fprintf(stderr, "%s: Conversion from %s unsupported\n", prog_name, from_code);
        fprintf(stderr, "%s: Conversion to %s unsupported\n", prog_name, to_code);
        return;
    }    
    fprintf(stderr, "%s: Conversion error\n", prog_name);
}

void _data_error(int errno, const char* prog_name) {
    if (errno == 1) {
        fprintf(stderr, "%s: Empty input data\n", prog_name);
        return;
    }    
    if (errno == 2) {
        fprintf(stderr, "%s: Empty output data\n", prog_name);
        return;
    }
    fprintf(stderr, "%s: Data error\n", prog_name); 
}

bool _is_empty_data(char* data, size_t size) {
    return !data || size == 0;
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

    //printf(">> from_code   : %s\n", from_code);
    //printf(">> to_code     : %s\n", to_code);
    //printf(">> file_name   : %s\n", file_name);

    int from_id   = lib_enc_get_encoding_id(from_code);
    int to_id     = lib_enc_get_encoding_id(to_code);
    bool has_from = from_id > 0;
    bool has_to   = to_id > 0;

    if (has_from && from_id != LIB_ENC_UTF8_ID) {
        has_from = lib_unimap_supports_map(from_id);
    }
    if (has_to && to_id != LIB_ENC_UTF8_ID) {
        has_to = lib_unimap_supports_map(to_id);
    }

    if (!has_from) {
        error = true;
        _conv_error(LIB_ENC_ERR_CONV_FROM_USUPPORTED, prog_name, from_code, to_code);
    }
    if (!has_to) {
        error = true;
        _conv_error(LIB_ENC_ERR_CONV_TO_USUPPORTED, prog_name, to_code, to_code);
    }

    //if (!lib_fs_exists(file_name)) {
    //    error = true;
    //    fprintf(stderr, "File not found           : %s\n", file_name);
    //}

    if (error) {
        return 1;
    }

    //printf(">> from_id : %d\n", from_id);
    //printf(">> to_id   : %d\n", to_id);
    
    char* from_data  = NULL;
    size_t from_size = 0;
    char* to_data    = NULL;
    size_t to_size   = 0;

    from_data = lib_io_read_bytes(file_name, from_size);
    if (_is_empty_data(from_data, from_size)) {
        _data_error(1, prog_name);
        return 1;
    }

    int errno = 0;
    if (has_from && to_id == LIB_ENC_UTF8_ID) {
        errno = lib_enc_conv_to_utf8_by_id(from_id, from_data, from_size, &to_data, &to_size);
        if (errno != 0) {
            _conv_error(errno, prog_name, from_code, to_code);
            free(from_data);
            free(to_data);
            return 1;
        }
        if (_is_empty_data(to_data, to_size)) {
            _data_error(2, prog_name);
            free(from_data);
            free(to_data);
            return 1;
        }
    } else {
        errno = lib_unimap_conv_by_id(from_id, to_id, from_data, from_size);
        if (errno != 0) {
            _conv_error(errno, prog_name, from_code, to_code);
            free(from_data);
            return 1;
        }
        to_data = from_data;
        to_size = from_size;
    }

    //printf(">> from ptr : %p\n", from_data);
    //printf(">> to   ptr : %p\n", to_data);

    to_data[to_size] = '\0';
    //puts(to_data);
    printf("%s", to_data);

    //for (int i = 0; i < to_size; i++) {
    //    printf("%c", to_data[i]);
    //}

    free(from_data);
    //printf("%s", ">> from free\n");
    if (from_data != to_data) {
        free(to_data);
        //printf("%s", ">> to free\n");
    }

}
