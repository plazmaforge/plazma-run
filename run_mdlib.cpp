#include <stdio.h>
#include <errno.h>

#include "getopt.h"
#include "iolib.h"

#include "run_mdlib.h"

static void print_title(lib_md_config_t* config, const char* title) {
    if (!title) {
        return;
    }
    printf("%s (\"%s\") = ", config->md_name, title);
}

static void print_sum(lib_md_config_t* config, unsigned char sum[]) {
    const char* fmt = config->is_upper  ? "%02X" : "%02x";
    for (int i = 0; i < config->md_size; i++) {
        printf(fmt, sum[i]);
    }
}

static void print_result(lib_md_config_t* config, const char* title, unsigned char sum[]) {
    if (config->is_title) {
        print_title(config, title);
    }
    print_sum(config, sum);
    printf("\n");
}

static void print_error(lib_md_config_t* config, const char* error) {
    fprintf(stderr, "Error calculation %s for %s: %s\n", config->md_name, (config->mode == RUN_MD_BY_STRING ? "string" : "file"), error);
}

static const char* get_base_name(const char* name) {
    if (!name) {
        return NULL;
    }

    const char* base = strrchr(name, '\\');
    const char* base2 = strrchr(name, '/');

    if (base2 > base) {
        base = base2;
    }

    base = base ? (base + 1): name;

    return base;
}

void run_md_usage(lib_md_config_t* config) {

    const char* base_name = get_base_name(app_name);

    fprintf(stderr, "Usage: %s [-tu] -s string | file ...\n", base_name);
}

int run_md_by_mode(lib_md_config_t* config, const char* file_name, const char* data, size_t size) {
    if (!data) {
        print_error(config, (config->mode == RUN_MD_BY_STRING ? "String is empty" : "Data is empty"));
        return 1;
    }
    unsigned char sum[config->md_size];
    if (config->md_func((const unsigned char*) data, size, sum) != 0) {
        print_error(config, (config->mode == RUN_MD_BY_STRING ? data : file_name));
        return 1;
    }
    print_result(config, (config->mode == RUN_MD_BY_STRING ? data : file_name), sum);
    return 0;
}

int run_md(lib_md_config_t* config, int argc, char* argv[]) {

    app_name = argv[0];

    if (argc < 2) {
        run_md_usage(config);
        return 1;
    }

    bool error = false;
    int opt;

    config->mode = RUN_MD_BY_FILE; /* Set mode by file */
    config->is_title = false;
    config->is_upper = false;

    char* data = NULL;
    size_t size = 0;

    while ((opt = getopt(argc, argv, "tus:")) != -1) {
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
            errno = 0;

            //size = 0;
            //data = lib_io_read_bytes(file_name, size);

            ////
            data = NULL;
            size = 0;
            int retval = lib_io_read_all_bytes(file_name, &data);
            if (retval < 0) {
                // error
                if (data) {
                    free(data);
                }
                error = 1;
                fprintf(stderr, "%s: %s: %s\n", app_name, file_name, strerror(errno));
                continue;
            }
            size = retval;
            ////

            // ERROR
            //if (errno != 0) {
            //    if (data) {
            //        free(data);
            //    }
            //    error = 1;
            //    fprintf(stderr, "%s: %s: %s\n", app_name, file_name, strerror(errno));
            //    continue;
            //}

            // NO DATA
            if (!data) {
                error = 1;
                fprintf(stderr, "%s: %s: %s\n", app_name, file_name, "No data");
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

