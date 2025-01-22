#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"

static bool _lib_stremp(char* str) {
    return (!str || *str == '\0');
}

static char* _lib_strrdel(char* str, char* sub) {
    if (!str) {
        return NULL;
    }
    if (!sub) {
        return str;
    }
    size_t total = strlen(str);
    if (total == 0) {
        return str;
    }
    size_t len = strlen(sub);
    if (len == 0 || len > total) {
        return str;
    }
    size_t offset = total - len;
    for (size_t i = 0; i < len; i++) {
        if (str[i + offset] != sub[i]) {
            return str;
        }
    }
    str[offset] = '\0'; // truncate string
    return str;
}

static char* _lib_base_name0(char* name) {
    if (!name) {
        return NULL;
    }
    char* base  = strrchr(name, '/');   /* NIX */
    char* base2 = strrchr(name, '\\');  /* WIN */
    if (base2 > base) {
        base = base2;
    }
    base = base ? (base + 1): name;
    return base;
}

static char* _lib_base_name(char* name) {
    if (!name) {
        return NULL;
    }
    size_t len = strlen(name);
    if (len == 0) {
        return name;
    }

    //fprintf(stderr, ">> len   : %lu\n", len);

    long i = len - 1;
    while (i >= 0 && (name[i] == '/' || name[i] == '\\')) {
        i--;
    }

    //fprintf(stderr, ">> last  : %ld\n", i);

    if (i == -1) {
        //fprintf(stderr, ">> return <empty>\n");
        name[0] = '\0';
        return name;
    }

    long j = i;
    while (i >= 0 && (name[i] != '/' && name[i] != '\\')) {
        i--;
    }

    char* base = name;
    
    //fprintf(stderr, ">> first : %ld\n", i);

    if (j > 0 && j < len - 1) {
        //fprintf(stderr, ">> trunc : %ld\n", j);
        base[j + 1] = '\0'; // truncate
    }

    if (i >= 0) {
        //fprintf(stderr, ">> offset: %ld\n", i + 1);
        base = base + i + 1; // offset
    }

    //fprintf(stderr, ">> base  : %s\n", base);

    return base;
}

int run_basename(char* path, char* suffix) {
    if (_lib_stremp(path)) {
        return 1;
    }
    char* name = (char*) _lib_base_name(path);
    if (_lib_stremp(name)) {
        return 1;
    }
    name = _lib_strrdel(name, suffix);
    if (_lib_stremp(name)) {
        return 1;
    }
    fprintf(stdout, "%s\n", name);
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-basename string [suffix]\n");
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    char* path = NULL;  
    char* suffix = NULL;

    /*
    while ((opt = lib_getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            suffix = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }
    */

    if (error) {
        usage();
        return 0;
    }

    if (argc - optind < 1) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }

    //lib_sys_locale_init();

    path = argv[optind];
    
    if (++optind < argc) {
        suffix = argv[optind];
    }

    //fprintf(stderr, ">> path  : %s\n", path);
    //fprintf(stderr, ">> suffix: %s\n", suffix);

    int retval = run_basename(path, suffix);
 
    //lib_sys_locale_restore();
    
    return retval;
}