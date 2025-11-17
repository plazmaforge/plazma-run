#ifndef PLAZMA_LIB_UCS4LIB_H
#define PLAZMA_LIB_UCS4LIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"
#include "unidef.h"

static int lib_ucs4_char_seq(const char* str);

static int lib_ucs4be_char_seq(const char* str);

static int lib_ucs4le_char_seq(const char* str);

static int lib_ucs4_to_code(const char* str, int* cp);

static int lib_ucs4_to_code(const char* str, int* cp);

static int lib_ucs4le_to_code(const char* str, int* cp);

static int lib_ucs4_code_seq(int cp);

static int lib_ucs4_to_char(char* buf, int cp);

static int lib_ucs4be_to_char(char* buf, int cp);

static int lib_ucs4le_to_char(char* buf, int cp);


//// UCS-4

//// UCS-4: check

static int lib_ucs4_check_range(int cp) {
    if (cp < 0) {
        return LIB_UNI_TOO_SMALL; // For [int] only
    }
    if (cp > LIB_UCS4_MAX) {
        return LIB_UNI_TOO_BIG;
    }
    return 0;
}

////

static int _ucs4_char_seq(bool be, const char* str) {
    if (!str) {
        // error
        return -1;
    }
    return 4; // always four
}

static int lib_ucs4_char_seq(const char* str) {
    return lib_ucs4be_char_seq(str);
}

static int lib_ucs4be_char_seq(const char* str) {
    return _ucs4_char_seq(true, str);
}

static int lib_ucs4le_char_seq(const char* str) {
    return _ucs4_char_seq(false, str);
}

/*
 * Convert UCS-4 char to a codepoint.
 * Store the result to the codepoint.
 * Return lenght of the char or error (-1)
 */
static int _ucs4_to_code(bool be, const char* str, int* cp) {
    if (!str) {
        // error
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
    return 4; // always four
}

static int lib_ucs4_to_code(const char* str, int* cp) {
    return _ucs4_to_code(true, str, cp);
}

static int lib_ucs4be_to_code(const char* str, int* cp) {
    return _ucs4_to_code(true, str, cp);
}

static int lib_ucs4le_to_code(const char* str, int* cp) {
    return _ucs4_to_code(false, str, cp);
}

static int lib_ucs4_code_seq(int cp) {
    int err = lib_ucs4_check_range(cp);
    if (err != 0) {
        // error
        return err;
    }
    return 4; // always two
}

/**
 * Convert the codepoint to UCS-4 char.
 * Store the result to the buffer.
 * Return lenght of the char or error (-1)
 */
static int _ucs4_to_char(bool be, char* buf, int cp) {
    int err = lib_ucs4_check_range(cp);
    if (err != 0) {
        // error
        return err;
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

    buf[i1] = (cp >> 24) & 0xFF;
    buf[i2] = (cp >> 16) & 0xFF;
    buf[i3] = (cp >> 8) & 0xFF;
    buf[i4] = cp & 0xFF;

    return 4; // always four
}

////

int lib_ucs4_to_char(char* buf, int cp) {
   return lib_ucs4be_to_char(buf, cp);
}

int lib_ucs4be_to_char(char* buf, int cp) {
   return _ucs4_to_char(true, buf, cp);
}

int lib_ucs4le_to_char(char* buf, int cp) {
   return _ucs4_to_char(false, buf, cp);
}

/*
  https://unicodebook.readthedocs.io/unicode_encodings.html
*/

#endif // PLAZMA_LIB_UCS4LIB_H
