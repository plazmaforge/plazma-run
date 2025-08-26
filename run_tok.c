#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"

static int _get_format(const char* name) {
    if (!name) {
        return -1;
    }
    // stub
    return -1;
}

static bool _is_witespace(char ch) {
    return ch == ' ';
}

static bool _is_line(char ch) {
    return ch == '\r' || ch == '\n';
}

static bool _is_delim(char ch, const char* delim, size_t size) {
    if (!delim || size == 0) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        if (ch == delim[i]) {
            return true;
        }
    }
    return false;
}

#define RUN_TOK_DELIM " ,.:;"

typedef struct run_tok_config_t {
    int format;
    const char* delim;
    bool ignore_ws;
    bool ignore_ln;
} run_tok_config_t;

int run_tok_data(run_tok_config_t* config, const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    if (size == 0) {
        return 0;
    }

    int format = config->format; 
    const char* delim = config->delim;
    //bool ignore_ws = config->ignore_ws;

    size_t delim_size = delim ? strlen(delim) : 0;

    //fprintf(stderr, ">> delim      : %s\n", delim);
    //fprintf(stderr, ">> delim_size : %lu\n", delim_size);
    //fprintf(stderr, ">> data       : %s\n", data);
    //fprintf(stderr, ">> data_size  : %lu\n", size);

    char ch = 0;
    if (delim_size == 0) {
        for (size_t i = 0; i < size; i++) {
            ch = data[i];
            if (config->ignore_ws && _is_witespace(ch)) {
                continue;
            }
            if (config->ignore_ln && _is_line(ch)) {
                continue;
            }
            fprintf(stdout, "%c", ch);
        }
        fprintf(stdout, "\n");
        return 0;
    }

    for (size_t i = 0; i < size; i++) {
        ch = data[i];
        //fprintf(stderr, ">> ch         : %c\n", ch);
        if (_is_delim(ch, delim, delim_size)) {
            fprintf(stdout, "\n");
            continue;
        }
        if (config->ignore_ws && _is_witespace(ch)) {
            continue;
        }
        if (config->ignore_ln && _is_line(ch)) {
            continue;
        }
        fprintf(stdout, "%c", ch);
    }

    fprintf(stdout, "\n");
    return 0;
}

static int run_tok_file(run_tok_config_t* config, const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* data = NULL;
    size_t size = 0;
    int error = lib_io_read_all_bytes(file_name, &data, &size);
    if (error != 0) {

        // error
        if (data) {
            free(data);
        }
        //_file_error(file_name);
        return 1;
    }
    error = run_tok_data(config, data, size);
    if (data) {
        free(data);
    }
    return error;
}

void usage() {
    //fprintf(stderr, "Usage: run-tok [-l] [-ws] [-ln] [-d delim] [-f format] -s string | file\n");
    fprintf(stderr, "Usage: run-tok [-ws] [-ln] [-d delim] -s string | file\n");
}

int main(int argc, char* argv[]) {
    
    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;
    int long_ind;
    int min_arg = 1;

    bool flag_list        = false;
    bool flag_string      = false;
    bool flag_ignore_ws   = false;
    bool flag_ignore_ln   = false;
    const char* delim     = NULL;
    const char* data      = NULL;
    size_t size           = 0;
    const char* file_name = NULL;
    int format            = 0;

    static lib_option long_option[] = {
          //{"list",   no_argument,       0, 'l'},
          //{"format", required_argument, 0, 'f'},
          {"ws",     no_argument, 0, 1001},
          {"ln",     no_argument, 0, 1002},
          {NULL,     0,           0, 0 }
    };

    while ((opt = lib_getopt_long(argc, argv, "d:s:" /*"ld:f:s:"*/, long_option, &long_ind)) != -1) {
        
        switch (opt) {
        // case 'l':
        //     flag_list = true;
        //     break;
        case 1001:
            flag_ignore_ws = true;
            break;
        case 1002:
            flag_ignore_ln = true;
            break;
        case 'd':
            delim = optarg;
            break;
        // case 'f':
        //     format = _get_format(optarg);
        //     if (format <= 0) {
        //         fprintf(stderr, "%s: Incorrect format: %s\n", prog_name, optarg);
        //         error = 1;
        //     }
        //     break;
        case 's':
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

    error = 0;
    if (flag_string) {
        if (!data) {
            fprintf(stderr, "%s: Input string is required\n", prog_name);
            error = 1;
        } else {
            size = strlen(data);
            if (size == 0) {
                fprintf(stderr, "%s: Input string is empty\n", prog_name);
                error = 1;
            }
        }
    } else {
        if (optind >= argc) {
            fprintf(stderr, "%s: File name is required\n", prog_name);
            error = 1;
        } else {
            file_name = argv[optind];
        }
    }

    if (error) {
        usage();
        return 1;
    }

    if (!delim) {
        delim = RUN_TOK_DELIM;
    }

    run_tok_config_t config;

    config.format = format; 
    config.delim = delim;
    config.ignore_ws = flag_ignore_ws;
    config.ignore_ln = flag_ignore_ln;

    if (flag_string) {
        error = run_tok_data(&config, data, size);
    } else {
        error = run_tok_file(&config, file_name);
    }

    if (error != 0) {
        return 1;
    }
    
    return 0;
}