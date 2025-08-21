#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "getopt.h"
#include "clilib.h"
#include "uuid.h"

static int get_format(bool upper, bool pack) {
    if (upper && pack) {
        return LIB_UUID_FORMAT_TYPE_UPPER_PACK;
    } 
    if (!upper && !pack) {
        return LIB_UUID_FORMAT_TYPE_LOWER;
    }   
    if (upper) {
        return LIB_UUID_FORMAT_TYPE_UPPER;
    } 
    if (pack) {
        return LIB_UUID_FORMAT_TYPE_LOWER_PACK;
    }
    return LIB_UUID_FORMAT_TYPE;
}

static bool is_upper_format_default() {
    return (LIB_UUID_FORMAT_TYPE == LIB_UUID_FORMAT_TYPE_UPPER 
    || LIB_UUID_FORMAT_TYPE == LIB_UUID_FORMAT_TYPE_UPPER_PACK);
}

static char* _lib_strnew(size_t size) {
  size_t _size = size * sizeof(char) + 1;
  char* str = (char*) malloc(_size);
  if (!str) {
    return NULL;
  }
  memset(str, 0, _size);
  return str;
}

static char* _lib_strndup(const char* src, size_t num) {
  char* dst = _lib_strnew(num);
  if (!dst) {
    return NULL;
  }    
  memcpy(dst, src, num);
  //dst[num] = '\0';
  return dst;
}

static char* _lib_strdup(const char* src) {
  if (!src) {
    return NULL;
  }
  size_t len = strlen(src);
  return _lib_strndup(src, len);
}


int run_uuid(int version, int format, lib_uuid_t nsid, char* name, int count, bool trailing) {
    if (!lib_uuid_supports_version(version)) {
        return 1;
    }

    int error = 0;

    for (int i = 0; i < count; i++) {
        lib_uuid_t uuid;

        error = lib_uuid_create_vx(&uuid, nsid, name, version);
        if (error != 0) {
            fprintf(stderr, "%s: Error generation UUID: version=%d\n", prog_name, version);
            break;
        }

        lib_uuid_printf(format, uuid);
        if (count > 1 && i < count - 1) {
            printf("\n");
        }
 
        if (version == 1) {
            lib_uuid_reset(); // Why?
        }
    }

    if (trailing) {
        printf("\n");
    }
    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-uuid [-nlup] [-v version] [-c count]\n");
}

int main(int argc, char* argv[]) {
    
    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;
    int long_ind;

    int version   = 1;    // by default
    int format    = 0;
    int count     = 1;    // by default
    bool trailing = true; // '\n'
    bool upper    = is_upper_format_default();
    bool pack     = false;

    char* space   = NULL;
    char* name    = NULL;

    lib_uuid_t nsid = NameSpace_DNS;

    static lib_option long_options[] = {
          {"name",  optional_argument, 0, 1004},
          {"space", optional_argument, 0, 1005},
          {NULL,    0,                 0, 0 }
    };

    //while ((opt = lib_getopt(argc, argv, "nlupv:c:")) != -1) {
    while ((opt = lib_getopt_long(argc, argv, "nlupv:c:", long_options, &long_ind)) != -1) {
        
        switch (opt) {
        case 'n':
            trailing = false;
            break;
        case 'l':
            upper = false;
            break;
        case 'u':
            upper = true;
            break;
        case 'p':
            pack = true;
            break;
        case 'v':
            version = atoi(optarg);
            if (version == 0) {
                fprintf(stderr, "%s: Incorrect version: %s\n", prog_name, optarg);
                error = 1;
            } else if (!lib_uuid_supports_version(version)) {
                fprintf(stderr, "%s: Unsupported version: %s\n", prog_name, optarg);
                error = 1;
            }
            break;
        case 'c':
            count = atoi(optarg);
            if (count == 0) {
                fprintf(stderr, "%s: Incorrect count value: %s\n", prog_name, optarg);
                error = 1;
            }
            break;
        case 1004: // name
            name = _lib_strdup(optarg);
            //fprintf(stderr, "%s: Name: %s\n", prog_name, name);
            break;
        case 1005: // space
            space = _lib_strdup(optarg);
            //fprintf(stderr, "%s: Space: %s\n", prog_name, space);
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (lib_uuid_supports_ns_version(version)) {
        if (!name) {
            //name = _lib_strdup("www.widgets.com");
            fprintf(stderr, "%s: Name argument is requered for version 3 (MD5) and 5 (SHA1)\n", prog_name);
            error = 1;
        } else {
            if (count > 1) {
                fprintf(stderr, "%s: Incorrect count value for version 3 (MD5) and 5 (SHA1): %d. Value must be 1\n", prog_name, count);
                error = 1;
            } else {
              if (!space) {
                  space = _lib_strdup("DNS"); // by default
              }
              nsid = lib_uuid_get_nsid(space);
              if (nsid.time_hi_and_version == 0) {
                  fprintf(stderr, "%s: Name Space not found: %s\n", prog_name, space);
                  error = 1;
              }
            }
        }
    } else {
        if (name) {
            fprintf(stderr, "%s: Name agrument is requered for version 3 (MD5) and 5 (SHA1) only: %s\n", prog_name, name);
            error = 1;
        }
        if (space) {
            fprintf(stderr, "%s: Name Space agrument is requered for version 3 (MD5) and 5 (SHA1): %s\n", prog_name, space);
            error = 1;
        }
    }

    if (error) {
        if (name) {
            free(name);
        }
        if (space) {
            free(space);
        }
        usage();
        return 1;
    }

    // if (argc - optind < min_arg) {
    //     fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
    //     usage();
    //     return 0;
    // }

    format = get_format(upper, pack);
    error = run_uuid(version, format, nsid, name, count, trailing);
    if (name) {
        free(name);
    }
    if (space) {
        free(space);
    }

    if (error != 0) {
        return 1;
    }
    
    return 0;
}