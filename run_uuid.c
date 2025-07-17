#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "getopt.h"
#include "clilib.h"
#include "uuid.h"

static bool supports_version(int version) {
    return (version == 1 
        || version == 3
        || version == 5
        || version == 7);
}

static int uuid_create(int version, lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen) {
    if (version == 1) {
        lib_uuid_create_v1(uuid);
        return 0;
    } else if (version == 3) {
        lib_uuid_create_v3(uuid, NameSpace_DNS, name, namelen);
        return 0;
    } else if (version == 5) {
        lib_uuid_create_v5(uuid, NameSpace_DNS, name, namelen);
        return 0;
    } else if (version == 7) {
        lib_uuid_create_v7(uuid);
        return 0;
    }
    return -1;
}

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

int run_uuid(int version, int format, /*lib_uuid_t nsid, char* name,*/ int count, bool trailing) {
    if (!supports_version(version)) {
        return -1;
    }

    //NameSpace_DNS, "www.widgets.com"
    lib_uuid_t nsid = NameSpace_DNS;
    void* name = "www.widgets.com";
    size_t namelen = 15;

    for (int i = 0; i < count; i++) {
        lib_uuid_t uuid;
        uuid_create(version, &uuid, nsid, name, namelen);

        lib_uuid_printf(format, uuid);

        ////
        /*
        unsigned char v[16];
        lib_uuid_pack(&uuid, v);
        printf("  ");
        for (int i = 0; i < 16; i++) {
            printf("%2.2x", v[i]);
        }

        lib_uuid_t x;
        lib_uuid_unpack(v, &x);

        printf("  ");
        lib_uuid_print(x);
        */

        ////

        if (version == 1) {
            lib_uuid_reset(); // Why?
        }
        if (count > 1 && i < count - 1) {
            printf("\n");
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
            } else if (!supports_version(version)) {
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
        return 0;
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
    run_uuid(version, format, count, trailing);
    
    return 0;
}