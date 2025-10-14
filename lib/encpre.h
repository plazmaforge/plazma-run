#ifndef PLAZMA_LIB_ENCPRE_H
#define PLAZMA_LIB_ENCPRE_H

#include <stdbool.h>

#define LIB_ENC_UNKNOWN         -1
#define LIB_ENC_NONE            0

/*
 Predefined encodings
*/

#define LIB_ENC_UTF7_ID         65000
#define LIB_ENC_UTF7_BOM_ID     1065000 /**/

#define LIB_ENC_UTF8_ID         65001
#define LIB_ENC_UTF8_BOM_ID     1065001 /**/

#define LIB_ENC_UTF16_ID        1200
#define LIB_ENC_UTF16BE_ID      1201
#define LIB_ENC_UTF16LE_ID      1202 /**/
#define LIB_ENC_UTF16BE_BOM_ID  1203 /**/
#define LIB_ENC_UTF16LE_BOM_ID  1204 /**/

#define LIB_ENC_UTF32_ID        12000
#define LIB_ENC_UTF32BE_ID      12001
#define LIB_ENC_UTF32LE_ID      12002 /**/
#define LIB_ENC_UTF32BE_BOM_ID  12003 /**/
#define LIB_ENC_UTF32LE_BOM_ID  12004 /**/


#define LIB_ENC_UTF1_BOM_ID       999007 /**/
#define LIB_ENC_UTF_EBCDIC_BOM_ID 999008 /**/

#define LIB_ENC_SCSU_BOM_ID       999009 /**/
#define LIB_ENC_BOCU1_BOM_ID      999010 /**/
#define LIB_ENC_GB18030_BOM_ID    999011 /**/

/**
 * Return true if the encoding id is UTF-7 type 
 */
static bool lib_enc_is_utf7(int enc_id) {
  return (   enc_id == LIB_ENC_UTF7_ID
    /**/  || enc_id == LIB_ENC_UTF7_BOM_ID
        );
}

/**
 * Return true if the encoding id is UTF-8 type 
 */
static bool lib_enc_is_utf8(int enc_id) {
  return (   enc_id == LIB_ENC_UTF8_ID
    /**/  || enc_id == LIB_ENC_UTF8_BOM_ID
        );
}

/**
 * Return true if the encoding id is UTF-16 type 
 */
static bool lib_enc_is_utf16(int enc_id) {
  return (   enc_id == LIB_ENC_UTF16_ID
          || enc_id == LIB_ENC_UTF16BE_ID
    /**/  || enc_id == LIB_ENC_UTF16LE_ID     /* codepage = 1200  */
    /**/  || enc_id == LIB_ENC_UTF16BE_BOM_ID /* codepage = 1200  */
    /**/  || enc_id == LIB_ENC_UTF16LE_BOM_ID /* codepage = 1200  */
    );
}

/**
 * Return true if the encoding id is UTF-32 type 
 */
static bool lib_enc_is_utf32(int enc_id) {
  return (   enc_id == LIB_ENC_UTF32_ID
          || enc_id == LIB_ENC_UTF32BE_ID
    /**/  || enc_id == LIB_ENC_UTF32LE_ID     /* codepage = 12000  */
    /**/  || enc_id == LIB_ENC_UTF32BE_BOM_ID /* codepage = 12000  */
    /**/  || enc_id == LIB_ENC_UTF32LE_BOM_ID /* codepage = 12000  */
    );
}

/**
 * Return true if the encoding id is UTF-16 or UTF-32 type 
 */
static bool lib_enc_is_utf16or32_encoding(int enc_id) {
  return (   lib_enc_is_utf16(enc_id)
          || lib_enc_is_utf32(enc_id) );
}

#endif // PLAZMA_LIB_ENCPRE_H
