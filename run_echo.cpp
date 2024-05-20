
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
// #include "strlib.h"
// #include "wstrlib.h"
#include "syslib.h"

void usage() {
    printf("Usage: run-echo [string ...]\n");
}

void print_args(char* args[], int count, int start, bool trailing) {
    for (int i = start; i < count; i++) {
        if (i > start) {
            printf(" ");
        }
        printf("%s", args[i]);
    }
    if (trailing) {
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }

    init_locale();

    print_args(argv, argc, 1, true);

    restore_locale();
    
    return 0;
}