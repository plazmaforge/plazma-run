#include <string.h>
#include <stdint.h>

#include "utf8lib.h"

// https://tools.ietf.org/html/rfc3629

static const int LIB_UTF8_SEQUENCE_LEN[256] = {
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0: 0x00-0x0F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 1: 0x10-0x1F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 2: 0x20-0x2F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 3: 0x30-0x3F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 4: 0x40-0x4F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 5: 0x50-0x5F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 6: 0x60-0x6F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 7: 0x70-0x7F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 8: 0x80-0x8F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 9: 0x90-0x9F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* A: 0xA0-0xAF */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* B: 0xB0-0xBF */
    0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* C: 0xC0-0xCF */
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* D: 0xD0-0xDF */
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* E: 0xE0-0xEF */
    4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* F: 0xF0-0xFF */
};

int lib_utf8_get_sequence_len(char c) {
    unsigned char i = (unsigned char) c;
    return LIB_UTF8_SEQUENCE_LEN[i];
}

int lib_utf8_get_sequence_len2(char c) {
    if (0xF0 == (0xF8 & c)) {
        return 4;
    } else if (0xE0 == (0xF0 & c)) {
        return 3;
    } else if (0xC0 == (0xE0 & c)) {
        return 2;
    }
    return 1; // TODO
}

int lib_utf8_get_sequence_codepoint(const char* str, int len) {
    if (!str || len <= 0) {
        return -1;
    }

    if (len == 1) {
        /* 1 byte utf8 codepoint */
        return str[0];
    } else if (len == 2) {
        /* 2 byte utf8 codepoint */
        return ((0x1F & str[0]) << 6) | (0x3F & str[1]);
    } else if (len == 3) {
        /* 3 byte utf8 codepoint */
        return ((0x0F & str[0]) << 12) | ((0x3F & str[1]) << 6) | (0x3F & str[2]);
        //str += 3;
    } else if (len == 4) {
        /* 4 byte utf8 codepoint */
        return ((0x07 & str[0]) << 18) | ((0x3F & str[1]) << 12) |
                     ((0x3F & str[2]) << 6) | (0x3F & str[3]);
    }

    return -1;
}

int lib_utf8_get_codepoint(const char* str) {
    int codepoint = -1;
    lib_utf8_to_codepoint(str, &codepoint);
    return codepoint;
}

int lib_utf8_to_utf8(char* dst, int codepoint) {

    if (!dst || codepoint < 0) {
        dst[0] = '\0';
        return 0;
    }

    if (codepoint <= 0x7F) {
        // 1-byte ASCII
        dst[0] = (char) codepoint;
        dst[1] = '\0';
        return 1;
    } else if (codepoint <= 0x07FF) {
        // 2-byte unicode
        dst[0] = (char) (((codepoint >> 6) & 0x1F) | 0xC0);
        dst[1] = (char) (((codepoint >> 0) & 0x3F) | 0x80);
        dst[2] = '\0';
        return 2;
    } else if (codepoint <= 0xFFFF) {
        // 3-byte unicode
        dst[0] = (char) (((codepoint >> 12) & 0x0F) | 0xE0); /* 0x0F */
        dst[1] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        dst[2] = (char) (((codepoint >> 0) & 0x3F) | 0x80);
        dst[3] = '\0';
        return 3;
    } else if (codepoint <= 0x10FFFF) {
        // 4-byte unicode
        dst[0] = (char) (((codepoint >> 18) & 0x07) | 0xF0); /* 0x07 */
        dst[1] = (char) (((codepoint >> 12) & 0x3F) | 0x80);
        dst[2] = (char) (((codepoint >> 6) & 0x3F) | 0x80);
        dst[3] = (char) (((codepoint >> 0) & 0x3F) | 0x80);
        dst[4] = '\0';
        return 4;
    }
    
    // error - use replacement character
    dst[0] = (char) 0xEF;
    dst[1] = (char) 0xBF;
    dst[2] = (char) 0xBD;
    dst[3] = '\0';
    return -1;
}

int lib_utf8_to_codepoint(const char* str, int* codepoint) {

    if (!str) {
        return 0;
    }
    int len = lib_utf8_get_sequence_len(str[0]);
    if (len <= 0) {
        return -1;
    }

    *codepoint = lib_utf8_get_sequence_codepoint(str, len);
    if (*codepoint < 0)  {
        return -1;
    }

    // if (len == 4) {
    //     /* 4 byte utf8 codepoint */
    //     *codepoint = ((0x07 & str[0]) << 18) | ((0x3f & str[1]) << 12) |
    //                  ((0x3f & str[2]) << 6) | (0x3f & str[3]);
    //     //str += 4;
    // } else if (len == 3) {
    //     /* 3 byte utf8 codepoint */
    //     *codepoint =
    //         ((0x0F & str[0]) << 12) | ((0x3F & str[1]) << 6) | (0x3F & str[2]);
    //     //str += 3;
    // } else if (len == 2) {
    //     /* 2 byte utf8 codepoint */
    //     *codepoint = ((0x1F & str[0]) << 6) | (0x3F & str[1]);
    //     //str += 2;
    // } else {
    //     /* 1 byte utf8 codepoint otherwise */
    //     *codepoint = str[0];
    //     //str += 1;
    // }

    // //return (char*) str;

    return len;
}

int lib_utf8_encode(char* dst, int codepoint) {
    return lib_utf8_to_utf8(dst, codepoint);
}

int lib_utf8_decode(const char* str, int* codepoint) {
    return lib_utf8_to_codepoint(str, codepoint);
}

int lib_utf8_get_codepoint_len(const char* str) {

    if (!str) {
        return 0;
    }

    int str_len = strlen(str);
    if (str_len == 0) {
        return 0;
    }

    int i = 0;
    while (i < str_len) {
        char c = str[i];
        int len = lib_utf8_get_sequence_len(c);
        if (len <= 0) {
            return -1;
        }
        i += len;
    }

    return i;
}
