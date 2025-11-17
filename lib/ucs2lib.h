#ifndef PLAZMA_LIB_UCS2LIB_H
#define PLAZMA_LIB_UCS2LIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"
#include "unidef.h"

static int lib_ucs2_char_seq(const char* str);

static int lib_ucs2be_char_seq(const char* str);

static int lib_ucs2le_char_seq(const char* str);

static int lib_ucs2_to_code(const char* str, int* cp);

static int lib_ucs2_to_code(const char* str, int* cp);

static int lib_ucs2le_to_code(const char* str, int* cp);

static int lib_ucs2_code_seq(int cp);

static int lib_ucs2_to_char(char* buf, int cp);

static int lib_ucs2be_to_char(char* buf, int cp);

static int lib_ucs2le_to_char(char* buf, int cp);


//// UCS-2

//// UCS-2: check

static int lib_ucs2_check_range(int cp) {
    if (cp < 0) {
        return LIB_UNI_TOO_SMALL; // For [int] only
    }
    if (cp > LIB_UCS2_MAX) {
        return LIB_UNI_TOO_BIG;
    }
    return 0;
}

////

static int _ucs2_char_seq(bool be, const char* str) {
    if (!str) {
        // error
        return -1;
    }
    return 2; // always two
}

static int lib_ucs2_char_seq(const char* str) {
    return lib_ucs2be_char_seq(str);
}

static int lib_ucs2be_char_seq(const char* str) {
    return _ucs2_char_seq(true, str);
}

static int lib_ucs2le_char_seq(const char* str) {
    return _ucs2_char_seq(false, str);
}

/*
 * Convert UCS-2 char to a codepoint.
 * Store the result to the codepoint.
 * Return lenght of the char or error (-1)
 */
static int _ucs2_to_code(bool be, const char* str, int* cp) {
    if (!str) {
        // error
        return -1;
    }
    int i1, i2;
    if (be) {
        i1 = 0; 
        i2 = 1; 
    } else {
        i1 = 1; 
        i2 = 0; 
    }
    uint8_t b1  = _u8(str[i1]);
    uint8_t b2  = _u8(str[i2]);
    uint16_t c  = _u16(b1, b2);
    *cp = c;
    return 2; // always two
}

static int lib_ucs2_to_code(const char* str, int* cp) {
    return _ucs2_to_code(true, str, cp);
}

static int lib_ucs2be_to_code(const char* str, int* cp) {
    return _ucs2_to_code(true, str, cp);
}

static int lib_ucs2le_to_code(const char* str, int* cp) {
    return _ucs2_to_code(false, str, cp);
}

static int lib_ucs2_code_seq(int cp) {
    int err = lib_ucs2_check_range(cp);
    if (err != 0) {
        // error
        return err;
    }
    return 2; // always two
}

/**
 * Convert the codepoint to UCS-2 char.
 * Store the result to the buffer.
 * Return lenght of the char or error (-1)
 */
static int _ucs2_to_char(bool be, char* buf, int cp) {
    int err = lib_ucs2_check_range(cp);
    if (err != 0) {
        // error
        return err;
    }

    int i1, i2;
    if (be) {
        i1 = 0; 
        i2 = 1; 
    } else {
        i1 = 1; 
        i2 = 0; 
    }
    buf[i1] = (cp >> 8) & 0xFF;
    buf[i2] = cp & 0xFF;
    return 2; // always two
}

////

int lib_ucs2_to_char(char* buf, int cp) {
   return lib_ucs2be_to_char(buf, cp);
}

int lib_ucs2be_to_char(char* buf, int cp) {
   return _ucs2_to_char(true, buf, cp);
}

int lib_ucs2le_to_char(char* buf, int cp) {
   return _ucs2_to_char(false, buf, cp);
}

/*
  https://unicodebook.readthedocs.io/unicode_encodings.html
*/

#endif // PLAZMA_LIB_UCS2LIB_H
