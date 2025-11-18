#ifndef PLAZMA_LIB_ENCBOM_H
#define PLAZMA_LIB_ENCBOM_H

#include "intlib.h"
#include "encpre.h"

/*
 * Return [EncodingID] BOM (byte order mark) type of a string.
 */
static int lib_enc_get_bom_n(const char* str, size_t num);

/*
 * Convert [EncodingID] BOM (byte order mark) type to a string representation.
 * First numbers only.
 */
static const char* lib_enc_to_bom_str(int enc_id);

////

// BOM
// https://en.wikipedia.org/wiki/Byte_order_mark

static int lib_enc_get_bom_n(const char* str, size_t num) {
    if (!str || num < 2) {
        return LIB_ENC_NONE;
    }

    uint8_t u1;
    uint8_t u2;
    uint8_t u3;
    uint8_t u4;

    // UTF-16
    if (num >= 2) {
        u1 = _u8(str[0]);
        u2 = _u8(str[1]);

        // UTF-16 (BE): FE FF
        if (u1 == 0xFE && u2 == 0xFF) {
            return LIB_ENC_UTF16BE_BOM_ID;
        }

        // UTF-16 (LE): FF FE
        if (u1 == 0xFF && u2 == 0xFE) {
            return LIB_ENC_UTF16LE_BOM_ID;
        }
    }

    // UTF-8
    if (num >= 3) {
        u1 = _u8(str[0]);
        u2 = _u8(str[1]);
        u3 = _u8(str[2]);

        // UTF-8: EF BB BF
        if (u1 == 0xEF && u2 == 0xBB && u3 == 0xBF) {
            return LIB_ENC_UTF8_BOM_ID;
        }

        // UTF-1: F7 64 4C
        if (u1 == 0xF7 && u2 == 0x64 && u3 == 0x4C) {
            return LIB_ENC_UTF1_BOM_ID;
        }

        // SCSU: 0E FE FF
        if (u1 == 0x0E && u2 == 0xFE && u3 == 0xFF) {
            return LIB_ENC_SCSU_BOM_ID;
        }

        // BOCU-1: FB EE 28
        if (u1 == 0xFB && u2 == 0xEE && u3 == 0x28) {
            return LIB_ENC_BOCU1_BOM_ID;
        }

    }

    // UTF-32
    if (num >= 4) {
        u1 = _u8(str[0]);
        u2 = _u8(str[1]);
        u3 = _u8(str[2]);
        u4 = _u8(str[3]);

        // UTF-32 (BE): 00 00 FE FF
        if (u1 == 0x00 && u2 == 0x00 && u3 == 0xFE && u4 == 0xFF) {
            return LIB_ENC_UTF32BE_BOM_ID;
        }

        // UTF-32 (LE): FF FE 00 00
        if (u1 == 0xFF && u2 == 0xFE && u3 == 0x00 && u4 == 0x00) {
            return LIB_ENC_UTF32LE_BOM_ID;
        }

        // UTF-7: 2B 2F 76
        if (u1 == 0x2B && u2 == 0x2F && u3 == 0x76) {

            //  38, 39, 2B, 2F
            if (u4 == 0x38 || u4 == 0x39 || u4 == 0x2B || u4 == 0x2F) {
                return LIB_ENC_UTF7_BOM_ID;
            }
        }

        // UTF-EBCDIC: DD 73 66 73
        if (u1 == 0xDD && u2 == 0x73 && u3 == 0x66 && u4 == 0x73) {
            return LIB_ENC_UTF_EBCDIC_BOM_ID;
        }

        // GB18030: 84 31 95 33
        if (u1 == 0x84 && u2 == 0x31 && u3 == 0x95 && u4 == 0x33) {
            return LIB_ENC_GB18030_BOM_ID;
        }

    }

    return LIB_ENC_NONE;
}

/**
 * Return lenght of BOM
 */
static int lib_enc_bom_len(int enc_id) {

    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID) {
        return 0;
    }
    if (enc_id == LIB_ENC_UTF8_BOM_ID) {
        return 3;
    }

    // UTF-16
    if (enc_id == LIB_ENC_UTF16_ID) {
        // UTF-16 by default with BOM (!)
        return 2;
    }

    // UTF-16 (BE/LE)
    if (enc_id == LIB_ENC_UTF16BE_ID
     || enc_id == LIB_ENC_UTF16LE_ID) {
        return 0;
    }

    // UTF-16-BOM
    if (enc_id == LIB_ENC_UTF16BE_BOM_ID
     || enc_id == LIB_ENC_UTF16LE_BOM_ID) {
        return 2;
    }

    // UTF-32
    if (enc_id == LIB_ENC_UTF32_ID) {
        // UTF-32 by default with BOM (!)
        return 4;
    }

    // UTF-32 (BE/LE)
    if (enc_id == LIB_ENC_UTF32BE_ID
     || enc_id == LIB_ENC_UTF32LE_ID) {
        return 0;
    }

    // UTF-32-BOM
    if (enc_id == LIB_ENC_UTF32BE_BOM_ID
     || enc_id == LIB_ENC_UTF32LE_BOM_ID) {
        return 4;
    }

    // UTF-7
    if (enc_id == LIB_ENC_UTF7_ID) {
        return 0;
    }
    if (enc_id == LIB_ENC_UTF7_BOM_ID) {
        return 4;
    }

    // UCS-2
    if (enc_id == LIB_ENC_UCS2_ID) {
        // UCS-2 by default without BOM (!)
        // UTF16 by default with BOM    (!)
        return 0;
    }

    // UCS-2 (BE/LE)
    if (enc_id == LIB_ENC_UCS2BE_ID
     || enc_id == LIB_ENC_UCS2LE_ID) {
        return 0;
    }

    // UCS-2-BOM
    if (enc_id == LIB_ENC_UCS2BE_BOM_ID
     || enc_id == LIB_ENC_UCS2LE_BOM_ID) {
        return 2;
    }

    // UCS-4
    if (enc_id == LIB_ENC_UCS4_ID) {
        // UCS-4  by default without BOM (!)
        // UTF-32 by default with BOM    (!)
        return 0;
    }

    // UCS-4 (BE/LE)
    if (enc_id == LIB_ENC_UCS4BE_ID
     || enc_id == LIB_ENC_UCS4LE_ID) {
        return 0;
    }

    // UCS-4-BOM
    if (enc_id == LIB_ENC_UCS4BE_BOM_ID
     || enc_id == LIB_ENC_UCS4LE_BOM_ID) {
        return 4;
    }

    return 0;
}

