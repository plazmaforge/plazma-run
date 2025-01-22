#include <stdio.h>
#include <errno.h>

#include "getopt.h"
#include "arglib.h"
#include "iolib.h"

#include "run_mdlib.h"

static int _print_title(lib_md_config_t* config, const char* title) {
    if (!title) {
        return 0;
    }
    return printf("%s (\"%s\") = ", config->md_name, title);
}

static int _print_sum(lib_md_config_t* config, unsigned char sum[]) {
    const char* fmt = config->is_upper  ? "%02X" : "%02x";
    int pos = 0;
    for (int i = 0; i < config->md_size; i++) {
        pos += printf(fmt, sum[i]);
    }
    return pos;
}

static int _print_result(lib_md_config_t* config, const char* title, unsigned char sum[]) {
    int pos = 0;
    if (config->is_title) {
        pos += _print_title(config, title);
    }
    pos += _print_sum(config, sum);
    pos += printf("\n");
    return pos;
}

// strlib::strsafe

static const char* _strsafe(const char* str) {
    return str ? str : "";
}

static const char* _get_mode_type(lib_md_config_t* config) {
    return (config->mode == RUN_MD_BY_STRING ? "string" : "file");
}

static const char* _get_mode_input(lib_md_config_t* config, const char* file_name, const char* data) {
    return _strsafe(config->mode == RUN_MD_BY_STRING ? data : file_name);
}

static void _file_error(lib_md_config_t* config, const char* file_name) {
    fprintf(stderr, "%s: %s: %s\n", prog_name, file_name, strerror(errno));
}

static void _data_error(lib_md_config_t* config, const char* file_name) {
    if (config->mode == RUN_MD_BY_FILE) {
        fprintf(stderr, "%s: %s: %s\n", prog_name, file_name, "No Data");
    } else {
        fprintf(stderr, "%s: %s\n", prog_name, "No Data");
    }
}

static void _calc_error(lib_md_config_t* config, const char* file_name, const char* data) {
    fprintf(stderr, "%s: Error calculation %s for %s: %s\n", 
    prog_name,
    config->md_name, 
    _get_mode_type(config), 
    _get_mode_input(config, file_name, data));
}

int run_md_by_mode(lib_md_config_t* config, const char* file_name, const char* data, size_t size) {
    if (!data) {
        _data_error(config, file_name);
        return 1;
    }
    unsigned char sum[config->md_size];
    if (config->md_func((const unsigned char*) data, size, sum) != 0) {
        _calc_error(config, file_name, data);
        return 1;
    }
    _print_result(config, _get_mode_input(config, file_name, data), sum);
    return 0;
}

void run_md_usage(lib_md_config_t* config) {
    fprintf(stderr, "Usage: %s [-tu] -s string | file ...\n", prog_name);
}

int run_md(lib_md_config_t* config, int argc, char* argv[]) {

    prog_name = lib_arg_get_prog_name(argv);

    if (argc < 2) {
        run_md_usage(config);
        return 1;
    }

    int error = 0;
    int opt;

    config->mode = RUN_MD_BY_FILE; /* Set mode by file */
    config->is_title = false;
    config->is_upper = false;

    char* data = NULL;
    size_t size = 0;

    while ((opt = lib_getopt(argc, argv, "tus:")) != -1) {
        switch (opt) {
        case 't':
            /* Set title option */
            config->is_title = true;
            break;
        case 'u':
            /* Set upper case option */
            config->is_upper = true;
            break;
        case 's':
            /* Set mode by string */
            config->mode = RUN_MD_BY_STRING;
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
        run_md_usage(config);
        return 1;
    }

    //printf("argc  : %i\n", argc);
    //printf("optind: %i\n", optind);

    if (config->mode == RUN_MD_BY_STRING) {
        size = strlen(data);
        return run_md_by_mode(config, NULL, data, size);
    } else {

        if (argc - optind < 1) {
            //fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
            run_md_usage(config);
            return 1;
        }

        const char* file_name;
        error = 0;

        for (int i = optind; i < argc; i++) {
            file_name = argv[i];            
            errno = 0;

            ////
            data = NULL;
            size = 0;
            int retval = lib_io_read_all_bytes(file_name, &data, &size);
            if (retval < 0) {
                // error
                if (data) {
                    free(data);
                }
                error = 1;
                _file_error(config, file_name);
                continue;
            }
            ////

            // NO DATA
            if (!data) {
                error = 1;
                _data_error(config, file_name);
                continue;                    
            }

            if (run_md_by_mode(config, file_name, data, size) != 0) {    
                error = 1;
            }
            free(data);
        }

        return error;
    }

}

