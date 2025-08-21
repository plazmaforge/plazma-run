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

    int version   = 0;
    int format    = 0;
    int count     = 1;
    bool trailing = true; // '\n'
    bool upper    = is_upper_format_default();
    bool pack     = false;

    lib_uuid_t nsid = NameSpace_DNS;
    char* name = "www.widgets.com";
 
    while ((opt = lib_getopt(argc, argv, "nlupv:c:")) != -1) {
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

    // if (argc - optind < min_arg) {
    //     fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
    //     usage();
    //     return 0;
    // }

    if (version == 0) {
        version = 1;
    }

    format = get_format(upper, pack);
    error = run_uuid(version, format, nsid, name, count, trailing);
    if (error != 0) {
        return 1;
    }
    
    return 0;
}