#include <stdio.h>
#include <stdlib.h>

#include "getopt.h"
#include "iolib.h"
#include "enclib.h"

void usage() {
    fprintf(stderr, "Usage: run-iconv -f fromcode -t tocode file\n");
}

static void _conv_error(int error_code, const char* prog_name, char* from_code, char* to_code) {
    if (error_code == LIB_ENC_ERR_CONV_FROM_USUPPORTED || error_code == LIB_UNIMAP_ERR_CONV_FROM_USUPPORTED) {
        fprintf(stderr, "%s: Conversion from %s unsupported\n", prog_name, from_code);
        return;
    }    
    if (error_code == LIB_ENC_ERR_CONV_TO_USUPPORTED || error_code == LIB_UNIMAP_ERR_CONV_TO_USUPPORTED) {
        fprintf(stderr, "%s: Conversion to %s unsupported\n", prog_name, to_code);
        return;
    }    
    if (error_code == LIB_ENC_ERR_CONV_USUPPORTED || error_code == LIB_UNIMAP_ERR_CONV_USUPPORTED) {
        fprintf(stderr, "%s: Conversion from %s unsupported\n", prog_name, from_code);
        fprintf(stderr, "%s: Conversion to %s unsupported\n", prog_name, to_code);
        return;
    }    
    fprintf(stderr, "%s: Conversion error\n", prog_name);
}

static void _file_error(int error_code, const char* prog_name, const char* file_name) {
    fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
}

static void _data_error(int error_code, const char* prog_name) {
    if (error_code == 1) {
        fprintf(stderr, "%s: Empty input data\n", prog_name);
        return;
    }    
    if (error_code == 2) {
        fprintf(stderr, "%s: Empty output data\n", prog_name);
        return;
    }
    fprintf(stderr, "%s: Data error\n", prog_name); 
}

static bool _is_empty_data(char* data, size_t size) {
    return !data || size == 0;
}

static void _data_free(char* from_data, char* to_data) {
    if (!from_data && !to_data) {
        return;
    }

    if (from_data) {
        free(from_data);
        //printf("%s", ">> data 'from' free\n");
        return;
    }

    if (to_data && from_data != to_data) {
        free(to_data);
        //printf("%s", ">> data 'to' free\n");
        return;
    }
}

void _print_encodings() {
    size_t size = lib_enc_get_encoding_size();
    for (size_t i = 0; i < size; i++) {        
        lib_encoding_t e = lib_enc_get_encoding(i);
        if (!lib_enc_supports_conv(e.id)) {
            continue;
        }
        printf("%-15s %-25s %s\n", e.name, e.description, lib_enc_get_encoding_type_name(e.id));
    }
}

int main(int argc, char* argv[]) {

    int min_arg = 1;
    if (argc < min_arg + 1) {
        usage();
        return 1;
    }

    const char* prog_name = getopt_prog_name(argv);
    bool error = false;
    int opt;

    char* from_code = NULL;
    char* to_code   = NULL;
    bool opt_list   = false;

    while ((opt = getopt(argc, argv, "f:t:l")) != -1) {
        switch (opt) {
        case 'l':
            opt_list = true;
            break;
        case 'f':
            from_code = optarg;
            break;
        case 't':
            to_code = optarg;
            break;
        case '?':
            error = true;
            break;
        case ':':
            error = true;
            break;
        }

        if (opt_list) {
            break;
        }
    }

    if (opt_list) {
        _print_encodings();
        return 0;
    }

    if (!from_code || !to_code) {
        error = true;
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

    int from_id   = lib_enc_get_conv_encoding_id(from_code);
    int to_id     = lib_enc_get_conv_encoding_id(to_code);
    bool has_from = from_id > 0;
    bool has_to   = to_id > 0;

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

    int retval = lib_io_read_all_bytes(file_name, &from_data);
    if (retval < 0) {
        _file_error(retval, prog_name, file_name);
        _data_free(from_data, to_data);
        return 1;
    }
    from_size = retval;

    if (_is_empty_data(from_data, from_size)) {
        _data_error(1, prog_name);
        _data_free(from_data, to_data);
        return 1;
    }
    
    retval =  lib_enc_conv_by_id(from_id, to_id, from_data, from_size, &to_data, &to_size);
    if (retval != 0) {
        _conv_error(retval, prog_name, from_code, to_code);
        _data_free(from_data, to_data);
        return 1;
    }
    if (_is_empty_data(to_data, to_size)) {
        _data_error(2, prog_name);
        _data_free(from_data, to_data);
        return 1;
    }

    //printf(">> from ptr : %p\n", from_data);
    //printf(">> to   ptr : %p\n", to_data);

    to_data[to_size] = '\0';
    //puts(to_data);
    printf("%s", to_data);

    //for (int i = 0; i < to_size; i++) {
    //    printf("%c", to_data[i]);
    //}

    _data_free(from_data, to_data);

}
