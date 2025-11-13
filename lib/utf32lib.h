#ifndef PLAZMA_LIB_UTF32LIB_H
#define PLAZMA_LIB_UTF32LIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"

//// UTF-32

static int lib_utf32_char_seq(const char* str);

static int lib_utf32_to_code(const char* str, int* cp);

static int lib_utf32be_to_code(const char* str, int* cp);

static int lib_utf32le_to_code(const char* str, int* cp);

static int lib_utf32_code_seq(int cp);

static int lib_utf32_to_char(char* buf, int cp);

static int lib_utf32be_to_char(char* buf, int cp);

static int lib_utf32le_to_char(char* buf, int cp);

//// UTF-32

static int lib_utf32_char_seq(const char* str) {
    if (!str) {
        return -1;
    }
    return 4;
}

/**
 * Convert UTF-32 char to the codepoint
 */
static int _utf32_to_code(bool be, const char* str, int* cp) {
    if (!str) {
        return -1;
    }

    int i1, i2, i3, i4;
    if (be) {
        i1 = 0; 
        i2 = 1; 
        i3 = 2; 
        i4 = 3;
    } else {
        i1 = 1; 
        i2 = 0; 
        i3 = 3; 
        i4 = 2;
    }

    uint8_t b1 = (uint8_t) str[i1];
    uint8_t b2 = (uint8_t) str[i2];
    uint8_t b3 = (uint8_t) str[i3];
    uint8_t b4 = (uint8_t) str[i4];
    *cp = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    //return 0;
    return 4;
}

static int lib_utf32_code_seq(int cp) {

    int err = lib_uni_check_range(cp);
    if (err != 0) {
        return err;
    }
    return 4;
}

/**
 * Convert the codepoint to UTF-32 char.
 * Store the result ot the buffer.
 */
static int _utf32_to_char(bool be, char* buf, int cp) {

    int err = lib_uni_check_range(cp);
    if (err != 0) {
        return err;
    }

    // 128640
    // 1F680
    // U+0001F680
    // -----------
    // 00 01 F6 80

    int i1, i2, i3, i4;
    if (be) {
        i1 = 0; 
        i2 = 1; 
        i3 = 2; 
        i4 = 3;
    } else {
        i1 = 1; 
        i2 = 0; 
        i3 = 3; 
        i4 = 2;
    }

    buf[i1] = (cp >> 24) & 0xFF;
    buf[i2] = (cp >> 16) & 0xFF;
    buf[i3] = (cp >> 8) & 0xFF;
    buf[i4] = cp & 0xFF;

    //return 0;
    return 4;
}

////

static int lib_utf32_to_code(const char* str, int* cp) {
    return lib_utf32be_to_code(str, cp);
}

static int lib_utf32be_to_code(const char* str, int* cp) {
    return _utf32_to_code(true, str, cp);
}

static int lib_utf32le_to_code(const char* str, int* cp) {
    return _utf32_to_code(false, str, cp);
}

//

static int lib_utf32_to_char(char* buf, int cp) {
   return lib_utf32be_to_char(buf, cp);
}

static int lib_utf32be_to_char(char* buf, int cp) {
   return _utf32_to_char(true, buf, cp);
}

static int lib_utf32le_to_char(char* buf, int cp) {
   return _utf32_to_char(false, buf, cp);
}

#endif // PLAZMA_LIB_UTF32LIB_H
