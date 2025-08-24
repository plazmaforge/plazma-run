#include <stdio.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"

#include "run_edlib.h"

static const char* _to_message(const char* message) {
    return (message ? message : "Unknown");
}

static void _error(const char* title, const char* message) {
    if (title) {
        fprintf(stderr, "%s: %s: %s\n", prog_name, title, _to_message(message));        
    } else {
        fprintf(stderr, "%s: %s\n", prog_name, _to_message(message));        
    }
}

static void _error_message(const char* message) {
    _error(NULL, message);
}

static void _file_error(const char* file_name) {
    _error(file_name, strerror(errno));
}

static void _encode_error(const char* message) {
    _error("Encode error", message);
}

static void _decode_error(const char* message) {
    _error("Decode error", message);
}

////

// static int _encode_algo(lib_ed_config_t* config, int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
//     return -1;
// }

// static int _decode_algo(lib_ed_config_t* config, int algo, const char* idata, size_t isize, char** odata, size_t* osize) {
//     return -1;
// }

// static int _get_algo(lib_ed_config_t* config, char* name) {
//     if (!name)  {
//         return -1;
//     }
//     return -1;
// }

static void _list_algo(lib_ed_config_t* config) {
    if (!config || !config->list_func) {
        return;
    }
    config->list_func();
}

////

static int run_encode_data(lib_ed_config_t* config, int algo, const char* idata, size_t isize) {
    if (!idata) {
        return 1;
    }

    char* odata = NULL;
    size_t osize = 0;

    int error = config->encode_func(algo, idata, isize, &odata, &osize);
    //int error = _encode_algo(algo, idata, isize, &odata, &osize);    
    if (error != 0) {
        if (odata) {
            free(odata);
        }
        _encode_error(NULL);
        return 1;
    }

    if (!odata) {
        _encode_error("Output data is empty");
        return 1;
    }

    for (size_t i = 0; i < osize; i++) {
        fprintf(stdout, "%c", odata[i]);
    }
    fprintf(stdout, "\n");
    free(odata);
    return 0;
}

static int run_decode_data(lib_ed_config_t* config, int algo, const char* idata, size_t isize) {
    if (!idata) {
        return 1;
    }
    char* odata = NULL;
    size_t osize = 0;

    int error = config->decode_func(algo, idata, isize, &odata, &osize);
    // _decode_algo(algo, idata, isize, &odata, &osize);
    if (error != 0) {
        if (odata) {
            free(odata);
        }
        _decode_error("Invalid character in input stream");
        return 1;
    }

    if (!odata) {
        _decode_error("Output data is empty");
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

static int run_encode_file(lib_ed_config_t* config, int algo, const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* idata = NULL;
    size_t isize = 0;
    int error = lib_io_read_all_bytes(file_name, &idata, &isize);
    if (error != 0) {

        // error
        if (idata) {
            free(idata);
        }
        _file_error(file_name);
        return 1;
    }
    error = run_encode_data(config, algo, idata, isize);
    if (idata) {
        free(idata);
    }
    return error;
}

static int run_decode_file(lib_ed_config_t* config, int algo, const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* idata = NULL;
    size_t isize = 0;
    int error = lib_io_read_all_bytes(file_name, &idata, &isize);
    if (error != 0) {

        // error
        if (idata) {
            free(idata);
        }
        _file_error(file_name);
        return 1;
    }
    error = run_decode_data(config, algo, idata, isize);
    if (idata) {
        free(idata);
    }
    return error;
}

int _lib_stricmp(const char* str1, const char* str2) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }

  char* s1 = (char*) str1;
  char* s2 = (char*) str2;
  int c1   = 0;
  int c2   = 0;

  while (true) {
    if (*s1 == '\0') {
      return (*s2 == '\0') ? 0 : -1;
    }
    if (*s2 == '\0') {
      return (*s1 == '\0') ? 0 : 1;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
    s1++;
    s2++;
  }
  return 0;
}

static void usage() {
    fprintf(stderr, "Usage: run-encode [-Dl] -a algo -s string | file\n");
}

int run_ed(lib_ed_config_t* config, int argc, char* argv[]) {

    prog_name = lib_cli_prog_name(argv);
    
    int error = 0;
    int opt;
    int long_ind;
    int min_arg = 1;

    bool flag_decode = false;
    bool flag_list   = false;
    bool flag_string = false;
    char* data       = NULL;
    int algo         = 0;

    if (argc < min_arg + 1) {
        //fprintf(stderr, "%s: Minimum required arguments: %d\n", prog_name, min_arg);
        usage();
        return 1;
    }

    static lib_option long_options[] = {
          {"list",    no_argument,       0, 'l'},
          {"algo",    required_argument, 0, 'a'},          
          {NULL,      0,                 0, 0}
    };

    while ((opt = lib_getopt_long(argc, argv, "Dla:s:", long_options, &long_ind)) != -1) {
        switch (opt) {
        case 'D':
            /* Set decode flag */
            flag_decode = true;
            //fprintf(stderr, "%s: Flag 'D'\n", prog_name);
            break;
        case 'l':
            /* Set list mode */
            flag_list = true;
            //fprintf(stderr, "%s: Flag 'l'\n", prog_name);
            break;
        case 'a':
            /* Set algorithm */
            //fprintf(stderr, "%s: Flag 'a': %s\n", prog_name, optarg);
            algo = config->get_func(optarg); // _get_algo(optarg);
            if (algo <= 0) {
                _error("Unsupported algorithm", optarg);
                error = 1;
            }
            break;
        case 's':
            /* Set mode by string */
            //fprintf(stderr, "%s: Flag 's': %s\n", prog_name, optarg);
            flag_string = true;
            data = optarg;
            break;
        case '?':
            //fprintf(stderr, "%s: Error '?'\n", prog_name);
            error = 1;    
            break;
        case ':':
            //fprintf(stderr, "%s: Error ':'\n", prog_name);
            error = 1;
            break;
        }
    }

    if (!flag_list) {
        if (algo == 0) {
            _error(NULL, "Algorithm is required");
            error = 1;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    if (flag_list) {
        _list_algo(config);
        return 0;
    }

    //printf("argc  : %i\n", argc);
    //printf("optind: %i\n", optind);

    #ifdef _WIN32
    int result = _setmode(_fileno(stdout), _O_BINARY);
    if (result == -1) {
        _error(NULL, "Cannot set binary mode in stdout");
    }
    #endif

    if (flag_string) {
        if (!data) {
            _error(NULL, "Input string is required");
            return 1;
        }
        if (flag_decode) {
            return run_decode_data(config, algo, data, strlen(data));
        } else {
            return run_encode_data(config, algo, data, strlen(data));
        }        
    } else {
        if (optind >= argc) {
            _error(NULL, "File name is required");
            usage();
            return 1;
        }
        char* file_name = argv[optind];
        //fprintf(stderr, "file: %s\n", file_name);
        if (flag_decode) {
            return run_decode_file(config, algo, file_name);
        } else {
            return run_encode_file(config, algo, file_name);
        }        
    }
    return 0;
}   
 