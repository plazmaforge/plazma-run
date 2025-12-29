#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "run_dclib.h"

void usage() {
    fprintf(stderr, "Usage: run-oct2dec value\n");
}

int main(int argc, char* argv[]) {

    lib_cli_prog_init(argv);

    if (lib_cli_not_arg(argc)) {
        usage();
        return 1;
    }

    const char* str = argv[1];
    uint64_t num = 0;
    int error = run_str2num(str, 8, &num);
    if (error != 0) {
        return 1;        
    }

    fprintf(stdout, "%llu\n", num);
    return 0;
}
