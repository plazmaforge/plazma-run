#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "clilib.h"
#include "run_dclib.h"


int _lib_stricmp(const char* str1, const char* str2) {
  if (!str1 && !str2) {
    return 0;
  }
  if (!str1) {
    return -1;
  }
  if (!str2) {
    return 1;
  }
  if (str1 == str2) {
    return 0;
  }

  char* s1 = (char*) str1;
  char* s2 = (char*) str2;
  int c1   = 0;
  int c2   = 0;

  while (true) {
    if (*s1 == '\0') {
      return (*s2 == '\0') ? 0 : -1;
    }
    if (*s2 == '\0') {
      return (*s1 == '\0') ? 0 : 1;
    }
    c1 = tolower(*s1);
    c2 = tolower(*s2);
    if (c1 < c2) {
      return -1;
    }
    if (c2 < c1) {
      return 1;
    }
    s1++;
    s2++;
  }
  return 0;
}

static int _get_base(char* name) {
    if (!name)  {
        return 0;
    }

    if (_lib_stricmp(name, "bin") == 0) {
        return 2;
    } else if (_lib_stricmp(name, "oct") == 0) {
        return 8;
    } else if (_lib_stricmp(name, "dec") == 0) {
        return 10;
    } else if (_lib_stricmp(name, "hex") == 0) {
        return 16;
    }

    return 0;
}

bool _is_support(int base) {
    return base == 2 
    || base == 8 
    || base == 10 
    || base == 16;
}

void usage() {
    fprintf(stderr, "Usage: run-dconv -f <from> -t <to> <value>\n");
}

static void print_list() {
    fprintf(stdout, " 2, bin\n");
    fprintf(stdout, " 8, oct\n");
    fprintf(stdout, "10, dec\n");
    fprintf(stdout, "16, hex\n");
}

int main(int argc, char* argv[]) {

    lib_cli_prog_init(argv);

    int argm = 1;
    if (lib_cli_not_argind(argc, argm)) {        
        usage();
        return 1;
    }
    
    int error = 0;
    int opt;

    char* from_type = NULL;
    char* to_type   = NULL;
    int from_base   = 0;
    int to_base     = 0;

    bool opt_list   = false;

    while ((opt = lib_getopt(argc, argv, "lf:t:")) != -1) {
        switch (opt) {
        case 'l':
            opt_list = true;
            break;
        case 'f':
            from_type = optarg;
            break;
        case 't':
            to_type = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }

        if (opt_list) {
            break;
        }
    }

    if (opt_list) {
        print_list();
        return 0;
    }

    if (!from_type || !to_type) {
        error = 1;
    }

    if (error) {
        usage();
        return 1;
    }

    from_base = _get_base(from_type);
    to_base = _get_base(to_type);

    if (from_base == 0) {
        from_base = atoi(from_type);
    }
    if (to_base == 0) {
        to_base = atoi(to_type);
    }

    error = 0;
    if (!_is_support(from_base)) {
        error = 1;
        fprintf(stderr, "Incorrect 'from' value: %d. The value must be: 2, 8, 10, 16\n", from_base);
    }
    if (!_is_support(to_base)) {
        error = 1;
        fprintf(stderr, "Incorrect 'to' value: %d. The value must be: 2, 8, 10, 16\n", to_base);
    }

    if (error) {
        return 1;        
    }

    const char* str = argv[optind];
    uint64_t num = 0;

    // fprintf(stderr, "from_type: %s\n", from_type);
    // fprintf(stderr, "to_type  : %s\n", to_type);
    // fprintf(stderr, "from_base: %d\n", from_base);
    // fprintf(stderr, "to_base  : %d\n", to_base);
    // fprintf(stderr, "str      : %s\n", str);

    error = run_str2num(str, from_base, &num);
    if (error) {
        return 1;        
    }

    if (to_base == 2) {
        ////
    } else if (to_base == 8) {
        fprintf(stdout, "%llo\n", num);
    } else if (to_base == 10) {
        fprintf(stdout, "%llu\n", num);
    } else if (to_base == 16) {
        fprintf(stdout, "%llX\n", num);
    }
    
    return 0;
}
