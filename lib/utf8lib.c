#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "encpre.h"
#include "encbom.h"
#include "unilib.h"
#include "intlib.h"
#include "utf8lib.h"

/*

https://datatracker.ietf.org/doc/html/rfc3629

Char. number range     |        UTF-8 octet sequence
      (hexadecimal)    |              (binary)
   --------------------+---------------------------------------------
   0000 0000-0000 007F | 0xxxxxxx
   0000 0080-0000 07FF | 110xxxxx 10xxxxxx
   0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
   0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/

// https://tools.ietf.org/html/rfc3629

static const int LIB_UTF8_SEQ[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0: 1: 0x00 - 0x0F */ /* 0x00 - 0x7F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 1: 1: 0x10 - 0x1F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 2: 1: 0x20 - 0x2F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 3: 1: 0x30 - 0x3F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4: 1: 0x40 - 0x4F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 5: 1: 0x50 - 0x5F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 6: 1: 0x60 - 0x6F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 7: 1: 0x70 - 0x7F */ /* ----------- */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8: 1: 0x80 - 0x8F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9: 1: 0x90 - 0x9F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* A: 1: 0xA0 - 0xAF */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* B: 1: 0xB0 - 0xBF */
    0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* C: 2: 0xC0 - 0xCF */ /* 0xC2 - 0xDF */
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* D: 2: 0xD0 - 0xDF */ /* ----------- */
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* E: 3: 0xE0 - 0xEF */ /* 0xE0 - 0xEF */
    4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* F: 4: 0xF0 - 0xFF */ /* 0xF0 - 0xF4 */
};

/* Surrogate pair zone. */
#define LIB_UNI_SUR_HIGH_START  0xD800
#define LIB_UNI_SUR_HIGH_END    0xDBFF
#define LIB_UNI_SUR_LOW_START   0xDC00
#define LIB_UNI_SUR_LOW_END     0xDFFF

/* Start of the "not character" range. */
#define LIB_UNI_NOT_CHAR_MIN    0xFDD0

/* End of the "not character" range.  */
#define LIB_UNI_NOT_CHAR_MAX    0xFDEF

/* Error codes                        */
#define LIB_UNI_SUR_PAIR       -2
#define LIB_UNI_TOO_BIG        -7
#define LIB_UNI_NOT_CHAR       -8

#define LIB_UTF8_BAD_BYTE      -1

/* The maximum possible value of a Unicode code point. See
   http://www.cl.cam.ac.uk/~mgk25/unicode.html#ucs. */

#define LI__UNI_MAX            0x10ffff
#define LIB_UNI_MAX            0x10FFFF

/* The maximum possible value which will fit into four bytes of
   UTF-8. This is larger than UNICODE_MAXIMUM. */

#define LI__UTF8_4             0x1fffff
#define LIB_UTF8_4             0x1FFFFF

//// static

static int _utf8_to_code(const char* str, int len);

static int _utf8_to_char(char* buf, int cp, int len);

static int _utf8_to_case_char(
    int mode, const char* str, char* buf,
    int* cp1, int* len1, int* cp2, int* len2);

static void _reset_var(int* var1, int* var2);

static void _reset_val(int* val);

static void _reset_buf(char* buf);

// static uint8_t _u8(char value) {
//     return (uint8_t) value;
// }

//// uni: check

static int _uni_check_ffff(int cp) {
    if ((cp & 0xFFFF) >= 0xFFFE) {
        return LIB_UNI_NOT_CHAR;
    }
    return 0;
}

static int _uni_check_not_char(int cp) {
    if (cp >= LIB_UNI_NOT_CHAR_MIN && cp <= LIB_UNI_NOT_CHAR_MAX) {
	  return LIB_UNI_NOT_CHAR;
    }
    return 0;
}

static int _uni_check_sur(int cp) {
    if (cp >= LIB_UNI_SUR_HIGH_START && cp <= LIB_UNI_SUR_LOW_END) {
        return LIB_UNI_SUR_PAIR;
    }
    return 0;
}

//// uni: is

bool lib_uni_is_not_char(int cp) {
    return _uni_check_not_char(cp) != 0;
}

bool lib_uni_is_char(int cp) {
    return _uni_check_not_char(cp) == 0;
}

bool lib_uni_is_sur(int cp) {
    return _uni_check_sur(cp) != 0;
}

int lib_uni_is_not_ffff(int cp) {
    return _uni_check_ffff(cp) == 0;
}

//// utf8

size_t lib_utf8_code_seq(int cp) {
    if (cp < 0) {
        return 0; /* error */
    }

    if (cp <= 0x7F) {
        // < 0x80
        // 0000 .. 007F
        // 0 .. 127
        // 0000 0000 .. 0111 1111
        // 1-byte ASCII
        return 1;
    } else if (cp <= 0x07FF) {
        // < 0x0800
        // 0080 .. 07FF
        // 128 .. 2 047
        // 0000 1000 0000 .. 0111 1111 1111
        // 2-byte unicode
        return 2;
    } else if (cp <= 0xFFFF) {
        // < 0x10000
        // 0800 .. FFFF
        // 2 048 .. 65 535
        // 1000 0000 0000 .. 1111 1111 1111 1111
        // 3-byte unicode
        return 3;
    } else if (cp <= 0x10FFFF) {
        // < 0x110000
        // 10000 .. 10FFFF
        // 65 536 .. 1 114 111
        // 1 0000 0000 0000 0000 .. 1 0000 1111 1111 1111 1111
        // 4-byte unicode
        return 4;
    }
    
    /* error */
    return 0;
}

// by array (strong)
//  1 000 000:  1.497s
// 10 000 000: 14.500s
size_t lib_utf8_byte_seq_array(char first) {
    uint8_t u = _u8(first);
    return LIB_UTF8_SEQ[u];
}

