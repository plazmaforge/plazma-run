#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "getopt.h"
#include "iolib.h"
#include "md5.h"

#define MD_NAME "MD5"
#define MD_SIZE 16

const int RUN_MD_BY_STRING = 1;
const int RUN_MD_BY_FILE   = 2;

typedef struct lib_md_config_t {
    const char* app_name;
    const char* md_name;
    int md_size;
    int mode;
    bool is_title;
    bool is_upper;
}
lib_md_config_t;


void run_md_usage(lib_md_config_t* config) {    
    //lib_fs_get_base_name(config->app_name)
    fprintf(stderr, "Usage: %s [-tu] -s string | file ...\n", config->app_name);
}

void print_title(lib_md_config_t* config, const char* title) {
    if (!title) {
        return;
    }
    printf("%s (\"%s\") = ", config->md_name, title);
}

void print_sum(lib_md_config_t* config, unsigned char sum[]) {
    const char* fmt = config->is_upper  ? "%02X" : "%02x";
    for (int i = 0; i < config->md_size; i++) {
        printf(fmt, sum[i]);
    }
}

void print_result(lib_md_config_t* config, const char* title, unsigned char sum[]) {
    if (config->is_title) {
        print_title(config, title);
    }
    print_sum(config, sum);
    printf("\n");
}

void print_error(lib_md_config_t* config, const char* error) {
    fprintf(stderr, "Error calculation %s for %s: %s\n", config->md_name, (config->mode == RUN_MD_BY_STRING ? "string" : "file"), error);
}

int run_md_by_mode(lib_md_config_t* config, const char* file_name, const char* data, size_t size) {
    if (!data) {
        print_error(config, (config->mode == RUN_MD_BY_STRING ? "String is empty" : "Data is empty"));
        return 1;
    }
    unsigned char sum[config->md_size];
    if (lib_md5((const unsigned char*) data, size, sum) != 0) {
        print_error(config, (config->mode == RUN_MD_BY_STRING ? data : file_name));
        return 1;
    }
    print_result(config, (config->mode == RUN_MD_BY_STRING ? data : file_name), sum);
    return 0;
}

// int run_md_by_string(lib_md_config_t* config, const char* data, size_t size) {
//     if (!data) {
//         fprintf(stderr, "Error calculation %s: String is empty\n", config->md_name);
//         return 1;
//     }
//     unsigned char sum[config->md_size];
//     if (lib_md5((const unsigned char*) data, size, sum) != 0) {
//         fprintf(stderr, "Error calculation %s for string: %s\n", config->md_name, data);
//         return 1;
//     }
//     print_result(config, data, sum);
//     return 0;
// }

// int run_md_by_file(lib_md_config_t* config, const char* data, size_t size) {
//     if (!data) {
//         fprintf(stderr, "Error calculation %s: Data is empty\n", config->md_name);
//         return 1;
//     }
//     unsigned char sum[config->md_size];
//     if (lib_md5((const unsigned char*) data, size, sum) != 0) {
//         fprintf(stderr, "Error calculation %s for file\n", config->md_name);
//         return 1;
//     }
//     print_result(config, data, sum);
//     return 0;
// }

// int run_md_by_files(lib_md_config_t* config, const char* data, size_t size) {
//     if (!data) {
//         fprintf(stderr, "Error calculation %s: Data is empty\n", config->md_name);
//         return 1;
//     }
//     unsigned char sum[config->md_size];
//     if (lib_md5((const unsigned char*) data, size, sum) != 0) {
//         fprintf(stderr, "Error calculation %s for file(s)\n", config->md_name);
//         return 1;
//     }
//     print_result(config, data, sum);
//     return 0;
// }

int run_md(lib_md_config_t* config, int argc, char* argv[]) {

    if (argc < 2) {
        run_md_usage(config);
        return 1;
    }

    bool error = false;
    int opt;

    config->mode = RUN_MD_BY_FILE;
    config->is_title = false;
    config->is_upper = false;

    char* data = NULL;
    size_t size = 0;

    while ((opt = getopt(argc, argv, "tus:")) != -1) {
        switch (opt) {
        case 't':
            config->is_title = true;
            break;
        case 'u':
            config->is_upper = true;
            break;
        case 's':
            config->mode = RUN_MD_BY_STRING;
            data = optarg;
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
            //fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
            run_md_usage(config);
            return 1;
        }

        const char* file_name;
        error = 0;

        for (int i = optind; i < argc; i++) {
            file_name = argv[i];
            size = 0;
            errno = 0;
            data = lib_io_read_bytes(file_name, size);

            // ERROR
            if (errno != 0) {
                if (data) {
                    free(data);
                }
                error = 1;
                fprintf(stderr, "%s: %s: %s\n", config->app_name, file_name, strerror(errno));
                continue;
            }

            // NO DATA
            if (!data) {
                error = 1;
                fprintf(stderr, "%s: %s: %s\n", config->app_name, file_name, "No data");
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

int main(int argc, char* argv[]) {

    lib_md_config_t config;
    config.app_name = "run-md5";
    config.md_name = MD_NAME;
    config.md_size = MD_SIZE;

    return run_md(&config, argc, argv);

}