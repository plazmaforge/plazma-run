#include <string.h>
#include <stdint.h>
//#include <stdio.h>

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

const char* lib_utf8_to_codepoint_next(const char* str, int* codepoint, int* len) {

    //const char* s = (const char*) str;
    *codepoint = -1;
    *len = -1;

    if (!str) {
        return NULL; //s;
    }
    int ilen = lib_utf8_get_sequence_len(str[0]);
    if (ilen <= 0) {
        return NULL; //s;
    }

    *codepoint = lib_utf8_get_sequence_codepoint(str, ilen);
    if (*codepoint < 0)  {
        return NULL; //s;
    }

    const char* s = (const char*) str;
    s += ilen;
    return s;
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

////

int lib_utf8_to_lower_codepoint(int cp) {
    if (((0x0041 <= cp) && (0x005a >= cp)) ||
        ((0x00c0 <= cp) && (0x00d6 >= cp)) ||
        ((0x00d8 <= cp) && (0x00de >= cp)) ||
        ((0x0391 <= cp) && (0x03a1 >= cp)) ||
        ((0x03a3 <= cp) && (0x03ab >= cp)) ||

        ((0x0410 <= cp) && (0x042f >= cp)))
        {
        cp += 32;
    } else if ((0x0400 <= cp) && (0x040f >= cp)) {
        cp += 80;         
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
               ((0x0132 <= cp) && (0x0137 >= cp)) ||
               ((0x014a <= cp) && (0x0177 >= cp)) ||
               ((0x0182 <= cp) && (0x0185 >= cp)) ||
               ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
               ((0x01de <= cp) && (0x01ef >= cp)) ||
               ((0x01f8 <= cp) && (0x021f >= cp)) ||
               ((0x0222 <= cp) && (0x0233 >= cp)) ||
               ((0x0246 <= cp) && (0x024f >= cp)) ||
               ((0x03d8 <= cp) && (0x03ef >= cp)) ||
               
               ((0x0460 <= cp) && (0x0481 >= cp)) ||
               ((0x048a <= cp) && (0x04ff >= cp))
               ) {
        cp |= 0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
               ((0x0179 <= cp) && (0x017e >= cp)) ||
               ((0x01af <= cp) && (0x01b0 >= cp)) ||
               ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
               ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp += 1;
        cp &= ~0x1;
    } else {
        switch (cp) {
            default: break;
            case 0x0178: cp = 0x00ff; break;
            case 0x0243: cp = 0x0180; break;
            case 0x018e: cp = 0x01dd; break;
            case 0x023d: cp = 0x019a; break;
            case 0x0220: cp = 0x019e; break;
            case 0x01b7: cp = 0x0292; break;
            case 0x01c4: cp = 0x01c6; break;
            case 0x01c7: cp = 0x01c9; break;
            case 0x01ca: cp = 0x01cc; break;
            case 0x01f1: cp = 0x01f3; break;
            case 0x01f7: cp = 0x01bf; break;
            case 0x0187: cp = 0x0188; break;
            case 0x018b: cp = 0x018c; break;
            case 0x0191: cp = 0x0192; break;
            case 0x0198: cp = 0x0199; break;
            case 0x01a7: cp = 0x01a8; break;
            case 0x01ac: cp = 0x01ad; break;
            case 0x01af: cp = 0x01b0; break;
            case 0x01b8: cp = 0x01b9; break;
            case 0x01bc: cp = 0x01bd; break;
            case 0x01f4: cp = 0x01f5; break;
            case 0x023b: cp = 0x023c; break;
            case 0x0241: cp = 0x0242; break;
            case 0x03fd: cp = 0x037b; break;
            case 0x03fe: cp = 0x037c; break;
            case 0x03ff: cp = 0x037d; break;
            case 0x037f: cp = 0x03f3; break;
            case 0x0386: cp = 0x03ac; break;
            case 0x0388: cp = 0x03ad; break;
            case 0x0389: cp = 0x03ae; break;
            case 0x038a: cp = 0x03af; break;
            case 0x038c: cp = 0x03cc; break;
            case 0x038e: cp = 0x03cd; break;
            case 0x038f: cp = 0x03ce; break;
            case 0x0370: cp = 0x0371; break;
            case 0x0372: cp = 0x0373; break;
            case 0x0376: cp = 0x0377; break;
            case 0x03f4: cp = 0x03d1; break;
            case 0x03cf: cp = 0x03d7; break;
            case 0x03f9: cp = 0x03f2; break;
            case 0x03f7: cp = 0x03f8; break;
            case 0x03fa: cp = 0x03fb; break;
        };
    }

    return cp;
}

int lib_utf8_to_upper_codepoint(int cp) {
    if (((0x0061 <= cp) && (0x007a >= cp)) ||
        ((0x00e0 <= cp) && (0x00f6 >= cp)) ||
        ((0x00f8 <= cp) && (0x00fe >= cp)) ||
        ((0x03b1 <= cp) && (0x03c1 >= cp)) ||
        ((0x03c3 <= cp) && (0x03cb >= cp)) ||

        ((0x0430 <= cp) && (0x044f >= cp))
        ) {
        cp -= 32;

    } else if ((0x0450 <= cp) && (0x045f >= cp)) {
        cp -= 80;        
    } else if (((0x0100 <= cp) && (0x012f >= cp)) ||
               ((0x0132 <= cp) && (0x0137 >= cp)) ||
               ((0x014a <= cp) && (0x0177 >= cp)) ||
               ((0x0182 <= cp) && (0x0185 >= cp)) ||
               ((0x01a0 <= cp) && (0x01a5 >= cp)) ||
               ((0x01de <= cp) && (0x01ef >= cp)) ||
               ((0x01f8 <= cp) && (0x021f >= cp)) ||
               ((0x0222 <= cp) && (0x0233 >= cp)) ||
               ((0x0246 <= cp) && (0x024f >= cp)) ||
               ((0x03d8 <= cp) && (0x03ef >= cp)) ||

               ((0x0460 <= cp) && (0x0481 >= cp)) ||
               ((0x048a <= cp) && (0x04ff >= cp))
               ) {
        cp &= ~0x1;
    } else if (((0x0139 <= cp) && (0x0148 >= cp)) ||
               ((0x0179 <= cp) && (0x017e >= cp)) ||
               ((0x01af <= cp) && (0x01b0 >= cp)) ||
               ((0x01b3 <= cp) && (0x01b6 >= cp)) ||
               ((0x01cd <= cp) && (0x01dc >= cp))) {
        cp -= 1;
        cp |= 0x1;
    } else {
        switch (cp) {
            default: break;
            case 0x00ff: cp = 0x0178; break;
            case 0x0180: cp = 0x0243; break;
            case 0x01dd: cp = 0x018e; break;
            case 0x019a: cp = 0x023d; break;
            case 0x019e: cp = 0x0220; break;
            case 0x0292: cp = 0x01b7; break;
            case 0x01c6: cp = 0x01c4; break;
            case 0x01c9: cp = 0x01c7; break;
            case 0x01cc: cp = 0x01ca; break;
            case 0x01f3: cp = 0x01f1; break;
            case 0x01bf: cp = 0x01f7; break;
            case 0x0188: cp = 0x0187; break;
            case 0x018c: cp = 0x018b; break;
            case 0x0192: cp = 0x0191; break;
            case 0x0199: cp = 0x0198; break;
            case 0x01a8: cp = 0x01a7; break;
            case 0x01ad: cp = 0x01ac; break;
            case 0x01b0: cp = 0x01af; break;
            case 0x01b9: cp = 0x01b8; break;
            case 0x01bd: cp = 0x01bc; break;
            case 0x01f5: cp = 0x01f4; break;
            case 0x023c: cp = 0x023b; break;
            case 0x0242: cp = 0x0241; break;
            case 0x037b: cp = 0x03fd; break;
            case 0x037c: cp = 0x03fe; break;
            case 0x037d: cp = 0x03ff; break;
            case 0x03f3: cp = 0x037f; break;
            case 0x03ac: cp = 0x0386; break;
            case 0x03ad: cp = 0x0388; break;
            case 0x03ae: cp = 0x0389; break;
            case 0x03af: cp = 0x038a; break;
            case 0x03cc: cp = 0x038c; break;
            case 0x03cd: cp = 0x038e; break;
            case 0x03ce: cp = 0x038f; break;
            case 0x0371: cp = 0x0370; break;
            case 0x0373: cp = 0x0372; break;
            case 0x0377: cp = 0x0376; break;
            case 0x03d1: cp = 0x03f4; break;
            case 0x03d7: cp = 0x03cf; break;
            case 0x03f2: cp = 0x03f9; break;
            case 0x03f8: cp = 0x03f7; break;
            case 0x03fb: cp = 0x03fa; break;
        };
    }

    return cp;
}

static int _lib_utf8_to_case_codepoint(int mode, int cp) {
    if (mode == 1) {
        return lib_utf8_to_lower_codepoint(cp);
    } else {
        return lib_utf8_to_upper_codepoint(cp);
    }
}

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
        cp2 = _lib_utf8_to_case_codepoint(mode, cp1);
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