// by range (strong)
//  1 000 000:  1.043s
// 10 000 000: 10.000s
size_t lib_utf8_byte_seq_strong(char first) {
    uint8_t u = _u8(first);
    if (u <= 0x7F) {
        // 0x00 .. 0x7F
        return 1;
    } else if (u >= 0xC2 && u <= 0xDF) {
        // 0xC2 .. 0xDF
        return 2;
    } else if (u >= 0xE0 && u <= 0xEF) {
        // 0xE0 .. 0xEF
        return 3;
    } else if (u >= 0xF0 && u <= 0xF4) {
        // 0xF0 .. 0xF4
        return 4;
    }
    return 0;
}

// by range
//  1 000 000: 0.980s
// 10 000 000: 9.500s
size_t lib_utf8_byte_seq_range(char first) {
    uint8_t u = _u8(first);
    if (u <= 0x7F) {
        // 0x00 .. 0x7F
        return 1;
    } else if (u >= 0xC0 && u <= 0xDF) {
        // 0xC0 .. 0xDF
        // 0xC0 .. 0xC1 - no checking
        return 2;
    } else if (u >= 0xE0 && u <= 0xEF) {
        // 0xE0 .. 0xEF
        return 3;
    } else if (u >= 0xF0 && u <= 0xFF) {
        // 0xF0 .. 0xFF
        // 0xF5 .. 0xFF - no checking
        return 4;
    }
    return 0;
}

size_t lib_utf8_byte_seq_alt(char first) {
    uint8_t u = _u8(first);
    if (0xF0 == (0xF8 & u)) {
        return 4;
    } else if (0xE0 == (0xF0 & u)) {
        return 3;
    } else if (0xC0 == (0xE0 & u)) {
        return 2;
    }
    return 1; // TODO
}

////

size_t lib_utf8_byte_seq(char first) {
    return lib_utf8_byte_seq_array(first);
}

int lib_utf8_get_code(const char* str) {
    int cp;
    lib_utf8_to_code(str, &cp);
    return cp;
}

int lib_utf8_to_char(char* buf, int cp) {

    if (!buf) {
        return 0;
    }

    if (cp < 0) {
        _reset_buf(buf);
        return 0;
    }

    size_t len = lib_utf8_code_seq(cp);
    return _utf8_to_char(buf, cp, len);
}

size_t lib_utf8_get_char_len(const char* str) {
    if (!str || str[0] == '\0') {
        return 0;
    }
    return lib_utf8_byte_seq(str[0]);
}

/**
 * Get UTF-8 char info:
 * - codepoint
 * - lenght of UTF-8 char
 * 
 * Return:
 *   0: success
 *  -1: error
 */
int lib_utf8_get_char_info(const char* str, int* cp, int* len) {

    _reset_var(cp, len);

    if (!str) {
        return 0;
    }

    // Get sequence len of char by first byte 
    *len = lib_utf8_byte_seq(str[0]);
    if (*len == 0) {
        return -1;
    }

    // Convert UTF-8 char to codepoint
    *cp = _utf8_to_code(str, *len);
    if (*cp < 0)  {
        return -1;
    }

    return 0;
}

int lib_utf8_to_code(const char* str, int* cp) {

    _reset_val(cp);

    if (!str) {
        return 0;
    }

    int len;
    int err = lib_utf8_get_char_info(str, cp, &len);
    if (err != 0) {
        return -1;
    }
    return len;
}

////

const char* lib_utf8_strlast(const char* str) {
    if (!str) {
        return NULL;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return NULL;
    }
    const char* s = (const char*) str;
    s += len;
    s = lib_utf8_strprev(s);
    return s;
}

const char* lib_utf8_strnext(const char* str) {
    if (!str || str[0] == '\0') {
        return NULL;
    }

    // Get sequence len of char by first byte 
    size_t len = lib_utf8_byte_seq(str[0]);
    if (len == 0) {
        // error
        return NULL;
    }

    // Convert UTF-8 char to codepoint
    //int cp = _lib_utf8_to_code(str, len);
    //if (cp < 0)  {
    //    // error
    //    return NULL;
    //}

    const char* s = (const char*) str;
    s += len;

    return s;
}

const char* lib_utf8_strprev(const char* str) {
    //printf(">> strprev: start\n");
    if (!str) {
        return NULL;
    }
    char* s = (char*) str;

    s--;
    //printf(">> strprev: shift-1: %s\n", s);
    if (lib_utf8_stremp(s)) {
        //printf(">> strprev: error-1\n");
        return NULL;
    }
    //printf(">> strprev: uuuu\n");
    uint8_t u = _u8(s[0]);
    if (u <= 0x7F) {
        // < 0x80
        // 1-byte ASCII
        //printf(">> strprev: return: byte-1\n");
        return s;
    } else if (u < 0xC0) {
        // 10xxxxxx
        // 2,3,4 - byte unicode
        s--;
        //printf(">> strprev: shift-2: %s\n", s);
        if (lib_utf8_stremp(s)) {
            //printf(">> strprev: error-2\n");
            return NULL;
        }

        u = _u8(s[0]);
        if (u >= 0x80 && u < 0xC0) {
            // 10xxxxxx
            s--;
            //printf(">> strprev: shift-3: %s\n", s);
            if (lib_utf8_stremp(s)) {
                //printf(">> strprev: error-3\n");
                return NULL;
            }

            u = _u8(s[0]);
            if (u >= 0xE0 && u < 0xF0) {
                // 1110xxxx
                // 3 - byte unicode
                //printf(">> strprev: return: byte-3\n");
                return s;
            }

            if (u >= 0x80 && u < 0xC0) {
                // 10xxxxxx
                s--;
                //printf(">> strprev: shift-4: %s\n", s);
                if (lib_utf8_stremp(s)) {
                    //printf(">> strprev: error-4\n");
                    return NULL;
                }

                u = _u8(s[0]);
                if (u >= 0xF0 && u < 0xF8) {
                    // 11110xxx
                    // 4 - byte unicode
                    //printf(">> strprev: return: byte-4\n");
                    return s;
                }

            }

        } else if (u >= 0xC0 && u < 0xE0) {
            // 110xxxxx
            // 2 - byte unicode
            //printf(">> strprev: return: byte-2\n");
            return s;
        }

    } else {
        // error
        return NULL;
    }
    
    /* error */
    return 0;
}

