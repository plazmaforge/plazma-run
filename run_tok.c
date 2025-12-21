#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"

#define RUN_TOK_DELIM " ,.:;"

typedef struct run_tok_config_t {
    int format;
    const char* delim;
    size_t delim_size;
    const char* delim_str;  // Not implemented yet
    size_t delim_str_size;  // Not implemented yet
    bool skip_ws;
    bool skip_ln;
    bool ignore_delim;
} run_tok_config_t;

////

static int _get_format(const char* name) {
    if (!name) {
        return -1;
    }
    //  // Not implemented yet
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

static bool _is_skip(run_tok_config_t* config, char ch) {
    if (config->skip_ws && _is_witespace(ch)) {
        return true;
    }
    if (config->skip_ln && _is_line(ch)) {
        return true;
    }
    return false;
}

int run_tok_data(run_tok_config_t* config, const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    if (size == 0) {
        return 0;
    }

    int format = config->format; 
    const char* delim = config->delim;
    size_t delim_size = config->delim_size;

    //fprintf(stderr, ">> delim      : %s\n", delim);
    //fprintf(stderr, ">> delim_size : %lu\n", delim_size);
    //fprintf(stderr, ">> data       : %s\n", data);
    //fprintf(stderr, ">> data_size  : %lu\n", size);

    char ch = 0;
    if (delim_size == 0) {
        for (size_t i = 0; i < size; i++) {
            ch = data[i];
            if (_is_skip(config, ch)) {
                continue;
            }
            fprintf(stdout, "%c", ch);
        }
        fprintf(stdout, "\n");
        return 0;
    }

    bool token_break   = false;
    size_t token_count = 0;

    for (size_t i = 0; i < size; i++) {
        ch = data[i];
        //fprintf(stderr, ">> ch         : %c\n", ch);

        if (_is_skip(config, ch)) {
            continue;
        }
        
        if (_is_delim(ch, delim, delim_size)) {
            token_break = true;            // SET TOKEN BREAK
            if (!config->ignore_delim) {
                if (token_count > 0) {
                    fprintf(stdout, "\n"); // PRINT TOKEN BREAK
                }
                fprintf(stdout, "%c", ch);
                token_count++;
            }            
            continue;
        }
        
        if (token_break) {
            token_break = false;       // RESET TOKEN BREAK
            if (token_count > 0) {
                fprintf(stdout, "\n"); // PRINT TOKEN BREAK
            }
        }
        token_count++;
        
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
        fprintf(stderr, "%s: %s: No such file or directory\n", prog_name, file_name);
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
    
    lib_cli_prog_init(argv);

    int error = 0;
    int opt;
    int long_ind;
    int min_arg = 1;

    bool flag_string       = false;
    bool flag_skip_ws      = false;
    bool flag_skip_ln      = false;
    bool flag_ignore_delim = true; // It is not skip: output line separator
    const char* delim      = NULL;
    const char* delim_str  = NULL; // Not implemented yet
    const char* data       = NULL;
    size_t data_size       = 0;
    const char* file_name  = NULL;
    int format             = 0;    // Not implemented yet

    static lib_option long_option[] = {
          //{"format",     required_argument, 0, 'f'},
          {"ws",           no_argument,       0, 1001},
          {"ln",           no_argument,       0, 1002},
          //{"ds",           required_argument, 0, 1003},
          {"ignore-delim", optional_argument,  0, 1004},
          {NULL,     0,                 0, 0 }
    };

    while ((opt = lib_getopt_long(argc, argv, "d:s:" /*"ld:f:s:"*/, long_option, &long_ind)) != -1) {
        
        switch (opt) {
        // case 'f':
        //     format = _get_format(optarg);
        //     if (format <= 0) {
        //         fprintf(stderr, "%s: Incorrect format: %s\n", prog_name, optarg);
        //         error = 1;
        //     }
        //     break;
        case 1001:
            flag_skip_ws = true;
            break;
        case 1002:
            flag_skip_ln = true;
            break;
        case 'd':
            delim = optarg;
            break;
        //case 1003:
        //    delim_str = optarg;
        //    break;
        case 1004:           
           fprintf(stderr, ">> optarg      : %s\n", optarg);
           //fprintf(stderr, ">> strcmp      : %s\n", strcmp(optarg, "false"));
           if (optarg) {
               flag_ignore_delim = optarg ? !(strcmp(optarg, "false") == 0) : true;
               if (strcmp(optarg, "false") == 0) {
                   flag_ignore_delim = false;
               } else if (strcmp(optarg, "true") == 0) {
                   flag_ignore_delim = true;
               } else {
                  fprintf(stderr, "%s: Incorrect value of 'ignore-delim'. Value must be 'true' or 'false'\n", prog_name);
                  error = 1;
               }               
           } else {
               flag_ignore_delim = true;
           }
           fprintf(stderr, ">> ignore-delim: %s\n", (flag_ignore_delim ? "true" : "false"));
           break;
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
            data_size = strlen(data);
            if (data_size == 0) {
                fprintf(stderr, "%s: Input string is empty\n", prog_name);
                error = 1;
            }
        }
    } else {
        //if (optind >= argc) {
        if (lib_cli_not_argind(argc, optind)) {    
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

    config.format         = format; // Not implemented yet 
    config.delim          = delim;
    config.delim_size     = delim ? strlen(delim) : 0;
    config.delim_str      = NULL;   // Not implemented yet
    config.delim_str_size = 0;      // Not implemented yet
    config.skip_ws        = flag_skip_ws;
    config.skip_ln        = flag_skip_ln;
    config.ignore_delim   = flag_ignore_delim;

    if (flag_string) {
        error = run_tok_data(&config, data, data_size);
    } else {
        error = run_tok_file(&config, file_name);
    }
    
    return error == 0 ? 0 : 1;
}