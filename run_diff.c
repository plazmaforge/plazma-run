#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "getopt.h"
#include "clilib.h"

static int _line_cmp(char* line1, size_t len1, char* line2, size_t len2) {
    if (!line1 && !line2) {
        fprintf(stderr, "line1, line2 are NULL\n");
        return 0;
    }
    if (len1 != len2) {
        fprintf(stderr, "line1: %lu, line2: %lu\n", len1, len2);
        return 1;
    }
    size_t len = len1;
    char* s1 = line1;
    char* s2 = line2;
    for (size_t i = 0; i < len; i++) {
        if (*s1++ != *s2++) {
            // /fprintf(stderr, "line1: %lu, line2: %lu\n", line1, line2);
            return 1;
        }
    }
    return  0;
} 

#ifdef _WIN32
#define GETLINE_MINSIZE 16
int _getline_win(char** line, size_t* n, FILE* file) {
    int ch;
    int i = 0;
    char free_on_err = 0;
    char *p;

    errno = 0;
    if (line == NULL || n == NULL || file == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (*line == NULL) {
        *n = GETLINE_MINSIZE;
        *line = (char*) malloc( sizeof(char) * (*n));
        if (*line == NULL) {
            errno = ENOMEM;
            return -1;
        }
        free_on_err = 1;
    }

    for (i = 0; ; i++) {
        ch = fgetc(file);
        while (i >= (*n) - 2) {
            *n *= 2;
            p = realloc(*line, sizeof(char) * (*n));
            if (p == NULL) {
                if (free_on_err)
                    free(*line);
                errno = ENOMEM;
                return -1;
            }
            *line = p;
        }
        if (ch == EOF) {
            if (i == 0) {
                if (free_on_err)
                     free(*line);
                return -1;
            }
            (*line)[i] = '\0';
            *n = i;
            return i;
        }

        if (ch == '\n') {
            (*line)[i] = '\n';
            (*line)[i+1] = '\0';
            *n = i+1;
            return i+1;
        }
        (*line)[i] = (char)ch;
    }
}
#else
ssize_t _getline_nix(char** line, size_t* cap, FILE* file) {
    return getline(line, cap, file);
}
#endif

ssize_t _getline(char** line, size_t* cap, FILE* file) {
    #ifdef _WIN32
    return _getline_win(line, cap, file);
    #else
    return _getline_nix(line, cap, file);
    #endif
}

int run_diff(const char* file_name1, const char* file_name2) {

    if (!file_name1 && !file_name2) {
        fprintf(stderr, "%s: File name1 and name2 are empty\n", prog_name);
        return 1;
    }

    if (!file_name1) {
        fprintf(stderr, "%s: File name1 is empty\n", prog_name);
        return 1;
    }

    if (!file_name2) {
        fprintf(stderr, "%s: File name2 is empty\n", prog_name);
        return 1;
    }

    FILE* file1 = fopen(file_name1, "r");

    if (!file1) {
        fprintf(stderr, "%s: %s: Unable to open file1\n", prog_name, file_name1);
        return 1;
    }

    FILE* file2 = fopen(file_name2, "r");

    if (!file2) {
        fprintf(stderr, "%s: %s: Unable to open file2\n", prog_name, file_name2);
        fclose(file1);
        return 1;
    }

    char* line1   = NULL;
    size_t cap1   = 0;
    size_t len1   = 0;
    ssize_t read1 = 0;

    char* line2   = NULL;
    size_t cap2   = 0;
    size_t len2   = 0;
    ssize_t read2 = 0;

    while ( (read1 = _getline(&line1, &cap1, file1)) != -1 ) {
        //fprintf(stderr, "Retrived line of length: %lu, %lu \n", read1, len1);
        //fprintf(stderr, "%s \n", line1);

        read2 = _getline(&line2, &cap2, file2);

        if (read2 == -1) {
            fprintf(stdout, "\n>>: %s\n", line1);
            break;
        }

        len1 = read1;
        len2 = read2;

        if (_line_cmp(line1, len1, line2, len2) != 0) {
            fprintf(stdout, "\n>>: %s\n", line1);
            fprintf(stdout, "\n<<: %s\n", line2);
        }
    }

    fclose(file1);
    fclose(file2);

    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-diff file1 file2\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 2; // <file1> <file2>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;

    // while ((opt = lib_getopt(argc, argv, "f:")) != -1) {
    //     switch (opt) {
    //     case 'f':
    //         break;
    //     case '?':
    //         error = 1;
    //         break;
    //     case ':':
    //         error = 1;
    //         break;
    //     }
    // }

    // if (error) {
    //     usage();
    //     return 0;
    // }

    // if (argc - optind < min_arg) {
    //     fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
    //     usage();
    //     return 0;
    // }

    char* file_name1 = argv[1];
    char* file_name2 = argv[2];

    return run_diff(file_name1, file_name2);

}