const char* lib_utf8_iterate(const char* str, char* buf, int* cp, int* len) {

    _reset_var(cp, len);

    if (buf) {
        // reset buf always
        _reset_buf(buf);
    }

    if (!str) {
        // error
        return NULL;
    }

    if (str[0] == '\0') {
        *len = 0;
        return NULL;
    }

    int err = lib_utf8_get_char_info(str, cp, len);
    if (err != 0 || *cp < 0 || *len <= 0) {
        return NULL;
    }

    const char* s = (const char*) str;
    s += *len;

    // copy UTF-8 char from string to the buffer
    if (buf) {
        lib_utf8_chrcpy(buf, str, *len);
    }    

    return s;
}

const char* lib_utf8_next(const char* str, int* cp, int* len) {
    return lib_utf8_iterate(str, NULL, cp, len);
}

int lib_utf8_encode(char* buf, int cp) {
    return lib_utf8_to_char(buf, cp);
}

int lib_utf8_decode(const char* str, int* cp) {
    return lib_utf8_to_code(str, cp);
}

/*
 * Copy UTF-8 char from src to dst
 */
void lib_utf8_chrcpy(char* dst, const char* src, int len) {
    memcpy(dst, src, len);
}

/**
 * Find UTF-8 char by the index.
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char(const char* str, char* buf, int index) {
    return lib_utf8_get_char_n(str, (str ? strlen(str) : 0), buf, index);
}

/**
 * Find UTF-8 char by the index.
 * First numbers only. 
 * Store this char to the buffer.
 * The buffer must be array with min size: 4 + 1
 * Return codepoint of this char or error (-1).
 */
int lib_utf8_get_char_n(const char* str, size_t num, char* buf, int index) {
    if (!str || num == 0) {
        _reset_buf(buf);
        return 0;
    }

    if (!buf || index < 0 || index >= num) {
        _reset_buf(buf);
        // error: invalid arguments
        return -1;
    }

    char* s = (char*) str;

    int i = 0;   // index og byte
    int k = 0;   // index of UTF-8 char
    int cp = -1; // codepoint of UTF-8 char

    //printf(">> input index: %d\n", index);
    _reset_buf(buf);

    while (i < num) {

        //printf("   byte index: %d\n", i);
        //printf("   char index: %d\n", k);

        if (!s) {
            // why?
            return -1;
        }

        char c = *s;

        // Get lenght of UTF-8 char
        size_t len = lib_utf8_byte_seq(c);
        if (len == 0) {
            // error: invalid sequence lenght
            return -1;
        }

        if (k == index) {

            // Convert UTF-8 char to codepoint
            cp = _utf8_to_code(s, len);
            if (cp < 0) {
                // error: invalid codepoint
                return -1;
            }

            // Copy current UTF-8 char to the buffer
            lib_utf8_chrcpy(buf, s, len);
            buf[len] = '\0';

            return cp;
        }
        
        i += len;
        s += len;
        k++;
    }

    //printf(">> return\n");
    //printf("   byte index: %d\n", i);
    //printf("   char index: %d\n", k);

    // error: invalid index of byte (range)
    // maybe sequence lenght of UTF-8 char is incorrect
    return -1;
}

/**
 * Get current UTF-8 char from a string and move to next UTF-8 char.
 * Store this char to the buffer
 * The buffer must be array with min size: 4 + 1
 * Return pointer of next char or NULL if it is end of the string
 */
const char* lib_utf8_get_char_next(const char* str, char* buf) {
    int cp;
    int len;
    return lib_utf8_iterate(str, buf, &cp, &len);
}

///

int lib_utf8_get_code_count(const char* str) {
    if (!str) {
        return 0;
    }
    return lib_utf8_get_code_count_n(str, strlen(str));
}

int lib_utf8_get_code_count_n(const char* str, size_t num) {
    if (!str || num == 0) {
        return 0;
    }

    size_t i = 0;
    size_t count = 0;
    while (i < num) {
        char c = str[i];
        size_t len = lib_utf8_byte_seq(c);
        //if (len == 0) {
        //    return -1;
        //}
        i += len;
        count++;
    }
    return count;
}

int lib_utf8_get_first_byte_count_n(const char* str, size_t num, size_t char_num) {
    if (!str || num == 0 || char_num == 0) {
        return 0;
    }

    size_t i = 0;
    size_t count = 0;
    while (i < num) {
        char c = str[i];
        size_t len = lib_utf8_byte_seq(c);
        if (len == 0) {
            return -1;
        }
        i += len;
        count++;
        if (char_num == count) {
            return i;
        }
    }
    return i; // truncate?
}

int lib_utf8_get_first_byte_count(const char* str, size_t char_num) {
    if (!str) {
        return 0;
    }
    return lib_utf8_get_first_byte_count_n(str, strlen(str), char_num);
}

/**
 * Return count of bytes
 */
int lib_utf8_get_byte_count(const char* str) {
    return str ? strlen(str) : 0;
}

/**
 * Return count of UTF-8 chars or error (-1).
 */
int lib_utf8_get_char_count(const char* str) {
    return lib_utf8_get_code_count(str);
}

////

