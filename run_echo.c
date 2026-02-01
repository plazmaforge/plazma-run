#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//#include <iostream.h>
//#include <windows.h>
//#include <io.h>
//#include <fcntl.h>

#include "getopt.h"
#include "clilib.h"
#include "syslib.h"

#include "iolib.h"

typedef struct run_echo_context_t {
    int argc;
    char** argv;
    int start; 
    bool trailing;
    const char* file_name;
    FILE* out;
} run_echo_context_t;

int run_echo(run_echo_context_t* ctx) {
    for (int i = ctx->start; i < ctx->argc; i++) {
        if (i > ctx->start) {
            fprintf(ctx->out, " ");
        }
        fprintf(ctx->out, "%s", ctx->argv[i]);
    }
    if (ctx->trailing) {
        fprintf(ctx->out, "\n");
    }
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-echo [-n] [-o <output file>] <string> ...\n");
}

int main(int argc, char* argv[]) {
   
    //SetConsoleOutputCP(CP_UTF8);
    //SetConsoleCP(CP_UTF8);

    //int prev_in = _setmode(_fileno(stdin), _O_U8TEXT);
    //int prev_out = _setmode(_fileno(stdout), _O_U8TEXT);

    //fprintf(stderr, "Prev input mode: %d\n", prev_in);
    //fprintf(stdout, "Prev output mode: \n");

    int min_arg = 1; // <string>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_cli_prog_name(argv);
    const char* file_name = NULL;
    int error = 0;
    int opt;

    bool trailing = true; // '\n'
    while ((opt = lib_getopt(argc, argv, "no:")) != -1) {
        switch (opt) {
        case 'n':
            trailing = false;
            break;
        case 'o':
            file_name = optarg;
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

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 1;
    }

    //lib_sys_locale_init_utf8();

    run_echo_context_t ctx;
    ctx.argc = argc;
    ctx.argv = argv;
    ctx.start = optind;
    ctx.trailing = trailing;
    error = 0;

    if (file_name) {
        ctx.file_name = file_name;
        ctx.out = lib_io_fopen(file_name, "wb+");
        if (!ctx.out) {
            fprintf(stderr, "%s: Cannot open file: %s\n", prog_name, file_name);
            error = 1;
        }
    } else {
        ctx.file_name = NULL;
        ctx.out = stdout;
    }

    if (error) {
        return 1;
    }
    
    error = run_echo(&ctx);

    if (ctx.file_name) {
        lib_io_fclose(ctx.out);
    }

    if (error) {
        return 1;
    }

    //lib_sys_locale_restore();

    return 0;
}