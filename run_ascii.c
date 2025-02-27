#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "getopt.h"

const char* NAME[32] = {
    "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
    "BS",  "HT",  "LF",  "VT",  "FF",  "CR",  "SO",  "SI",
    "DLE", "DC1", "DC2", "DC3", "DC4", "NAC", "SYN", "ETB",
    "CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US"
    };

static const char* _to_str(char c) {
    static char str[2];
    str[0] = c;
    str[1] = '\0';
    return str;
}

static const char* _to_name(char c) {
    if (c >= 0 && c <= 31) {
        return NAME[c];
    }
    if (c == 127) {
        return "DEL";
    }
    //return "";
    return _to_str(c);
}

static bool _non_print(char c) {
    return ((c >= 0 && c <= 31) || c == 127);
}

static void _print_char_dec(char c) {
    printf("%3d %-3s  ", c, _to_name(c));
    //if (_non_print(c)) {
    //    printf("%3d %-3s  ", c, _to_name(c));
    //} else {
    //    printf("%3d %c    ", c, c);
    //}
}

static void _print_char_hex(char c) {
    printf("%02X %-3s  ", c, _to_name(c));
    //if (_non_print(c)) {
    //    printf("%02X %-3s  ", c, _to_name(c));
    //} else {
    //    printf("%02X %c    ", c, c);
    //}
}

static void _print_char(int type, char c) {
    if (type == 1) {
        _print_char_dec(c);
    } else if (type == 2) {
        _print_char_hex(c);
    }
}

void usage() {
    fprintf(stderr, "Usage: run-ascii [-d | -h]\n");
}

int main(int argc, char* argv[]) {

    int error = 0;
    int opt;

    int type = 1;

    while ((opt = lib_getopt(argc, argv, "dh")) != -1) {
        switch (opt) {
        case 'd':
            /* Set DEC type */
            type = 1;
            break;
        case 'h':
            /* Set HEX type */
            type = 2;
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
     
    char c;
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
            c = row + col * 16;
            _print_char(type, c);
        }
        printf("\n");
    }    
    return 0;
}

// https://www.johndcook.com/blog/2022/05/28/how-to-memorize-the-ascii-table/
// https://www.sciencebuddies.org/science-fair-projects/references/ascii-table
// https://c-for-dummies.com/blog/?p=4895