/*
 * Convert all chars of a string to lower/upper case
 * depends on mode: 
 *  1 - lower
 *  2 - upper
 * 
 * Return count of converted bytes or error (-1).
*/
int lib_utf8_to_case(int mode, char* str) {
    if (!str) {
        return 0;
    }

    char* s = (char*) str;

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    int cp1;                   // old codepoint
    int cp2;                   // new codepoint
    int len1;                  // lenght of old char
    int len2;                  // lenght of new char
    int i = 0;

    for (;;) {

        //printf("\n");
        //printf("to_case: i    = %i\n", i);

        if (!s || s[0] == '\0') {
            //printf("to_case: return\n");
            return i;
        }

        int cc = _utf8_to_case_char(mode, s, buf, &cp1, &len1, &cp2, &len2);
        if (cc < 0) {
            // error
            return -1;
        }

        // If old codepoint == new codepoint
        // then skip this position
        if (cp1 == cp2) {
            s += len1;
            i += len1;
            continue;
        }

        // If new lenght != old lenght
        // then skip this position
        // May be need to return error (-1)
        if (len1 != len2) {
            s += len1;
            i += len1;
            continue;
        }

        //printf("to_case: s    = %s\n", s);
        //printf("to_case: buf  = %s\n", buf);

        // Copy new UTF-8 char to 
        // old UTF-8 char from the buffer
        lib_utf8_chrcpy(s, buf, len1);
        i += len1;

    }

    return i;

}

int lib_utf8_to_case_code(int mode, int cp) {
    return lib_uni_to_case_code(mode, cp);
}

////

int lib_utf8_to_lower(char* str) {
    return lib_utf8_to_case(LIB_UNI_CASE_LOWER, str);
}

int lib_utf8_to_lower_char(const char* str, char* buf) {
    return lib_utf8_to_case_char(LIB_UNI_CASE_LOWER, str, buf);
}

int lib_utf8_to_case_char(int mode, const char* str, char* buf) {
    int cp1, len1, cp2, len2;
    return _utf8_to_case_char(mode, str, buf, &cp1, &len1, &cp2, &len2);
}

/*
 * Convert a codepoint to lower case.
 * Return new codepoint.
 */
int lib_utf8_to_lower_code(int cp) {
    return lib_uni_to_lower_code(cp);
}

////

int lib_utf8_to_upper(char* str) {
    return lib_utf8_to_case(LIB_UNI_CASE_UPPER, str);
}

int lib_utf8_to_upper_char(const char* str, char* buf) {
    return lib_utf8_to_case_char(LIB_UNI_CASE_UPPER, str, buf);
}

/*
 * Convert a codepoint to upper case
 * Return new codepoint.
 */
int lib_utf8_to_upper_code(int cp) {
    return lib_uni_to_upper_code(cp);
}

////

bool lib_utf8_is_utf8_valid(const char* str) {
    if (!str) {
        return true;
    }
    return lib_utf8_is_utf8_valid_n(str, strlen(str));
}

bool lib_utf8_is_utf8_valid_n(const char* str, size_t num) {
    if (!str) {
        return true;
    }
    int count = lib_utf8_get_code_count_n(str, num);
    return count >= 0;
}

bool lib_utf8_is_utf_valid_n(const char* str, size_t num) {
    if (!str) {
        return true;
    }
    int bom = lib_enc_get_bom_n(str, num);
    if (bom > 0) {
        return (bom == LIB_ENC_UTF8_BOM_ID 
        || bom == LIB_ENC_UTF16BE_BOM_ID
        || bom == LIB_ENC_UTF16LE_BOM_ID
        || bom == LIB_ENC_UTF32BE_BOM_ID
        || bom == LIB_ENC_UTF16LE_BOM_ID
        );
    }
    return lib_utf8_is_utf8_valid(str);   

    int count = lib_utf8_get_code_count(str);
    return count >= 0;
}

bool lib_utf8_is_ascii_char(char c) {
    uint8_t u = _u8(c);
    return (u <= 127);
}

bool lib_utf8_is_ascii(const char* str) {
    if (!str) {
        return true;
    }
    return lib_utf8_is_ascii_n(str, strlen(str));
}

bool lib_utf8_is_ascii_n(const char* str, size_t num) {
    if (!str || num <= 0) {
        return true;
    }
    int bom = lib_enc_get_bom_n(str, num);
    if (bom > 0) {
        return false;
    }
    for (size_t i = 0; i < num; i++) {
        if (!lib_utf8_is_ascii_char(str[i])) {
            return false;
        }
    }
    return true;   
}

bool lib_utf8_is_utf8(const char* str) {
    if (!str) {
        return true;
    }
    return lib_utf8_is_utf8_n(str, strlen(str));
}

bool lib_utf8_is_utf8_n(const char* str, size_t num) {
    if (!str || num <= 0) {
        return true;
    }
    int bom = lib_enc_get_bom_n(str, num);
    if (bom > 0) {
        return (bom == LIB_ENC_UTF8_BOM_ID);
    }
    return lib_utf8_is_utf8_valid(str);   
}

/*
 * Return byte order mark (BOM) of a string.
 */
int lib_utf8_get_bom_n(const char* str, size_t num) {
    return lib_enc_get_bom_n(str, num);
}

const char* lib_utf8_to_bom_str(int bom) {
    return lib_enc_to_bom_str(bom);
}

////

/**
 * Convert UTF-8 char to the codepoint
 */
