#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "getopt.h"

#define LIB_TYPE_BIN 1 << 1
#define LIB_TYPE_OCT 1 << 2
#define LIB_TYPE_DEC 1 << 3
#define LIB_TYPE_HEX 1 << 4

#define LIB_TYPE_DEF LIB_TYPE_DEC

const char* CHR[32] = {
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

static const char* _to_chr(char c) {
    if (c >= 0 && c <= 31) {
        return CHR[c];
    }
    if (c == 127) {
        return "DEL";
    }
    return _to_str(c);
}

static bool _non_print(char c) {
    return ((c >= 0 && c <= 31) || c == 127);
}

static char to_bin(char c, int i) {
    return ( (c & (1 << i)) ? '1' : '0' );
}

static void to_bin_count(char* buf, char c, int count) {
    int n = count - 1;
    for (int i = n; i >= 0; --i) {
        buf[n - i] = to_bin(c, i);
    }
}

static void to_bin8(char* buf, char c) {
    to_bin_count(buf, c, 8);
}

static void _print_char_dec(char c) {
    printf("%3d %-3s  ", c, _to_chr(c));
}

static void _print_char_hex(char c) {
    printf("%02X %-3s  ", c, _to_chr(c));
}

static void _print_char(int type, char c) {
    if (type & LIB_TYPE_DEC) {
        printf("%3d ", c);
    } 
    if (type & LIB_TYPE_HEX) {
        printf("%02X ", c);
    }
    if (type & LIB_TYPE_OCT) {
        printf("%03o ", c);
    }
    if (type & LIB_TYPE_BIN) {
        char buf[9];
        buf[8] = '\0';
        to_bin8(buf, c);
        printf("%s ", buf);
    }

    printf("%-3s  ", _to_chr(c));
}

void usage() {
    fprintf(stderr, "Usage: run-ascii [-dxob]\n");
}

int main(int argc, char* argv[]) {

    int error = 0;
    int opt;

    int type = 0; //LIB_TYPE_DEF;

    while ((opt = lib_getopt(argc, argv, "dxob")) != -1) {
        switch (opt) {
        case 'd':
            /* Set DEC type */
            type |= LIB_TYPE_DEC;
            break;
        case 'x':
            /* Set HEX type */
            type |= LIB_TYPE_HEX;
            break;
        case 'o':
            /* Set OCT type */
            type |= LIB_TYPE_OCT;
            break;
        case 'b':
            /* Set BIN type */
            type |= LIB_TYPE_BIN;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (type == 0) {
        type = LIB_TYPE_DEF;
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