#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
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

int run_base32_encode_data(int algo, const char* data, size_t size) {
    if (!data) {
        return 1;
    }

    char* odata = NULL;
    size_t osize = 0;

    int retval = lib_base32_encode_type(algo, data, size, &odata, &osize);
    if (retval != 0) {
        free(odata);
        _encode_error();
        return 1;
    }

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

int run_base32_decode_data(int algo, const char* data, size_t size) {
    if (!data) {
        return 1;
    }
    char* odata = NULL;
    size_t osize = 0;

    int retval = lib_base32_decode_type(algo, data, size, &odata, &osize);
    if (retval != 0) {
        free(odata);
        _decode_error();
        return 1;
    }

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

int run_base32_encode_file(int algo, const char* file_name) {
    if (!file_name) {
        return 1;
    }

    char* data = NULL;
    size_t size = 0;
    int retval = lib_io_read_all_bytes(file_name, &data, &size);
    if (retval != 0) {

        // error
        if (data) {
            free(data);
        }
        _file_error(file_name);
        return 1;
    }
    retval = run_base32_encode_data(algo, data, size);
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
    if (retval != 0) {

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

static int _lib_stricmp(const char* str1, const char* str2) {
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

static int _get_algo(char* name) {
    if (!name)  {
        return 0;
    }

    if (_lib_stricmp(name, "base32") == 0) {
        return LIB_BASE32;
    } else if (_lib_stricmp(name, "base32u") == 0) {
        return LIB_BASE32U;
    } else if (_lib_stricmp(name, "base32l") == 0) {
        return LIB_BASE32L;
    } else if (_lib_stricmp(name, "zbase32") == 0) {
        return LIB_ZBASE32;
    } else if (_lib_stricmp(name, "base32hex") == 0) {
        return LIB_BASE32HEX;
    } else if (_lib_stricmp(name, "cbase32") == 0) {
        return LIB_CBASE32;
    }
    return 0;
}

static void _list_algo() {
    fprintf(stderr, "base32\n");
    fprintf(stderr, "base32u\n");
    fprintf(stderr, "base32l\n");
    fprintf(stderr, "zbase32\n");
    fprintf(stderr, "base32hex\n");
    fprintf(stderr, "cbase32\n");
}

void usage() {
    fprintf(stderr, "Usage: run-base32 [-Dl] -a algo -s string | file\n");
}

int main(int argc, char* argv[]) {

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;
    int long_ind;

    bool flag_decode = false;
    bool flag_list   = false;
    bool flag_string = false;
    char* data       = NULL;
    int algo         = LIB_BASE32; // by default

    if (argc < 2) {
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
            break;
        case 'l':
            /* Set list mode */
            flag_list = true;
            //fprintf(stderr, "%s: Flag 'l'\n", prog_name);
            break;
        case 'a':
            /* Set algorithm */
            //fprintf(stderr, "%s: Flag 'a': %s\n", prog_name, optarg);
            algo = _get_algo(optarg);
            if (algo <= 0) {
                fprintf(stderr, "%s: Unsupported algorithm: %s\n", prog_name, optarg);
                error = 1;
            }
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

    if (flag_list) {
        _list_algo();
        return 0;
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
        size_t size = strlen(data);
        if (size == 0) {
            fprintf(stderr, "%s: %s\n", prog_name, "Empty input string");
            return 1;
        }
        if (flag_decode) {
            return run_base32_decode_data(algo, data, strlen(data));
        } else {
            return run_base32_encode_data(algo, data, strlen(data));
        }        
    } else {
        if (optind >= argc) {
            fprintf(stderr, "%s: File name is required\n", prog_name);
            usage();
            return 1;
        }
        char* file_name = argv[optind];
        //fprintf(stderr, "file: %s\n", file_name);
        if (flag_decode) {
            return run_base32_decode_file(algo, file_name);
        } else {
            return run_base32_encode_file(algo, file_name);
        }        
    }
    return 0;
}   
 