static int _utf8_to_code(const char* str, int len) {

    // Get first byte
    uint8_t c = _u8(str[0]);

    if (len == 1) {

        /* 1 byte utf8 codepoint */
        return c;
    } else if (len == 2) {

        /* 2 byte utf8 codepoint */
        uint8_t d = _u8(str[1]);

        if (d < 0x80 || d > 0xBF) {
            return LIB_UTF8_BAD_BYTE;
	    }
        if (c <= 0xC1) {
            return LIB_UTF8_BAD_BYTE;
	    }
        return ((0x1F & c) << 6) | (0x3F & d);
    } else if (len == 3) {

        /* 3 byte utf8 codepoint */
        uint8_t d = _u8(str[1]);
        uint8_t e = _u8(str[2]);

        if (d < 0x80 || d > 0xBF || e < 0x80 || e > 0xBF) {
            return LIB_UTF8_BAD_BYTE;
	    }
	    if (c == 0xE0 && d < 0xA0) { 
            /* We don't need to check the value of str[2], because
            the if statement above this one already guarantees that
            it is 10xxxxxx. 
            */
           return LIB_UTF8_BAD_BYTE;
	    }

        int r = ((0x0F & c) << 12) | ((0x3F & d) << 6) | (0x3F & e);

        // Check: surrogate
        int err = _uni_check_sur(r);
        if (err != 0) {
            return err;
        }

        // Check: FFFF
        err = _uni_check_ffff(r);
        if (err != 0) {
            return err;
        }

        // Check: not char
        err = _uni_check_not_char(r);
        if (err != 0) {
            return err;
        }

        return r;

    } else if (len == 4) {

        /* 4 byte utf8 codepoint */
        uint8_t d = _u8(str[1]);
        uint8_t e = _u8(str[2]);
        uint8_t f = _u8(str[3]);

        if (/* c must be 11110xxx. */
            c >= 0xF8 ||
            /* d, e, f must be 10xxxxxx. */
            d < 0x80 || d >= 0xC0 ||
            e < 0x80 || e >= 0xC0 ||
            f < 0x80 || f >= 0xC0) {
                return LIB_UTF8_BAD_BYTE;
	    }

        if (c == 0xF0 && d < 0x90) { 
            /* We don't need to check the values of e and f, because
             the if statement above this one already guarantees that
             e and f are 10xxxxxx. */
            return LIB_UTF8_BAD_BYTE;
	    }

        int r = ((0x07 & c) << 18) | ((0x3F & d) << 12) | ((0x3F & e) << 6) | (0x3F & f);

        if (r > LIB_UNI_MAX) {
            return LIB_UNI_TOO_BIG;
	    }

        /* Non-characters U+nFFFE..U+nFFFF on plane 1-16 */

        int err = _uni_check_ffff(c);
        if (err != 0) {
            return err;
        }

        /* 
        We don't need to check for surrogate pairs here, since the
        minimum value of UCS-2 if there are four bytes of UTF-8 is 0x10000. 
        */

        return r;

    }

    // error: invalid lenght
    return -1;
}

/**
 * Convert the codepoint to UTF-8 char.
 * Store the result ot the buffer.
 */
static int _utf8_to_char(char* buf, int cp, int len) {

    //if (cp <= 0x7F) {
    if (len == 1) {
        // 1-byte ASCII
        buf[0] = (char) cp;
        buf[1] = '\0';
        return 1;
    //} else if (cp <= 0x07FF) {
    } else if (len == 2) {
        // 2-byte unicode
        buf[0] = (char) (((cp >> 6) & 0x1F) | 0xC0);
        buf[1] = (char) (((cp >> 0) & 0x3F) | 0x80);
        buf[2] = '\0';
        return 2;
    //} else if (cp <= 0xFFFF) {
    } else if (len == 3) {
        // 3-byte unicode
        buf[0] = (char) (((cp >> 12) & 0x0F) | 0xE0); /* 0x0F */
        buf[1] = (char) (((cp >> 6) & 0x3F) | 0x80);
        buf[2] = (char) (((cp >> 0) & 0x3F) | 0x80);
        buf[3] = '\0';

        // Check: surrogate
        int err =  _uni_check_sur(cp);
        if (err != 0) {
            return err;
        }

        // Check: not char
        err =  _uni_check_not_char(cp);
        if (err != 0) {
            return err;
        }

        return 3;
    //} else if (cp <= 0x10FFFF) {
    } else if (len == 4) {
        // 4-byte unicode
        buf[0] = (char) (((cp >> 18) & 0x07) | 0xF0); /* 0x07 */
        buf[1] = (char) (((cp >> 12) & 0x3F) | 0x80);
        buf[2] = (char) (((cp >> 6) & 0x3F) | 0x80);
        buf[3] = (char) (((cp >> 0) & 0x3F) | 0x80);
        buf[4] = '\0';
        return 4;
    }
    
    /* error - use replacement character: 0xFFFD */
    buf[0] = (char) 0xEF;
    buf[1] = (char) 0xBF;
    buf[2] = (char) 0xBD;
    buf[3] = '\0';
    return -1;
}

static int _utf8_to_case_char(
    int mode, const char* str, char* buf,
    int* cp1, int* len1, int* cp2, int* len2) {

    _reset_var(cp1, len1);
    _reset_var(cp2, len2);
    _reset_buf(buf);

    int err = lib_utf8_get_char_info(str, cp1, len1);
    if (err != 0) {
        return err;
    }

    // Convert old codepoint to new codepoint
    // lower/upper case depends on the mode
    *cp2 = lib_utf8_to_case_code(mode, *cp1);

    // If new codepoint is not correct
    // then return error (-1)
    if (*cp2 < 0) {
        return -1;
    }

    // If old codepoint == new codepoint
    // then shift a position and skip
    if (*cp1 == *cp2) {
        *len2 = *len1;
        return 0;
    }

    // Convert new codepoint to new char 
    // (min size = 4 + 1)
    //*len2 = lib_utf8_to_char(buf, *cp2);

    *len2 = lib_utf8_code_seq(*cp2);
    if (buf) {
        // important: when len < 0 because 
        // it set special/invalid value to the buffer
        _utf8_to_char(buf, *cp2, *len2);
    }
   
    //printf("to_case: len2 = %i\n", len2);

    // If new lenght is not correct
    // then return error (-1)
    if (*len2 <= 0) {
        return -1;
    }

    return *len1; // count of converted bytes
}

