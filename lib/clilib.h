#ifndef PLAZMA_LIB_CLILIB_H
#define PLAZMA_LIB_CLILIB_H

#include <string.h>

static const char* prog_name;

const char* lib_cli_base_name(const char* name);

const char* lib_cli_prog_name(char* const argv[]);

const char* lib_cli_base_name(const char* name) {
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

const char* lib_cli_prog_name(char* const argv[]) {
    if (!argv) {
        return NULL;
    }
    return lib_cli_base_name(argv[0]);
}

#endif // PLAZMA_LIB_CLILIB_H