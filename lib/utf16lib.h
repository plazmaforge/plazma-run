#ifndef PLAZMA_LIB_UTF16LIB_H
#define PLAZMA_LIB_UTF16LIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"
#include "unidef.h"

static int lib_utf16_char_seq(const char* str);

static int lib_utf16be_char_seq(const char* str);

static int lib_utf16le_char_seq(const char* str);

static int lib_utf16_to_code(const char* str, int* cp);

static int lib_utf16be_to_code(const char* str, int* cp);

static int lib_utf16le_to_code(const char* str, int* cp);

static int lib_utf16_code_seq(int cp);

static int lib_utf16_to_char(char* buf, int cp);

static int lib_utf16be_to_char(char* buf, int cp);

static int lib_utf16le_to_char(char* buf, int cp);

////

// https://stackoverflow.com/questions/43564445/how-to-map-unicode-codepoints-from-an-utf-16-file-using-c

//// UTF-16

static int _utf16_char_seq(bool be, const char* str) {
    if (!str) {
        // error
        return -1;
    }
    int i1;
    int i2;
    if (be) {
        i1 = 0;
        i2 = 1;
    } else {
        i1 = 1;
        i2 = 0;
    }
    uint8_t b1  = _u8(str[i1]);
    uint8_t b2  = _u8(str[i2]);
    uint16_t c1 = _u16(b1, b2);

    if (c1 >= 0xD800 && c1 < 0xDC00) {
        return 4;
    }
    return 2;
}

static int lib_utf16_char_seq(const char* str) {
    return lib_utf16be_char_seq(str);
}

static int lib_utf16be_char_seq(const char* str) {
    return _utf16_char_seq(true, str);
}

static int lib_utf16le_char_seq(const char* str) {
    return _utf16_char_seq(false, str);
}

/*
 * Convert UTF-16 char to a codepoint.
 * Store the result to the codepoint.
 * Return lenght of the char or error (-1)
 */
static int _utf16_to_code(bool be, const char* str, int* cp) {
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
    uint8_t b1  = _u8(str[i1]);
    uint8_t b2  = _u8(str[i2]);
    uint16_t c1 = _u16(b1, b2);

    if (c1 >= 0xD800 && c1 < 0xDC00) {
        uint8_t b3  = _u8(str[i3]);
        uint8_t b4  = _u8(str[i4]);
        uint16_t c2 = _u16(b3, b4);
        *cp = ((c1 & 0x3FF) << 10) + (c2 & 0x3FF) + 0x10000;
        return 4;
    }
    *cp = c1;
    return 2;
}

static int lib_utf16_to_code(const char* str, int* cp) {
    return _utf16_to_code(true, str, cp);
}

static int lib_utf16be_to_code(const char* str, int* cp) {
    return _utf16_to_code(true, str, cp);
}

static int lib_utf16le_to_code(const char* str, int* cp) {
    return _utf16_to_code(false, str, cp);
}

static int lib_utf16_code_seq(int cp) {
    int err = lib_uni_check_range(cp);
    if (err != 0) {
        // error
        return err;
    }

    if(cp < 0xD800 || (cp > 0xDFFF && cp < 0x10000)) {
        return 2;
    }
    return 4;
}

/**
 * Convert the codepoint to UTF-16 char.
 * Store the result to the buffer.
 * Return lenght of the char or error (-1)
 */
static int _utf16_to_char(bool be, char* buf, int cp) {
    int err = lib_uni_check_range(cp);
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
    if(cp < 0xD800 || (cp > 0xDFFF && cp < 0x10000)) {
        buf[i1] = (cp >> 8) & 0xFF;
        buf[i2] = cp & 0xFF;
        buf[i3] = 0;
        buf[i4] = 0;            
        return 2;
    }

    cp -= 0x010000;

    uint16_t hi = (cp >> 10) + 0xD800;
    uint16_t lo = (cp & 0x03FF) + 0xDC00;

    buf[i1] = (hi >> 8) & 0xFF;
    buf[i2] = hi & 0xFF;
    buf[i3] = (lo >> 8) & 0xFF;;
    buf[i4] = lo & 0xFF;;

    return 4;
}

////

int lib_utf16_to_char(char* buf, int cp) {
   return lib_utf16be_to_char(buf, cp);
}

int lib_utf16be_to_char(char* buf, int cp) {
   return _utf16_to_char(true, buf, cp);
}

int lib_utf16le_to_char(char* buf, int cp) {
   return _utf16_to_char(false, buf, cp);
}

#endif // PLAZMA_LIB_UTF16LIB_H