void _reset_var(int* var1, int* var2) {
    if (var1) {
        *var1  = -1;
    }
    if (var2) {
        *var2 = -1;
    }        
}

void _reset_val(int* val) {
    if (val) {
        *val = -1;
    }        
}

void _reset_buf(char* buf) {
    if (buf) {
        buf[0] = '\0';
    }    
}

//// std: begin ////

// strlen

size_t lib_utf8_strlen(const char* str) {
    int len = lib_utf8_get_char_count(str);
    // For compatibility with std::strlen
    return len < 0 ? 0 : len;
}

size_t lib_utf8_strnlen(const char* str, size_t num) {
    // For compatibility with std::strnlen
    // Calculate count of first bytes by UTF-8 char numbers
    int len = lib_utf8_get_first_byte_count(str, num);
    return len < 0 ? 0 : len;
}

// strcpy

char* lib_utf8_strcpy(char* dst, const char* src) {
    if (!dst || !src) {
        return dst;
    }
    return strcpy(dst, src);
}

char* lib_utf8_strncpy(char* dst, const char* src, size_t num) {
    if (!dst || !src) {
        return dst;
    }

    // Calculate count of first bytes by UTF-8 char numbers
    int count = lib_utf8_get_first_byte_count(src, num);
    if (count <= 0) {
        return dst;
    }
    return strncpy(dst, src, count);
}

// strcat

char* lib_utf8_strcat(char* dst, const char* src) {
    if (!dst || !src) {
        return dst;
    }
    return strcat(dst, src);
}

char* lib_utf8_strncat(char* dst, const char* src, size_t num) {
    if (!dst || !src || num == 0) {
        return dst;
    }

    // Calculate count of first bytes by UTF-8 char numbers
    int count = lib_utf8_get_first_byte_count(src, num);
    if (count <= 0) {
        return dst;
    }
    return strncat(dst, src, count);
}

// strcmp

int lib_utf8_strcmp(const char* str1, const char* str2) {
    if (!str1 && !str2) {
        return 0;
    }
    if (!str1) {
        return -1;
    }
    if (!str2) {
        return 1;
    }

    // TODO: Stub
    // Compare codepoints: char_next/char_iterate

    return strcmp(str1, str2);
}

int lib_utf8_strncmp(const char* str1, const char* str2, size_t num) {
    if (!str1 && !str2) {
        return 0;
    }
    if (!str1) {
        return -1;
    }
    if (!str2) {
        return 1;
    }

    // TODO: Stub
    // Compare codepoints: char_next/char_iterate

    // Calculate count of first bytes by UTF-8 char numbers
    int count1 = lib_utf8_get_first_byte_count(str1, num);
    int count2 = lib_utf8_get_first_byte_count(str2, num);

    if (count1 < 0 || count2 < 0 || count1 < num || count2 < num) {
        // error
        return -100;
    }

    if (count1 < count2) {
        return -1;
    }
    if (count1 > count2) {
        return 1;
    }

    return strncmp(str1, str2, count1);

}

// strchr

char* lib_utf8_strchr(const char* str, int ch) {
    // TODO: Not implemented yet
    // Search by codepoint: char_next/char_iterate
    return NULL;
}

char* lib_utf8_strrchr(const char* str, int ch) {
    // TODO: Not implemented yet
    // Search by codepoint: char_next/char_iterate
    return NULL;
}

// strstr

const char* lib_utf8_strstr(const char* str1, const char* str2) {
    if (!str1) {
        return NULL;
    }
    if (!str2) {
        // why? maybe str1
        return NULL;
    }
    // TODO: Stub
    // Search by codepoint: char_next/char_iterate
    return strstr(str1, str2);
}

// strlwr/upr

char* lib_utf8_strlwr(char* str) {
    // Ignore case error
    lib_utf8_to_lower(str);
    return str;
}

char* lib_utf8_strupr(char* str) {
    // Ignore case error
    lib_utf8_to_upper(str);
    return str;
}

// strrev

char* lib_utf8_strrev(char* str) {
    if (!str || str[0] == '\0') {
        return str;
    }
    size_t len = strlen(str);
    //printf("\n");
    //printf("!>> len: %lu\n", len);
    if (len == 0) {
        return str;
    }
    char tmp[len];
    // char tmp[len + 1];
    // tmp[len] = '\0';
    size_t i = 0;
    size_t pos = len - 1;
    size_t chl = 0;
    char* s = (char*) str;
    while (i < len) {
        chl = lib_utf8_byte_seq(*s);
        if (chl == 0) {
            chl = 1;
        }
        //printf("!>> chl: %lu\n", chl);
        //printf("!>> pos: %lu\n", pos);
        size_t j = pos;
        for (size_t k = 0; k < chl; k++) {
            size_t z = j - k;
            tmp[z] = str[i + chl - k - 1];
        }
        s += chl;
        i += chl;
        pos -= chl; 
    }
    //printf("!>>%s", tmp);
    memcpy(str, tmp, len);
    return str;
}

// strtok

char* lib_utf8_strtok(char* str, const char* delims) {
    if (!str || !delims) {
        return NULL;
    }
    // TODO: What about UTF-8 char delims?
    // const int* delims

    // TODO: Stub
    // Tokenize by codepoint: char_next/char_iterate

    return strtok(str, delims);
}

//// std: end   ////

//// alt: begin ////

bool lib_utf8_stremp(const char* str) {
    return (!str || str[0] == '\0');
}

// equals by byte
bool lib_utf8_streq(const char* str1, const char* str2) {
    if (!str1 || !str2) {
        return false;
    }
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    if (len1 != len2) {
        return false;
    }
    char* s1 = (char*) str1;
    char* s2 = (char*) str2;

    size_t i = 0;
    while (i < len1) {
        if (*s1 != *s2) {
            return false;
        }
        s1++;
        s2++;
        i++;
    }
    return true;
}

