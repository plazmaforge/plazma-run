//#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"

char* optinput;
//int optind = 0;
//int opterr = 1;

static char *input = NULL;
static int restopt = 0;
static int init = 0;

const char* find_short_option(const char* short_option, char c) {
    if (short_option == NULL) {
        return NULL;
    }
    return strchr(short_option, c);
}

const struct option* find_long_option(const struct option* long_option, int* long_index, const char* name, int len)  {
    const struct option* p = long_option;
    int i = *long_index;

    // search option in long_option list
    while (p != NULL && p->name != NULL) {
        if (!strncmp(name, p->name, len)) {
            *long_index = i;
            return p;
        }
        //printf(">> option %d, %s\n", i, p->name);
        i++;
        p++;
    }
    return NULL;
}

int check_next_value(int argc, char* const argv[], int optind) {
    if (optind + 1 >= argc) {
        return 0;
    }
    char *next = argv[optind + 1];
    //printf(">> argc: %d, oprind %d, next: '%s'\n", argc, optind, next);
    return next[0] != '-';
}

int check_multi_short_option(const char* short_option, const char* input, int start, int end) {
    const char* opt;
    for (int i = start; i < end; i++) {
        opt = strchr(short_option, input[i]);
        if (opt == NULL) {
            return 1;  // not found
        }
        if (*(++opt) == ':') {
            return 2; // found, but require value. an option in 'multi_short' mode has not value
        }
    }
    return 0;
}

void getopt_init() {    
    optinput = NULL;

    optind = 1;
    optopt = 0; //NULL;
    optarg = NULL;
    opterr = 1; // by default: print option error flag = true (for all options)

    input = NULL;
    restopt = 0;
}

