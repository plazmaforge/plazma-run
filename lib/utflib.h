#ifndef PLAZMA_LIB_UTFLIB_H
#define PLAZMA_LIB_UTFLIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"
#include "encpre.h"
#include "encbom.h"
#include "unidef.h"
#include "utf8lib.h"
#include "utf16lib.h"
#include "utf32lib.h"

//// UTF

/**
 * Return true if UTF (UTF-8, UTF-16, UTF-32) string is valid.
 * First numbers only.
 */
static bool lib_utf_is_utf_n(const char* str, size_t num);

static int lib_utf_char_seq(int utf_id, const char* str);

static int lib_utf_code_seq(int utf_id, int cp);

/*
 * Convert a codepount to a char and return lenght of the char.
 */
static int lib_utf_to_char(int enc_id, char* buf, int cp);

/*
 * Convert a char to a codepount and return lenght of the char.
 */
static int lib_utf_to_code(int enc_id, const char* str, int* cp);


//// UTF

static bool lib_utf_is_utf_n(const char* str, size_t num) {
    if (!str) {
        return true;
    }
    int enc_id = lib_enc_get_bom_n(str, num);
    if (enc_id > 0) {
        return (enc_id == LIB_ENC_UTF8_BOM_ID 
        || enc_id == LIB_ENC_UTF16BE_BOM_ID
        || enc_id == LIB_ENC_UTF16LE_BOM_ID
        || enc_id == LIB_ENC_UTF32BE_BOM_ID
        || enc_id == LIB_ENC_UTF16LE_BOM_ID
        );
    }

    // TODO: Not implemented yet
    return false;

    //return lib_utf8_is_valid(str);   
    //int count = lib_utf8_code_count(str);
    //return count >= 0;
}

//// UTF

static int lib_utf_char_seq(int enc_id, const char* str) {
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

static int lib_utf_code_seq(int enc_id, int cp) {

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
static int lib_utf_to_char(int enc_id, char* buf, int cp) {

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
static int lib_utf_to_code(int enc_id, const char* str, int* cp) {
    
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

#endif // PLAZMA_LIB_UTFLIB_H