/**
 * Set BOM and return lenght of BOM or error (-1)
 */
static int lib_enc_set_bom(int enc_id, char* str) {
    if (!str) {
        return 0;
    }

    // UTF-8
    if (enc_id == LIB_ENC_UTF8_ID) {
        return 0;
    }
    if (enc_id == LIB_ENC_UTF8_BOM_ID) {
        str[0] = 0xEF;
        str[1] = 0xBB;
        str[2] = 0xBF;
        return 3;
    }

    // UTF-16, UCS-2
    if (enc_id == LIB_ENC_UTF16_ID
     // UTF-16 by default with BOM (!)
     || enc_id == LIB_ENC_UTF16BE_BOM_ID
     || enc_id == LIB_ENC_UCS2BE_BOM_ID  /* like UTF-16 */
     ) {
        str[0] = 0xFE;
        str[1] = 0xFF;
        return 2;
    }
    if (enc_id == LIB_ENC_UTF16LE_BOM_ID
     || enc_id == LIB_ENC_UCS2LE_BOM_ID  /* like UTF-16 */
    ) {
        str[0] = 0xFF;
        str[1] = 0xFE;
        return 2;
    }

    // UTF-16, UCS-2 (BE/LE)
    if (enc_id == LIB_ENC_UTF16BE_ID
     || enc_id == LIB_ENC_UTF16LE_ID
     || enc_id == LIB_ENC_UCS2BE_ID      /* like UTF-16 */
     || enc_id == LIB_ENC_UCS2LE_ID      /* like UTF-16 */
     // UCS-2 by default without BOM (!)
     || enc_id == LIB_ENC_UCS2_ID

     ) {
        return 0;
    }

    // UTF-32
    if (enc_id == LIB_ENC_UTF32_ID
     // UTF-32 by default with BOM (!)
     || enc_id == LIB_ENC_UTF32BE_BOM_ID
     || enc_id == LIB_ENC_UCS4BE_BOM_ID  /* like UTF-32 */
     ) {
        str[0] = 0x00;
        str[1] = 0x00;
        str[2] = 0xFE;
        str[3] = 0xFF;
        return 4;
    }
    if (enc_id == LIB_ENC_UTF32LE_BOM_ID
     || enc_id == LIB_ENC_UCS4LE_BOM_ID  /* like UTF-32 */
    ) {
        str[0] = 0xFF;
        str[1] = 0xFE;
        str[2] = 0x00;
        str[3] = 0x00;
        return 4;
    }

    // UTF-32 (BE/LE)
    if (enc_id == LIB_ENC_UTF32BE_ID
     || enc_id == LIB_ENC_UTF32LE_ID
     || enc_id == LIB_ENC_UCS4BE_ID      /* like UTF-32 */
     || enc_id == LIB_ENC_UCS4LE_ID      /* like UTF-32 */
     // UCS-4 by default without BOM (!)
     || enc_id == LIB_ENC_UCS4_ID

     ) {
        return 0;
    }

    // UTF-7
    if (enc_id == LIB_ENC_UTF7_ID) {
        return 0;
    }
    if (enc_id == LIB_ENC_UTF7_BOM_ID) {
        str[0] = 0x2B;
        str[1] = 0x2F;
        str[2] = 0x76;
        str[3] = 0x38; // 0x38 | 0x39 | 0x2B | 0x2F
        return 4;
    }

    return 0;
}

////

static const char* lib_enc_to_bom_str(int enc_id) {
    switch (enc_id) {

    // UTF-8
    case LIB_ENC_UTF8_BOM_ID:
        return "UTF-8";

    // UTF-16
    case LIB_ENC_UTF16BE_BOM_ID:
        return "UTF-16BE";
    case LIB_ENC_UTF16LE_BOM_ID:
        return "UTF-16LE";

    // UTF-32
    case LIB_ENC_UTF32BE_BOM_ID:
        return "UTF-32BE";
    case LIB_ENC_UTF32LE_BOM_ID:
        return "UTF-32LE";

    // UTF-7
    case LIB_ENC_UTF7_BOM_ID:
        return "UTF-7";

    // UTF-1, UTF_EBCDIC
    case LIB_ENC_UTF1_BOM_ID:
        return "UTF-1";
    case LIB_ENC_UTF_EBCDIC_BOM_ID:
        return "UTF-EBCDIC";

    // SCSU, BOCU-1, GB18030
    case LIB_ENC_SCSU_BOM_ID:
        return "SCSU";
    case LIB_ENC_BOCU1_BOM_ID:
        return "BOCU-1";
    case LIB_ENC_GB18030_BOM_ID:
        return "GB-18030";

    }
    return "";
}

#endif // PLAZMA_LIB_ENCBOM_H
