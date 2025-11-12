#ifndef PLAZMA_LIB_UTF7LIB_H
#define PLAZMA_LIB_UTF7LIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"

// uint8_t to_base64
static const int map1[64] = {
    
    /* A-Z */
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,

    /* a-z */
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,

    /* 0-9 */
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57,

    /* +/ */
    43, 47
};

// int8_t from_base64
static const int map2[128] = {

    /* C0 controls, -1 for legal ones (CR LF TAB), -3 for illegal ones */
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -1, -1, -3, -3, -1, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,

    /* general punctuation with + and / and a special value (-2) for - */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -2, -1, 63,

    /* digits */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,

    /* A-Z */
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -3, -1, -1, -1,

    /* a-z */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -3, -3
};

/**
 * Calculate lenght of output base64 array by input lenght
 */
static size_t _b64_len(size_t len) {
    return (len * 4 + 2) / 3;
}

/**
 * Calculate lenght of output UTF16 array by input lenght
 */
static size_t _u16_len(size_t len) {
    return (len * 3) / 4;
}

static int base64_encode(char* idata, size_t isize, char* odata, size_t osize) {

    size_t dsize = (isize * 4 + 2) / 3;
    // osize = ((isize + 2) / 3) * 4;  // with ==
    // osize = dsize;

    size_t i = 0;
    size_t j = 0;

    uint8_t i0, i1, i2 = 0;
    uint8_t j0, j1, j2, j3 = 0;

    while (i < isize) {
        i0 = _u8(idata[i]);
        i++;

        if (i < isize) {
            i1 = _u8(idata[i]);
            i++;
        } else {
            i1 = 0;
        }

        if (i < isize) {
            i2 = _u8(idata[i]);
            i++;
        } else {
            i2 = 0;
        }

        ////

        //fprintf(stderr, ">> i-0: %d\n", i0);
        //fprintf(stderr, ">> i-1: %d\n", i1);
        //fprintf(stderr, ">> i-2: %d\n", i2);

        j0 = i0 / 4;
        j1 = ((i0 & 3) * 0x10) | (i1 / 0x10);
        j2 = ((i1 & 0x0F) * 4) | (i2 / 0x40);
        j3 = i2 & 0x3F;

        odata[j] = map1[j0];
        //fprintf(stderr, ">> odata-0: %d\n", odata[j]);
        j++;

        odata[j] = map1[j1];
        //fprintf(stderr, ">> odata-1: %d\n", odata[j]);
        j++;

        if (j < dsize) {
            odata[j] = map1[j2];
            //fprintf(stderr, ">> odata-2: %d\n", odata[j]);
        } else {
            // odata[j] = '=';
        }
        j++;

        if (j < dsize) {
            odata[j] = map1[j3];
            //fprintf(stderr, ">> odata-3: %d\n", odata[j]);
        } else {
            // odata[j] = '=';
        }
        j++;

    }
    return 0;
}


static int base64_decode(char* idata, size_t isize, char* odata, size_t osize) {

    // u16_len
    // osize = (isize * 3) / 4;

    size_t i = 0;
    size_t j = 0;

    char i0, i1, i2, i3 = 0;
    char b0, b1, b2, b3 = 0;
    char j0, j1, j2 = 0;

    while (i < isize) {
        i0 = idata[i];
        i++;
        i1 = idata[i];
        i++;

        if (i < isize) {
            i2 = idata[i];
            i++;
        } else {
            i2 = 'A';
        }

        if (i < isize) {
            i3 = idata[i];
            i++;
        } else {
            i3 = 'A';
        }

        // check i0-i3 > 127: error

        b0 = map2[i0];
        b1 = map2[i1];
        b2 = map2[i2];
        b3 = map2[i3];

        // check b0-b3 > 63: error

        j0 = (b0 * 4) | b1 / 0x10;
        j1 = ((b1 & 0x0F) * 0x10) | (b2 / 4);
        j2 = ((b2 & 3) * 0x40) | b3;

        odata[j] = j0;
        j++;
        if (j < osize) {
            odata[j] = j1;
            j++;
        }
        if (j < osize) {
            odata[j] = j2;
            j++;
        }
    }
    return 0;
}

/* Set directly */
static bool IN_SET_D(int c) {
    return
       (c >= 65 && c <= 90)  /* A-Z */
    || (c >= 97 && c <= 122) /* a-z */
    || (c >= 48 && c <= 57)  /* 0-9 */
    || c == 39 
    || c == 40               /* ( */ 
    || c == 41               /* ) */ 
    || (c >= 44 && c <= 47)
    || c == 58               /* : */ 
    || c == 63;              /* ? */ 
}

/* Set optional */
static bool IN_SET_O(int c) {
    return
       (c >= 33 && c <= 38)
    || c == 42               /* * */
    || (c >= 59 && c <= 64)
    || c == 91
    || (c >= 93 && c <= 96)
    || (c >= 123 && c <= 125);
}

/* Set base64 */
static bool IN_SET_B(int c) {
    return
       (c >= 65 && c <= 90)  /* A-Z */
    || (c >= 97 && c <= 122) /* a-z */
    || (c >= 48 && c <= 57)  /* 0-9 */
    || c == 43 /* + */
    || c == 47; /* / */
}

#endif // PLAZMA_LIB_UTF7LIB_H
