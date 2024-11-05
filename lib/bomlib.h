#ifndef PLAZMA_LIB_BOMLIB_H
#define PLAZMA_LIB_BOMLIB_H

#define LIB_BOM_UNKNOWN    -1
#define LIB_BOM_NONE       0

#define LIB_BOM_UTF8       1
#define LIB_BOM_UTF16_BE   2
#define LIB_BOM_UTF16_LE   3
#define LIB_BOM_UTF32_BE   4
#define LIB_BOM_UTF32_LE   5

#define LIB_BOM_UTF7       6
#define LIB_BOM_UTF1       7
#define LIB_BOM_UTF_EBCDIC 8

#define LIB_BOM_SCSU       9
#define LIB_BOM_BOCU1      10
#define LIB_BOM_GB_18030   11

/*
 * Return byte order mark (BOM) of a string.
 */
int lib_bom_get_bom_n(const char* str, int num);

/*
 * Convert BOM type to a string representation.
 * First numbers only.
 */
const char* lib_bom_to_bom_str(int bom);

////

// BOM
// https://en.wikipedia.org/wiki/Byte_order_mark

int lib_bom_get_bom_n(const char* str, int num) {
    if (!str || num < 2) {
        return LIB_BOM_NONE;
    }

    unsigned char u1;
    unsigned char u2;
    unsigned char u3;
    unsigned char u4;

    // UTF-16
    if (num >= 2) {
        u1 = (unsigned char) str[0];
        u2 = (unsigned char) str[1];

        // UTF-16 (BE): FE FF
        if (u1 == 0xFE && u2 == 0xFF) {
            return LIB_BOM_UTF16_BE;
        }

        // UTF-16 (LE): FF FE
        if (u1 == 0xFF && u2 == 0xFE) {
            return LIB_BOM_UTF16_LE;
        }
    }

    // UTF-8
    if (num >= 3) {
        u1 = (unsigned char) str[0];
        u2 = (unsigned char) str[1];
        u3 = (unsigned char) str[2];

        // UTF-8: EF BB BF
        if (u1 == 0xEF && u2 == 0xBB && u3 == 0xBF) {
            return LIB_BOM_UTF8;
        }

        // UTF-7: 2B 2F 76
        if (u1 == 0x2B && u2 == 0x2F && u3 == 0x76) {

            // TODO: if (u4 !=  38, 39, 2B, 2F)
            // return LIB_BOM_UNKNOWN

            return LIB_BOM_UTF7;
        }

        // UTF-1: F7 64 4C
        if (u1 == 0xF7 && u2 == 0x64 && u3 == 0x4C) {
            return LIB_BOM_UTF1;
        }

        // SCSU: 0E FE FF
        if (u1 == 0x0E && u2 == 0xFE && u3 == 0xFF) {
            return LIB_BOM_SCSU;
        }

        // BOCU-1: FB EE 28
        if (u1 == 0xFB && u2 == 0xEE && u3 == 0x28) {
            return LIB_BOM_BOCU1;
        }

    }

    // UTF-32
    if (num >= 4) {
        u1 = (unsigned char) str[0];
        u2 = (unsigned char) str[1];
        u3 = (unsigned char) str[2];
        u4 = (unsigned char) str[3];

        // UTF-32 (BE): 00 00 FE FF
        if (u1 == 0x00 && u2 == 0x00 && u3 == 0xFE && u4 == 0xFF) {
            return LIB_BOM_UTF32_BE;
        }

        // UTF-32 (LE): FF FE 00 00
        if (u1 == 0xFF && u2 == 0xFE && u3 == 0x00 && u4 == 0x00) {
            return LIB_BOM_UTF32_LE;
        }

        // UTF-EBCDIC: DD 73 66 73
        if (u1 == 0xDD && u2 == 0x73 && u3 == 0x66 && u4 == 0x73) {
            return LIB_BOM_UTF_EBCDIC;
        }

        // GB18030: 84 31 95 33
        if (u1 == 0x84 && u2 == 0x31 && u3 == 0x95 && u4 == 0x33) {
            return LIB_BOM_GB_18030;
        }

    }

    return LIB_BOM_NONE;
}

const char* lib_bom_to_bom_str(int bom) {
    switch (bom) {

    // UTF-8
    case LIB_BOM_UTF8:
        return "UTF-8";

    // UTF-16
    case LIB_BOM_UTF16_BE:
        return "UTF-16BE";
    case LIB_BOM_UTF16_LE:
        return "UTF-16LE";

    // UTF-32
    case LIB_BOM_UTF32_BE:
        return "UTF-32BE";
    case LIB_BOM_UTF32_LE:
        return "UTF-32LE";

    // UTF-7, UTF-1, UTF-EBCDIC
    case LIB_BOM_UTF7:
        return "UTF-7";
    case LIB_BOM_UTF1:
        return "UTF-1";
    case LIB_BOM_UTF_EBCDIC:
        return "UTF-EBCDIC";

    // SCSU, BOCU-1, GB18030
    case LIB_BOM_SCSU:
        return "SCSU";
    case LIB_BOM_BOCU1:
        return "BOCU-1";
    case LIB_BOM_GB_18030:
        return "GB-18030";

    }
    return "";
}

#endif // PLAZMA_LIB_BOMLIB_H