// equals by codepoint
bool lib_utf8_strceq(const char* str1, const char* str2) {
    if (!str1 || !str2) {
        return false;
    }
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);
    if (len1 != len2) {
        return false;
    }
    char* s1 = (char*) str1;
    char* s2 = (char*) str2;

    int cp1;
    int l1;
    int cp2;
    int l2;

    size_t i = 0;
    while (i < len1) {

        l1 = lib_utf8_to_code(s1, &cp1);
        if (l1 <= 0 || cp1 < 0) {
            // error
            return false;
        }

        l2 = lib_utf8_to_code(s2, &cp2);
        if (l2 <= 0 || cp2 < 0) {
            // error
            return false;
        }

        if (l1 != l2 || cp1 != cp2) {
            return false;
        }

        s1 += l1;
        s2 += l1;
        i += l1;
    }
    return true;
}

// equals by ignore case codepoint
// what about unicode compare?
bool lib_utf8_strieq(const char* str1, const char* str2) {
    if (!str1 || !str2) {
        return false;
    }
    size_t len1 = strlen(str1);
    size_t len2 = strlen(str2);

    // TODO: Why?
    if (len1 != len2) {
        return false;
    }

    char* s1 = (char*) str1;
    char* s2 = (char*) str2;

    int cp1;
    int l1;
    int cp2;
    int l2;

    size_t i = 0;
    //size_t j = 0; // TODO
    while (i < len1 /*&& j < len2*/) {

        l1 = lib_utf8_to_code(s1, &cp1);
        if (l1 <= 0 || cp1 < 0) {
            // error
            return false;
        }
        
        l2 = lib_utf8_to_code(s2, &cp2);
        if (l2 <= 0 || cp2 < 0) {
            // error
            return false;
        }

        // TODO: Why?
        if (l1 != l2) {
            return false;
        }
        
        cp1 = lib_utf8_to_case_code(1, cp1);
        if (cp1 < 0) {
            // error
            return false;
        }        

        cp2 = lib_utf8_to_case_code(1, cp2);
        if (cp2 < 0) {
            // error
            return false;
        }        

        if (cp1 != cp2) {
            return false;
        }

        s1 += l1;
        s2 += l1;
        //s2 += l2; // TODO
        i += l1;
        //j += l2;  // TODO
    }
    return true;
}

//// alt: end ////

// https://stackoverflow.com/questions/43564445/how-to-map-unicode-codepoints-from-an-utf-16-file-using-c

//// UTF-16

static uint16_t _u16(uint8_t b1, uint8_t b2) {
    return (b1 << 8) | b2;
}

