#ifndef PLAZMA_LIB_CLILIB_H
#define PLAZMA_LIB_CLILIB_H

#include <stdbool.h>
#include <string.h>

static const char* prog_name;


static void lib_cli_prog_init(char* const argv[]);

static const char* lib_cli_base_name(const char* name);

static const char* lib_cli_prog_name(char* const argv[]);

static void lib_cli_prog_init(char* const argv[]) {
    prog_name = lib_cli_prog_name(argv);
}

static const char* lib_cli_base_name(const char* name) {
    if (!name) {
        return NULL;
    }
    const char* base  = strrchr(name, '/');   /* NIX */
    const char* base2 = strrchr(name, '\\');  /* WIN */
    if (base2 > base) {
        base = base2;
    }
    base = base ? (base + 1): name;
    return base;
}

static const char* lib_cli_prog_name(char* const argv[]) {
    if (!argv) {
        return NULL;
    }
    return lib_cli_base_name(argv[0]);
}

/**
 * Return TRUE if argument index is valid (inside)
 */
static bool lib_cli_in_argind(int argc, int ind) {
    return ind < argc;
}

/**
 * Return TRUE if argument index is not valid (outside)
 */
static bool lib_cli_not_argind(int argc, int ind) {
    // ind >= argc;
    return !lib_cli_in_argind(argc, ind);
}

/**
 * Return TRUE if first argument index is not valid (outside)
 */
static bool lib_cli_not_arg(int argc) {
    return !lib_cli_in_argind(argc, 1);
}

static bool lib_cli_not_argmin(int argc, int ind, int min) {
    return argc - ind < min;
}

#endif // PLAZMA_LIB_CLILIB_H