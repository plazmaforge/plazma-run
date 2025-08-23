#include <stdio.h>
//#include <ctype.h>
#include <errno.h>

//#ifdef _WIN32
//#include <fcntl.h>
//#include <io.h>
//#endif

#include "getopt.h"
//#include "clilib.h"
#include "iolib.h"

#include "run_enclib.h"

// static void _file_error(const char* file_name) {
//     fprintf(stderr, "%s: %s: %s\n", prog_name, file_name, strerror(errno));
// }

// static void _encode_error(const char* message) {
//     fprintf(stderr, "%s: Encode error: %s\n", prog_name, (message ? message : "Unknown"));
// }

// static void _decode_error(const char* message) {
//     fprintf(stderr, "%s: Decode error: %s\n", prog_name, (message ? message : "Unknown"));
// }

////

int run_enc(int argc, char* argv[]) {

    //prog_name = lib_cli_prog_name(argv);
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
        //usage();
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
            //algo = _get_algo(optarg);
            if (algo <= 0) {
                //fprintf(stderr, "%s: Unsupported algorithm: %s\n", prog_name, optarg);
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
        if (algo <= 0) {
            //fprintf(stderr, "%s: Algorithm is required\n", prog_name);
            error = 1;
        }
    }

    if (error) {
        //usage();
        return 1;
    }

    if (flag_list) {
        //_list_algo();
        return 0;
    }

    //printf("argc  : %i\n", argc);
    //printf("optind: %i\n", optind);

    #ifdef _WIN32
    int result = _setmode(_fileno(stdout), _O_BINARY);
    if (result == -1) {
        //fprintf(stderr, "%s: %s\n", prog_name, "Cannot set binary mode in stdout");
    }
    #endif

    if (flag_string) {
        if (!data) {
            //fprintf(stderr, "%s: %s\n", prog_name, "Input string is required\n");
            return 1;
        }
        if (flag_decode) {
            //return run_decode_data(algo, data, strlen(data));
        } else {
            //return run_encode_data(algo, data, strlen(data));
        }        
    } else {
        if (optind >= argc) {
            //fprintf(stderr, "%s: %s\n", prog_name, "File name is required");
            //usage();
            return 1;
        }
        char* file_name = argv[optind];
        //fprintf(stderr, "file: %s\n", file_name);
        if (flag_decode) {
            //return run_decode_file(algo, file_name);
        } else {
            //return run_encode_file(algo, file_name);
        }        
    }
    return 0;
}