int getopt_internal(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind, int long_only) {

    if (argc <= 0 || argv == NULL /*|| optstr == NULL*/) {
        return -1;
    }

    //printf(">> optind: %d\n", optind);
    //printf(">> opterr: %d\n", opterr);

    // init
    if (!init) {
        getopt_init();
        init = 1;
    }

    //printf(">> optind: %d\n", optind);
    //printf(">> opterr: %d\n", opterr);

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

    // Correct print error flag by short option string
    int print_error = opterr;
    if (short_option != NULL) {
        if (short_option[0] == ':') {
            print_error = 0;                      // first ':'
        } else if (short_option[0] == '-' || short_option[0] == '+') {
            print_error = short_option[1] == ':'; // second '-:', '+:'
        }
    }

    if (prefix_len == 0) {
       // optarg
       return -1;
    }

    //////

    // Multi short option: '-abc' -> '-a -b -c'
    int multi_short = long_option == NULL ? 1 : 0;
    //printf("multi_short: %d\n", multi_short);

    //////

    char ch = input[restopt > 0 ? restopt : prefix_len]; // first char after '-' or '--'

    if (ch == '-') {
        // '---..'
        return -1;
    }

    optopt = ch; // by default

    int str_len = restopt > 0 ? 1 : input_len - prefix_len;
 
    //printf(">> optind: %d, input: '%s', input_len %d, str_len %d, print_error %d\n", optind, input, input_len, str_len, print_error);

    if (str_len > 1) {

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
                fprintf(stderr, "%s: Illegal input %s\n", argv[0], input);
            }            
            optind++;
            return '?';
        }

        if (eq_pos > 0) {
            end = eq_pos;
        }

        char* name = input + start;
        int len = end - start;

        if (multi_short && short_option != NULL && prefix_len == 1) {

            // multi short option cannot be with '=': '-abc=10'
            int error = 0;
            if (eq_pos > 0) {

                if (eq_pos - start == 1) {
                    // '-a=10
                    error = check_multi_short_option(short_option, input, start, end);
                    if (error > 0) {
                        if (print_error) {
                            if (error == 1) {
                                fprintf(stderr, "%s: Illegal multi option %s\n", argv[0], input);
                            } else {
                                fprintf(stderr, "%s: Incorrect multi option %s\n", argv[0], input);
                            }
                        }
                        optind++;
                        return '?';
                    }
                    optarg = name + len + 1;
                    optind++;
                    return optopt;
                }

                if (print_error) {
                  fprintf(stderr, "%s: Illegal multi option %s\n", argv[0], input);
                }            
                optind++;
                return '?';
            }

            // search option in short_option list
            error = check_multi_short_option(short_option, input, start, end);

            if (error > 0) {
                if (print_error) {
                    if (error == 1) {
                        fprintf(stderr, "%s: Illegal multi option %s\n", argv[0], input);
                    } else {
                        fprintf(stderr, "%s: Incorrect multi option %s\n", argv[0], input);
                    }                  
                }            
                optind++;
                return '?';
            }
            restopt = start;

        } else {

            int index = 0;
            const struct option* p = find_long_option(long_option, &index, name, len);

            if (p == NULL) {
                if (print_error) {
                    fprintf(stderr, "%s: Illegal long option %s\n", argv[0], input);
                }            
                optind++;
                return '?';
            }

            *long_ind = index;

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

                // optarg by '='
                if (eq_pos > 0) {
                    optarg = name + len + 1;
                    optind++;
                    if (p->flag != NULL) {
                        *(p->flag) = p->val;
                    }
                    return p->val;
                }

                // otparg by next 'argv'                    
                if (!check_next_value(argc, argv, optind)) {

                    if (p->has_arg == required_argument) {
                        if (print_error) {
                           fprintf(stderr, "%s: Option requires an argument %s\n", argv[0], input);
                        }
                        optind++;
                        return ':';
                    }

                    optind++;
                    if (p->flag != NULL) {
                       *(p->flag) = p->val;
                    }
                    return p->val;
                }

                optind++;
                optarg = argv[optind];
                optind++; // next

                if (p->flag != NULL) {
                    *(p->flag) = p->val;
                }
                return p->val;
            }

            // TODO: Incorrect p->has_arg. It must be: [0,1,2]
            if (print_error) {
                fprintf(stderr, "%s: Illegal long option %s\n", argv[0], input);
            }            
            optind++;
            return '?';

        }

    }

    if (restopt > 0) {
        optopt = input[restopt];
        //printf(">> restopt: %d, optopt: '%c'\n", restopt, optopt);
        if (restopt < input_len) {
            restopt++;
        } else {
            restopt = 0; // reset
            optind++;    // next
        }
        return optopt;
    }
    
    // Short option: '-a'
    const char* opt = find_short_option(short_option, ch);
    //printf(">> short_option: '%s', ch: '%c', opt: '%s'\n", short_option, ch, opt);

    if (opt == NULL) {
        if (print_error) {
            fprintf(stderr, "%s: Illegal option %s\n", argv[0], input);
        }
        optind++;
        return '?';
    }

    if (*(++opt) == ':') {

        // optional arg
        if (*(++opt) == ':') {
            // '::'
            optind++;
            return optopt;
        }

        // required arg
        if (!check_next_value(argc, argv, optind)) {
            if (print_error) {
                fprintf(stderr, "%s: Option requires an argument %s\n", argv[0], input);
            }
            optind++;
            return ':';
        }

        optind++; // arg
        optarg = argv[optind];
        optind++; // next
        return optopt;
    } 
    
    optind++;
    return optopt;

}

int getopt(int argc, char* const argv[], const char* short_option) {
   return getopt_internal(argc, argv, short_option, NULL, NULL, 0);
}

int getopt_long(int argc, char* const argv[], const char* short_option, const option* long_option, int* long_ind) {
    return getopt_internal(argc, argv, short_option, long_option, long_ind, 0);
}

int getopt_long_only(int argc, char* const argv[], const char* short_option, const struct option* long_option, int* long_ind) {
    return getopt_internal(argc, argv, short_option, long_option, long_ind, 1);
}
