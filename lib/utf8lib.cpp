#include <string.h>
#include <stdint.h>
//#include <stdio.h>

#include "bomlib.h"
//#include "enclib.h"

#include "unilib.h"
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

static const int LIB_UTF8_SEQUENCE_LEN[256] = {
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

#define LIB_UTF_SUR_HIGH_START  0xD800
#define LIB_UTF_SUR_HIGH_END    0xDBFF
#define LIB_UTF_SUR_LOW_START   0xDC00
#define LIB_UTF_SUR_LOW_END     0xDFFF

bool lib_utf_is_codepoint_surrogate(int cp) {
    return cp >= LIB_UTF_SUR_HIGH_START && cp <= LIB_UTF_SUR_LOW_END;
}

////

int lib_utf8_get_codepoint_sequence_len(int cp) {
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
int lib_utf8_get_byte_sequence_len_array(char first) {
    unsigned char u = (unsigned char) first;
    return LIB_UTF8_SEQUENCE_LEN[u];
}

// by range (strong)
//  1 000 000:  1.043s
// 10 000 000: 10.000s
int lib_utf8_get_byte_sequence_len_strong(char first) {
    unsigned char u = (unsigned char) first;
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
int lib_utf8_get_byte_sequence_len_range(char first) {
    unsigned char u = (unsigned char) first;
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

int lib_utf8_get_byte_sequence_len_alt(char first) {
    unsigned char u = (unsigned char) first;
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

int lib_utf8_get_byte_sequence_len(char first) {
    return lib_utf8_get_byte_sequence_len_array(first);
}

int lib_utf8_get_codepoint_by_len(const char* ch, int len) {
    if (!ch || len <= 0) {
        return -1;
    }

    if (len == 1) {
        /* 1 byte utf8 codepoint */
        return ch[0];
    } else if (len == 2) {
        /* 2 byte utf8 codepoint */
        return ((0x1F & ch[0]) << 6) | (0x3F & ch[1]);
    } else if (len == 3) {
        /* 3 byte utf8 codepoint */
        return ((0x0F & ch[0]) << 12) | ((0x3F & ch[1]) << 6) | (0x3F & ch[2]);
        //str += 3;
    } else if (len == 4) {
        /* 4 byte utf8 codepoint */
        return ((0x07 & ch[0]) << 18) | ((0x3F & ch[1]) << 12) |
                     ((0x3F & ch[2]) << 6) | (0x3F & ch[3]);
    }

    return -1;
}

int lib_utf8_get_codepoint(const char* ch) {
    int cp = -1;
    lib_utf8_to_codepoint(ch, &cp);
    return cp;
}

int lib_utf8_to_utf8(char* ch, int cp) {

    if (!ch || cp < 0) {
        ch[0] = '\0';
        return 0;
    }

    int len = lib_utf8_get_codepoint_sequence_len(cp);

    //if (cp <= 0x7F) {
    if (len == 1) {
        // 1-byte ASCII
        ch[0] = (char) cp;
        ch[1] = '\0';
        return 1;
    //} else if (cp <= 0x07FF) {
    } else if (len == 2) {
        // 2-byte unicode
        ch[0] = (char) (((cp >> 6) & 0x1F) | 0xC0);
        ch[1] = (char) (((cp >> 0) & 0x3F) | 0x80);
        ch[2] = '\0';
        return 2;
    //} else if (cp <= 0xFFFF) {
    } else if (len == 3) {
        // 3-byte unicode
        ch[0] = (char) (((cp >> 12) & 0x0F) | 0xE0); /* 0x0F */
        ch[1] = (char) (((cp >> 6) & 0x3F) | 0x80);
        ch[2] = (char) (((cp >> 0) & 0x3F) | 0x80);
        ch[3] = '\0';
        return 3;
    //} else if (cp <= 0x10FFFF) {
    } else if (len == 4) {
        // 4-byte unicode
        ch[0] = (char) (((cp >> 18) & 0x07) | 0xF0); /* 0x07 */
        ch[1] = (char) (((cp >> 12) & 0x3F) | 0x80);
        ch[2] = (char) (((cp >> 6) & 0x3F) | 0x80);
        ch[3] = (char) (((cp >> 0) & 0x3F) | 0x80);
        ch[4] = '\0';
        return 4;
    }
    
    /* error - use replacement character: 0xFFFD */
    ch[0] = (char) 0xEF;
    ch[1] = (char) 0xBF;
    ch[2] = (char) 0xBD;
    ch[3] = '\0';
    return -1;
}

int lib_utf8_to_codepoint(const char* ch, int* cp) {

    if (!ch) {
        return 0;
    }

    // Get sequence len of char by first byte 
    int len = lib_utf8_get_byte_sequence_len(ch[0]);
    if (len <= 0) {
        return -1;
    }

    // Get codepoint by char with len
    *cp = lib_utf8_get_codepoint_by_len(ch, len);
    if (*cp < 0)  {
        return -1;
    }

    return len;
}

const char* lib_utf8_to_codepoint_next(const char* str, int* cp, int* len) {

    //const char* s = (const char*) str;
    *cp = -1;
    *len = -1;

    if (!str) {
        return NULL; //s;
    }
    int char_len = lib_utf8_get_byte_sequence_len(str[0]);
    if (char_len <= 0) {
        return NULL; //s;
    }

    *cp = lib_utf8_get_codepoint_by_len(str, char_len);
    if (*cp < 0)  {
        return NULL; //s;
    }

    const char* s = (const char*) str;
    s += char_len;
    return s;
}

int lib_utf8_encode(char* ch, int cp) {
    return lib_utf8_to_utf8(ch, cp);
}

int lib_utf8_decode(const char* ch, int* cp) {
    return lib_utf8_to_codepoint(ch, cp);
}

//// str

int lib_utf8_get_str_len(const char* str) {
    return lib_utf8_get_codepoint_count(str);
}

int lib_utf8_get_str_len_n(const char* str, int len) {
    return lib_utf8_get_codepoint_count_n(str, len);
}

////

int lib_utf8_get_codepoint_count(const char* str) {
    if (!str) {
        return 0;
    }
    return lib_utf8_get_codepoint_count_n(str, strlen(str));
}

int lib_utf8_get_codepoint_count_n(const char* str, int len) {
    if (!str || len <= 0) {
        return 0;
    }

    int i = 0;
    while (i < len) {
        char c = str[i];
        int char_len = lib_utf8_get_byte_sequence_len(c);
        if (char_len <= 0) {
            return -1;
        }
        i += char_len;
    }
    return i;
}

////

// int lib_utf8_to_lower_codepoint(int cp) {
//     return lib_uni_to_lower_codepoint(cp);
// }

// int lib_utf8_to_upper_codepoint(int cp) {
//     return lib_uni_to_upper_codepoint(cp);    
// }

// static int _lib_utf8_to_case_codepoint(int mode, int cp) {
//     if (mode == 1) {
//         return lib_uni_to_lower_codepoint(cp);
//     } else {
//         return lib_uni_to_upper_codepoint(cp);
//     }
// }

static int _lib_utf8_to_case(int mode, const char* str) {
    if (!str) {
        return 0;
    }
    char* s = (char*) str;
    char buf[] = "\0\0\0\0\0";
    int cp1 = -1;
    int cp2 = -1;
    int len1 = -1;
    int len2 = -1;
    int i = 0;
    for (;;) {

        //printf("\n");
        //printf("to_case: i    = %i\n", i);

        if (!s || s[0] == '\0') {
            //printf("to_case: return\n");
            return i;
        }

        cp1 = -1;
        cp2 = -1;
        len1 = -1;
        len2 = -1;

        len1 = lib_utf8_to_codepoint(s, &cp1);
        //printf("to_case: len1 = %i\n", len1);
        //printf("to_case: cp1  = %i, 0x%.4X\n", cp1, cp1);
        if (cp1 < 0 || len1 <= 0) {
            return -1;
        }
        cp2 = lib_uni_to_case_codepoint(mode, cp1);
        //printf("to_case: cp2  = %i, 0x%.4X\n", cp2, cp2);

        if (cp2 < 0) {
            return -1;
        }

        if (cp1 == cp2) {
            s += len1;
            i += len1;
            continue;
        }

        len2 = lib_utf8_to_utf8(buf, cp2);
        //printf("to_case: len2 = %i\n", len2);

        if (len2 <= 0) {
            return -1;
        }
        if (len1 != len2) {
            return -1;
        }

        //printf("to_case: s    = %s\n", s);
        //printf("to_case: buf  = %s\n", buf);

        for (int k = 0; k < len1; k++) {
            //str[i + k] = buf[k];
            //s[k] = buf[k];

            *s = buf[k];
            s += 1;
        }

        //s += len1;
        i += len1;

    }

    return i;

}

////

int lib_utf8_to_lower(const char* str) {
    //printf("to_lower\n");
    return _lib_utf8_to_case(1, str);
}

int lib_utf8_to_upper(const char* str) {
    //printf("to_upper\n");
    return _lib_utf8_to_case(2, str);
}

////

bool lib_utf8_is_utf8_valid(const char* str) {
    if (!str) {
        return true;
    }
    return lib_utf8_is_utf8_valid_n(str, strlen(str));
}

bool lib_utf8_is_utf8_valid_n(const char* str, int len) {
    if (!str) {
        return true;
    }
    int count = lib_utf8_get_codepoint_count_n(str, len);
    return count >= 0;
}

bool lib_utf8_is_utf_valid_n(const char* str, int len) {
    if (!str) {
        return true;
    }
    int bom = lib_bom_get_bom_n(str, len);
    if (bom > 0) {
        return (bom == LIB_BOM_UTF8 
        || bom == LIB_BOM_UTF16_BE
        || bom == LIB_BOM_UTF16_LE
        || bom == LIB_BOM_UTF32_BE
        || bom == LIB_BOM_UTF16_LE
        );
    }
    return lib_utf8_is_utf8_valid(str);   

    int count = lib_utf8_get_codepoint_count(str);
    return count >= 0;
}

bool lib_utf8_is_ascii_char(char c) {
    unsigned char u = (unsigned char) c;
    return (u <= 127);
}

bool lib_utf8_is_ascii(const char* str) {
    if (!str) {
        return true;
    }
    return lib_utf8_is_ascii_n(str, strlen(str));
}

bool lib_utf8_is_ascii_n(const char* str, int len) {
    if (!str || len <= 0) {
        return true;
    }
    int bom = lib_bom_get_bom_n(str, len);
    if (bom > 0) {
        return false;
    }
    unsigned char u;
    for (int i = 0; i < len; i++) {
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

bool lib_utf8_is_utf8_n(const char* str, int len) {
    if (!str || len <= 0) {
        return true;
    }
    int bom = lib_bom_get_bom_n(str, len);
    if (bom > 0) {
        return (bom == LIB_BOM_UTF8);
    }
    return lib_utf8_is_utf8_valid(str);   
}

/*
 * Return byte order mark (BOM) by string
 */
int lib_utf8_get_bom_n(const char* str, int len) {
    return lib_bom_get_bom_n(str, len);
}

const char* lib_utf8_to_bom_str(int bom) {
    return lib_bom_to_bom_str(bom);
}
