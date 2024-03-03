
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
// #include "strlib.h"
// #include "wstrlib.h"
#include "syslib.h"

void usage() {
    printf("Usage: run-echo <text>\n");
}

void print_input(const char* input) {
    if (!input) {
        return;
    }
    printf("%s\n", input);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }

    init_locale();

    print_input(argv[1]);

    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}