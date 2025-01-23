#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"

static bool _lib_stremp(char* str) {
    return (!str || *str == '\0');
}

static char* _lib_dir_name(char* name) {
    if (!name) {
        return NULL;
    }
    size_t len = strlen(name);
    if (len == 0) {
        return name;
    }

    //fprintf(stderr, ">> len   : %lu\n", len);

    // skip slashes and stop at first char or out side
    ssize_t i = len - 1;
    while (i >= 0 && (name[i] == '/' || name[i] == '\\')) {
        i--;
    }

    //fprintf(stderr, ">> last  : %ld\n", i);

    if (i == -1) {
        //fprintf(stderr, ">> return[1] '/'\n");

        // out side: all chars are slashes, for example
        // '/',  '\\',  '////', '\\\\', '////\\\\' , '\\\\////'

        name[0] = '/'; // TODO: What about WIN?
        name[1] = '\0';
        return name;
    }

    // skip chars and stop at first slashe or out side
    ssize_t j = i;
    while (i >= 0 && (name[i] != '/' && name[i] != '\\')) {
        i--;
    }

    if (i == -1) {
        //fprintf(stderr, ">> return[2] '.'\n");

        // out side: for example
        // 'abc/',  'abc\\',  'abc////', 'abc\\\\', 'abc////\\\\' , 'abc\\\\////'

        name[0] = '.'; // TODO: What about WIN?
        name[1] = '\0';
        return name;
    }

    // skip separators and stop at first char or out side
    while (i >= 0 && (name[i] == '/' || name[i] == '\\')) {
        i--;
    }

    if (i == -1) {
        //fprintf(stderr, ">> return[3] '/'\n");

        // out side: for example
        // '/abc/',  '\\abc\\',  '////abc////', '\\\\abc\\\\', '////\\\\abc////\\\\' , '\\\\////abc\\\\////'

        name[0] = '/'; // TODO: What about WIN?
        name[1] = '\0';
        return name;
    }

    char* base = name;

    base[i + 1] = '\0'; // truncate    
    //fprintf(stderr, ">> base  : %s\n", base);

    return base;
}

int run_dirname(char* path) {
    if (_lib_stremp(path)) {
        return 1;
    }
    char* name = (char*) _lib_dir_name(path);
    //if (_lib_stremp(name)) {
    //    return 1;
    //}
    fprintf(stdout, "%s\n", name);
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-dirname string\n");
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
    
    //fprintf(stderr, ">> path  : %s\n", path);

    int retval = run_dirname(path);
 
    //lib_sys_locale_restore();
    
    return retval;
}