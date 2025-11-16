#ifndef PLAZMA_LIB_UCSLIB_H
#define PLAZMA_LIB_UCSLIB_H

#include <stdbool.h>
#include <stdint.h>

#include "intlib.h"
#include "encpre.h"
#include "encbom.h"
#include "unidef.h"
#include "ucs2lib.h"
//#include "ucs4lib.h"

static int lib_ucs_char_seq(int enc_id, const char* str);

static int lib_ucs_code_seq(int enc_id, int cp);

/*
 * Convert a codepount to a char and return lenght of the char.
 */
static int lib_ucs_to_char(int enc_id, char* buf, int cp);

/*
 * Convert a char to a codepount and return lenght of the char.
 */
static int lib_ucs_to_code(int enc_id, const char* str, int* cp);


// UCS

static int lib_ucs_char_seq(int enc_id, const char* str) {
    if (!str) {
        // error
        return 0;
    }

    // UCS-4
    if (enc_id == LIB_ENC_UCS2_ID) {
        return lib_ucs2_char_seq(str);
    }
    if (enc_id == LIB_ENC_UCS2BE_ID
     //|| enc_id == LIB_ENC_UCS2BE_BOM_ID) 
    ) {
        return lib_ucs2be_char_seq(str);
    }
    if (enc_id == LIB_ENC_UCS2LE_ID
     //|| enc_id == LIB_ENC_UCS2LE_BOM_ID) 
     ) {
        return lib_ucs2le_char_seq(str);
    }

    // UCS-4  (BE/LE)
    // if (enc_id == LIB_ENC_UCS4_ID 
    //  || enc_id == LIB_ENC_UCS4BE_ID 
    //  || enc_id == LIB_ENC_UCS4LE_ID
    //  || enc_id == LIB_ENC_UCS4BE_BOM_ID
    //  || enc_id == LIB_ENC_UCS4LE_BOM_ID) {
    //     return lib_ucs4_char_seq(str);
    // }

    return 0;
}

static int lib_ucs_code_seq(int enc_id, int cp) {

    // UCS-2 (BE/LE)
    if (enc_id == LIB_ENC_UCS2_ID 
     || enc_id == LIB_ENC_UCS2BE_ID 
     || enc_id == LIB_ENC_UCS2LE_ID
     //|| enc_id == LIB_ENC_UCS2BE_BOM_ID 
     //|| enc_id == LIB_ENC_UCS2LE_BOM_ID

     ) {
        return lib_ucs2_code_seq(cp);
    }

    // UCS-4 (BE/LE)
    // if (enc_id == LIB_ENC_UCS4_ID 
    //  || enc_id == LIB_ENC_UCS4BE_ID 
    //  || enc_id == LIB_ENC_UCS4LE_ID
    //  || enc_id == LIB_ENC_UCS4BE_BOM_ID
    //  || enc_id == LIB_ENC_UCS4LE_BOM_ID) {
    //     return lib_ucs4_code_seq(cp);
    // }

    return 0;
}

/*
 * Convert a codepount to a char and return lenght of the char.
 */
static int lib_ucs_to_char(int enc_id, char* buf, int cp) {


    // UCS-2
    if (enc_id == LIB_ENC_UCS2_ID) {
        return lib_ucs2_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UCS2BE_ID) {
        return lib_ucs2be_to_char(buf, cp);
    }
    if (enc_id == LIB_ENC_UCS2LE_ID) {
        return lib_ucs2le_to_char(buf, cp);
    }
    // if (enc_id == LIB_ENC_UCS2BE_BOM_ID) {
    //     return lib_ucs2be_to_char(buf, cp);
    // }
    // if (enc_id == LIB_ENC_UCS26LE_BOM_ID) {
    //     return lib_ucs2le_to_char(buf, cp);
    // }

    // UCS-4
    // if (enc_id == LIB_ENC_UCS4_ID) {
    //     return lib_ucs4_to_char(buf, cp);
    // }
    // if (enc_id == LIB_ENC_UCS4BE_ID) {
    //     return lib_ucs4be_to_char(buf, cp);
    // }
    // if (enc_id == LIB_ENC_UCS4LE_ID) {
    //     return lib_ucs4le_to_char(buf, cp);
    // }
    // if (enc_id == LIB_ENC_UCS4BE_BOM_ID) {
    //     return lib_ucs4be_to_char(buf, cp);
    // }
    // if (enc_id == LIB_ENC_UCS4LE_BOM_ID) {
    //     return lib_ucs4le_to_char(buf, cp);
    // }

    return -1;
}

/*
 * Convert a char to a codepount and return lenght of the char.
 */
static int lib_ucs_to_code(int enc_id, const char* str, int* cp) {
    
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


#endif // PLAZMA_LIB_UCSLIB_H
