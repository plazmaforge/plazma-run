#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "getopt.h"

#define LIB_TYPE_BIN 1 << 1
#define LIB_TYPE_OCT 1 << 2
#define LIB_TYPE_DEC 1 << 3
#define LIB_TYPE_HEX 1 << 4

#define LIB_TYPE_DEF LIB_TYPE_DEC | LIB_TYPE_HEX
#define LIB_TYPE_ALL LIB_TYPE_BIN | LIB_TYPE_OCT | LIB_TYPE_DEC | LIB_TYPE_HEX

#define LIB_SEPARATE_BIN 1

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

static char _to_bin(char c, int i) {
    return ( (c & (1 << i)) ? '1' : '0' );
}

static void _to_bin_count(char* buf, char c, int count) {
    int n = count - 1;
    for (int i = n; i >= 0; --i) {
        buf[n - i] = _to_bin(c, i);
    }
}

static void _shift_bin_count(char* buf, int count) {
    int mid = count / 2;
    int n = count - 1;
    for (int i = n; i >= mid; --i) {
        buf[i + 1] = buf[i];
    }
    buf[mid] = ' ';
}

static void _to_bin8(char* buf, char c) {
    _to_bin_count(buf, c, 8);
}

static void _shift_bin8(char* buf) {
    _shift_bin_count(buf, 8);
}

static void _print_hcell(int type) {
    if (type & LIB_TYPE_DEC) {
        printf("Dec ");
    } 
    if (type & LIB_TYPE_HEX) {
        printf("Hex ");
    }
    if (type & LIB_TYPE_OCT) {
        printf("Oct ");
    }
    if (type & LIB_TYPE_BIN) {
        printf("Bin      ");
        if (LIB_SEPARATE_BIN) {
            printf(" ");
        }
    }
    printf("      ");
}

static void _print_tcell(int type, char c) {
    if (type & LIB_TYPE_DEC) {
        printf("%3d ", c);
    } 
    if (type & LIB_TYPE_HEX) {
        printf("%02X  ", c);
    }
    if (type & LIB_TYPE_OCT) {
        printf("%03o ", c);
    }
    if (type & LIB_TYPE_BIN) {
        size_t len = 8;
        if (LIB_SEPARATE_BIN) {
            len++;
        }
        char buf[len + 1];
        buf[len] = '\0';

        _to_bin8(buf, c);
        if (LIB_SEPARATE_BIN) {
            _shift_bin8(buf);
        }        
        printf("%s ", buf);
    }

    printf("%-3s   ", _to_chr(c));
}

void usage() {
    fprintf(stderr, "Usage: run-ascii [-dxoba]\n");
}

int main(int argc, char* argv[]) {

    int error = 0;
    int opt;

    int type = 0;

    while ((opt = lib_getopt(argc, argv, "dxoba")) != -1) {
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
        case 'a':
            /* Set ALL type */
            type = LIB_TYPE_ALL;
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

    // Print header
    for (int col = 0; col < 8; col++) {
        _print_hcell(type);
    }
    printf("\n");

    // Print table     
    char c;
    for (int row = 0; row < 16; row++) {
        for (int col = 0; col < 8; col++) {
            c = row + col * 16;
            _print_tcell(type, c);
        }
        printf("\n");
    }    
    return 0;
}

// https://www.johndcook.com/blog/2022/05/28/how-to-memorize-the-ascii-table/
// https://www.sciencebuddies.org/science-fair-projects/references/ascii-table
// https://c-for-dummies.com/blog/?p=4895