static int _utf16_char_seq(bool be, const char* str) {
    if (!str) {
        return 0;
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

int lib_utf16_char_seq(const char* str) {
    return lib_utf16be_char_seq(str);
}

int lib_utf16be_char_seq(const char* str) {
    return _utf16_char_seq(true, str);
}

int lib_utf16le_char_seq(const char* str) {
    return _utf16_char_seq(false, str);
}

static int _utf16_to_code(bool be, const char* str, int* cp) {
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
    uint8_t b1  = _u8(str[i1]);
    uint8_t b2  = _u8(str[i2]);
    uint16_t c1 = _u16(b1, b2);

    if (c1 >= 0xD800 && c1 < 0xDC00) {
        uint8_t b3  = _u8(str[i3]);
        uint8_t b4  = _u8(str[i4]);
        uint16_t c2 = _u16(b3, b4);
        *cp = ((c1 & 0x3FF) << 10) + (c2 & 0x3FF) + 0x10000;
        //return 0;
        return 4;
    }
    *cp = c1;
    //return 0;
    return 2;
}

int lib_utf16_to_code(const char* str, int* cp) {
    return _utf16_to_code(true, str, cp);
}

int lib_utf16be_to_code(const char* str, int* cp) {
    return _utf16_to_code(true, str, cp);
}

int lib_utf16le_to_code(const char* str, int* cp) {
    return _utf16_to_code(false, str, cp);
}

size_t lib_utf16_code_seq(int cp) {
    if (cp < 0 || cp > 0x10FFFF) {
        return 0;
    }
    if(cp < 0xD800 || (cp > 0xDFFF && cp < 0x10000)) {
        return 2;
    }
    return 4;
}

/**
 * Convert the codepoint to UTF-16 char.
 * Store the result ot the buffer.
 */
static int _utf16_to_char(bool be, char* buf, int cp) {
    if (cp < 0 || cp > 0x10FFFF) {
        //return -1;
        return 0;
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
        //return 0;
        return 2;
    }

    cp -= 0x010000;

    uint16_t hi = (cp >> 10) + 0xD800;
    uint16_t lo = (cp & 0x03FF) + 0xDC00;

    buf[i1] = (hi >> 8) & 0xFF;
    buf[i2] = hi & 0xFF;
    buf[i3] = (lo >> 8) & 0xFF;;
    buf[i4] = lo & 0xFF;;

    //return 0;
    return 4;
}

//// UTF-32

int lib_utf32_char_seq(const char* str) {
    if (!str) {
        return 0;
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

size_t lib_utf32_code_seq(int cp) {
    if (cp < 0 || cp > 0x10FFFF) {
        return 0;
    }
    return 4;
}

/**
 * Convert the codepoint to UTF-32 char.
 * Store the result ot the buffer.
 */
int _utf32_to_char(bool be, char* buf, int cp) {
    if (cp < 0 || cp > 0x10FFFF) {
        //return -1;
        return 0;
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

int lib_utf16_to_char(char* buf, int cp) {
   return lib_utf16be_to_char(buf, cp);
}

int lib_utf16be_to_char(char* buf, int cp) {
   return _utf16_to_char(true, buf, cp);
}

int lib_utf16le_to_char(char* buf, int cp) {
   return _utf16_to_char(false, buf, cp);
}

////

int lib_utf32_to_code(const char* str, int* cp) {
    return lib_utf32be_to_code(str, cp);
}

int lib_utf32be_to_code(const char* str, int* cp) {
    return _utf32_to_code(true, str, cp);
}

int lib_utf32le_to_code(const char* str, int* cp) {
    return _utf32_to_code(false, str, cp);
}

//

int lib_utf32_to_char(char* buf, int cp) {
   return lib_utf32be_to_char(buf, cp);
}

int lib_utf32be_to_char(char* buf, int cp) {
   return _utf32_to_char(true, buf, cp);
}

int lib_utf32le_to_char(char* buf, int cp) {
   return _utf32_to_char(false, buf, cp);
}

//// UTF

size_t lib_utf_char_seq(int enc_id, const char* str) {
    if (!str /*|| str[0] == '\0'*/) {
        return 0;
    }

    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID || enc_id == LIB_ENC_UTF8_BOM_ID) {
        if (str[0] == '\0') {
            return 0;
        }
        return lib_utf8_byte_seq(str[0]);
    }

    // UTF-16
    if (enc_id == LIB_ENC_UTF16_ID) {
        return lib_utf16_char_seq(str);
    }
    if (enc_id == LIB_ENC_UTF16BE_ID 
     || enc_id == LIB_ENC_UTF16BE_BOM_ID) {
        return lib_utf16be_char_seq(str);
    }
    if (enc_id == LIB_ENC_UTF16LE_ID
     || enc_id == LIB_ENC_UTF16LE_BOM_ID) {
        return lib_utf16le_char_seq(str);
    }

    // UTF-32  (BE/LE)
    if (enc_id == LIB_ENC_UTF32_ID 
     || enc_id == LIB_ENC_UTF32BE_ID 
     || enc_id == LIB_ENC_UTF32LE_ID
     || enc_id == LIB_ENC_UTF32BE_BOM_ID
     || enc_id == LIB_ENC_UTF32LE_BOM_ID
     ) {
        return lib_utf32_char_seq(str);
    }

    return 0;
}

size_t lib_utf_code_seq(int enc_id, int cp) {

    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID 
     || enc_id == LIB_ENC_UTF8_BOM_ID) {
        return lib_utf8_code_seq(cp);
    }

    // UTF-16 (BE/LE)
    if (enc_id == LIB_ENC_UTF16_ID 
     || enc_id == LIB_ENC_UTF16BE_ID 
     || enc_id == LIB_ENC_UTF16LE_ID
     || enc_id == LIB_ENC_UTF16BE_BOM_ID 
     || enc_id == LIB_ENC_UTF16LE_BOM_ID

     ) {
        return lib_utf16_code_seq(cp);
    }

    // UTF-32 (BE/LE)
    if (enc_id == LIB_ENC_UTF32_ID 
     || enc_id == LIB_ENC_UTF32BE_ID 
     || enc_id == LIB_ENC_UTF32LE_ID
     || enc_id == LIB_ENC_UTF32BE_BOM_ID 
     || enc_id == LIB_ENC_UTF32LE_BOM_ID
     ) {
        return lib_utf32_code_seq(cp);
    }

    return 0;
}

/*
 * Convert a codepount to a char and return lenght of the char.
 */
int lib_utf_to_char(int enc_id, char* buf, int cp) {

    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID || enc_id == LIB_ENC_UTF8_BOM_ID) {
        return lib_utf8_to_char(buf, cp);
    }

    // UTF-16
    if (enc_id == LIB_ENC_UTF16_ID) {
        return lib_utf16_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF16BE_ID) {
        return lib_utf16be_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF16LE_ID) {
        return lib_utf16le_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF16BE_BOM_ID) {
        return lib_utf16be_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF16LE_BOM_ID) {
        return lib_utf16le_to_char(buf, cp);
    }

    // UTF-32
    if (enc_id == LIB_ENC_UTF32_ID) {
        return lib_utf32_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF32BE_ID) {
        return lib_utf32be_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF32LE_ID) {
        return lib_utf32le_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF32BE_BOM_ID) {
        return lib_utf32be_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UTF32LE_BOM_ID) {
        return lib_utf32le_to_char(buf, cp);
    }

    return -1;
}

/*
 * Convert a char to a codepount and return lenght of the char.
 */
int lib_utf_to_code(int enc_id, const char* str, int* cp) {
    
    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID || enc_id == LIB_ENC_UTF8_BOM_ID) {
        return lib_utf8_to_code(str, cp);
    }

    // UTF-16
    if (enc_id == LIB_ENC_UTF16_ID) {
        return lib_utf16_to_code(str, cp);
    }
    if (enc_id == LIB_ENC_UTF16BE_ID) {
        return lib_utf16be_to_code(str, cp);
    }
    if (enc_id == LIB_ENC_UTF16LE_ID) {
        return lib_utf16le_to_code(str, cp);
    }

    // UTF-32
    if (enc_id == LIB_ENC_UTF32_ID) {
        return lib_utf32_to_code(str, cp);
    }
    if (enc_id == LIB_ENC_UTF32BE_ID) {
        return lib_utf32be_to_code(str, cp);
    }
    if (enc_id == LIB_ENC_UTF32LE_ID) {
        return lib_utf32le_to_code(str, cp);
    }

    return -1;
}

/*

https://github.com/benkasminbullock/unicode-c/blob/master/unicode.c

https://dev.to/rdentato/utf-8-strings-in-c-1-3-42a4

https://doc.cat-v.org/bell_labs/utf-8_history

https://www.martinpickering.com/posts/unicode-in-c/

*/