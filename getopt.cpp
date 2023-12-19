#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"

char* optinput;
int optind = 0;
int opterr = 1;

static char *input = NULL;
static int restopt = 0;

void getopt_init() {
    optinput = NULL;
    optind = 1;

    input = NULL;
    restopt = 0;
}

int getopt_internal(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind, int long_only) {

    if (argc <= 0 || argv == NULL /*|| optstr == NULL*/) {
        return -1;
    }

    // Fixed optind
    if (optind <= 0) {
        getopt_init();
    }

    if (optind >= argc) {
        return -1;
    }

    optopt = '?';
    optarg = NULL;

    input = argv[optind];
    optinput = input;

    if (input == NULL) {
        return -1;
    }

    int input_len = strlen(input);
    if (input_len == 0) {
        return -1;
    }

    int prefix_len = 0;

    if (input[0] == '-') {
        prefix_len = 1;
        if (input_len > 1 && input[1] == '-') {
            prefix_len++;
        }
    }

    if (prefix_len == input_len) {
        // '-'
        // '--'
        return -1;
    }

    int print_error = opterr;
    if (short_option != NULL) {
        if (short_option[0] == ':') {
            print_error = 0;                      // first ':'
        } else if (short_option[0] == '-' || short_option[0] == '+') {
            print_error = short_option[1] == ':'; // second '-:', '+:'
        }
    }

    if (prefix_len == 0) {
       // TODO: optarg
       return -1;
    }

    //////

    // Long option: '-abc'
    int multi_short = 1;

    //////

    char ch = input[restopt > 0 ? restopt : prefix_len]; // first char after '-' or '--'

    if (ch == '-') {
        // '---..'
        return -1;
    }

    optopt = ch; // by default

    int name_len = restopt > 0 ? 1 : input_len - prefix_len;
 
    //printf(">> optind %d, input %s, name_len %d, print_error %d\n", optind, input, name_len, print_error);

    if (name_len > 1) {

        // '-abc', '-abc=24'
        int start = prefix_len;
        int end = input_len;
        int eq_pos = -1;

        // search '='
        for (int i = start; i < end; i++) {
            if (input[i] == '=') {
                eq_pos = i;
            }
        }

        if (eq_pos == start || eq_pos == end - 1) {
            // '-=10', '-name=', -='
            if (print_error) {
                fprintf(stderr, "%s: illegal input %s\n", argv[0], input);
            }            
            optind++;
            return '?';
        }

        if (eq_pos > 0) {
            end = eq_pos;
        }

        if (multi_short && short_option != NULL) {

            // multi short option cannot be with '=': '-abc=10'
            if (eq_pos > 0) {
                if (print_error) {
                  fprintf(stderr, "%s: illegal multi option %s\n", argv[0], input);
                }            
                optind++;
                return '?';
            }

            // search option in short_option list
            int error = 0;
            for (int i = start; i < end; i++) {
                const char* opt = strchr(short_option, input[i]);
                if (opt == NULL) {
                    error = 1; // not found
                    break;
                }
                if (*(++opt) == ':') {
                    error = 2; // found, but require value. an option in 'multi_short' mode has not value
                }
            }

            if (error > 0) {
                if (print_error) {
                    if (error == 1) {
                        fprintf(stderr, "%s: illegal multi option %s\n", argv[0], input);
                    } else {
                        fprintf(stderr, "%s: incorrect multi option %s\n", argv[0], input);
                    }                  
                }            
                optind++;
                return '?';
            }
            restopt = start;

        } else {

            char* name = input + start;
            int len = end - start;

            const option* p = long_option;
            const option* pfound = NULL;
            int i = 0;

            // search option in long_option list
            //while ((p = long_option) != NULL && p->name != NULL) {
            while (p != NULL && p->name != NULL) {
                if (!strncmp(name, p->name, len)) {
                    pfound = p;
                    break;
                }
                //printf(">> option %d, %s\n", i, p->name);
                i++;
                p++;
            }

            if (pfound == NULL) {
                if (print_error) {
                    fprintf(stderr, "%s: illegal long option %s\n", argv[0], input);
                }            
                optind++;
                return '?';
            }

            *long_ind = i;

            if (p->has_arg == no_argument) {
                //printf("optarg - no arg\n");
                optind++;
                if (p->flag != NULL) {
                    *(p->flag) = p->val;
                }
                return p->val;
            }

            if (p->has_arg == required_argument || p->has_arg == optional_argument) {
                //printf("optarg - has arg\n");
                // Require arg
                char *next = optind >= argc ? NULL : argv[optind + 1];
                if (next == NULL || next[0] == '-') {
                    //if (print_error && p->has_arg == required_argument) {
                    //    fprintf(stderr, "%s: option requires an argument %s\n", argv[0], input);
                    //}
                    optind++;
                    if (p->has_arg == required_argument) {
                        if (print_error) {
                           fprintf(stderr, "%s: option requires an argument %s\n", argv[0], input);
                        }
                        return ':';
                    }

                    if (eq_pos > 0) {
                      //printf(">> optarg with = \n");
                      optarg = name + len + 1;                    
                    }

                    if (p->flag != NULL) {
                       *(p->flag) = p->val;
                    }
                    return p->val;
                }

                //printf(">> eq_pos %d\n", eq_pos);

                optind++; // arg
                if (eq_pos > 0) {
                    //printf(">> optarg with = \n");
                    optarg = name + len + 1;                    
                } else {
                    optarg = argv[optind];
                    optind++; // next
                }

                //printf(">> optarg %s\n", optarg);

                if (p->flag != NULL) {
                    *(p->flag) = p->val;
                }
                return p->val;
            }

            // TODO
            if (print_error) {
                fprintf(stderr, "%s: illegal long option %s\n", argv[0], input);
            }            
            optind++;
            return '?';

        }

    }

    if (restopt > 0) {
        optopt = input[restopt];
        //printf(">> restopt %d, optopt %c\n", restopt, optopt);
        if (restopt < input_len) {
            restopt++;
        } else {
            restopt = 0; // reset
            optind++;    // next
        }
        return optopt;
    }
    
    // Short option: '-a'
    const char* opt = short_option ? strchr(short_option, ch) : NULL;

    if (opt == NULL) {
        if (print_error) {
            fprintf(stderr, "%s: illegal option %s\n", argv[0], input);
        }
        optind++;
        return '?';
    }

    if (*(++opt) == ':') {

        // Require arg
        char *next = optind >= argc ? NULL : argv[optind + 1];

        if (next == NULL || next[0] == '-') {
            if (print_error) {
                fprintf(stderr, "%s: option requires an argument %s\n", argv[0], input);
            }
            optind++;
            return ':';
        }

        optind++; // arg
        optarg = argv[optind];
        optind++; // next
        return optopt;
    } else {
        optind++;
        return optopt;
    }

    // TODO
    return -1;


}

int getopt(int argc, char* const argv[], const char* short_option) {
   return getopt_internal(argc, argv, short_option, NULL, NULL, 0);
}

int getopt_long(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind) {
    return getopt_internal(argc, argv, short_option, long_option, long_ind, 0);
}

int getopt_long_only(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind) {
    return getopt_internal(argc, argv, short_option, long_option, long_ind, 1);
}