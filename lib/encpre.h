#ifndef PLAZMA_LIB_ENCPRE_H
#define PLAZMA_LIB_ENCPRE_H

#include <stdbool.h>

#define LIB_ENC_UNKNOWN         -1
#define LIB_ENC_NONE            0

/*
 Predefined encodings
*/

/* ISO */
#define LIB_ENC_ISO_8859_1        28591
#define LIB_ENC_ISO_8859_2        28592
#define LIB_ENC_ISO_8859_3        28593
#define LIB_ENC_ISO_8859_4        28594

#define LIB_ENC_ISO_8859_5        28595
#define LIB_ENC_ISO_8859_6        28596
#define LIB_ENC_ISO_8859_7        28597
#define LIB_ENC_ISO_8859_8        28598
#define LIB_ENC_ISO_8859_9        28599

#define LIB_ENC_ISO_8859_10       28600
#define LIB_ENC_ISO_8859_11       28601

#define LIB_ENC_ISO_8859_13       28603
#define LIB_ENC_ISO_8859_14       28604
#define LIB_ENC_ISO_8859_15       28605
#define LIB_ENC_ISO_8859_16       28606

/* UTF */
#define LIB_ENC_UTF7_ID           65000
#define LIB_ENC_UTF7_BOM_ID       1065000 /**/

#define LIB_ENC_UTF8_ID           65001
#define LIB_ENC_UTF8_BOM_ID       1065001 /**/

#define LIB_ENC_UTF16_ID          1200
#define LIB_ENC_UTF16BE_ID        1201
#define LIB_ENC_UTF16LE_ID        1202 /**/
#define LIB_ENC_UTF16BE_BOM_ID    1203 /**/
#define LIB_ENC_UTF16LE_BOM_ID    1204 /**/

#define LIB_ENC_UTF32_ID          12000
#define LIB_ENC_UTF32BE_ID        12001
#define LIB_ENC_UTF32LE_ID        12002 /**/
#define LIB_ENC_UTF32BE_BOM_ID    12003 /**/
#define LIB_ENC_UTF32LE_BOM_ID    12004 /**/

/* UCS */
#define LIB_ENC_UCS2_ID           1001200 /**/
#define LIB_ENC_UCS2BE_ID         1001201 /**/
#define LIB_ENC_UCS2LE_ID         1001202 /**/

#define LIB_ENC_UCS4_ID           1012000 /**/
#define LIB_ENC_UCS4BE_ID         1012001 /**/
#define LIB_ENC_UCS4LE_ID         1012002 /**/

/* OTHER */
#define LIB_ENC_UTF1_BOM_ID       999007 /**/
#define LIB_ENC_UTF_EBCDIC_BOM_ID 999008 /**/

#define LIB_ENC_SCSU_BOM_ID       999009 /**/
#define LIB_ENC_BOCU1_BOM_ID      999010 /**/
#define LIB_ENC_GB18030_BOM_ID    999011 /**/


/**
 * Return true if the encoding id is ISO type 
 */
static bool lib_enc_is_iso(int enc_id) {
  return (   enc_id == LIB_ENC_ISO_8859_1
          || enc_id == LIB_ENC_ISO_8859_2
          || enc_id == LIB_ENC_ISO_8859_3
          || enc_id == LIB_ENC_ISO_8859_4
          || enc_id == LIB_ENC_ISO_8859_5
          || enc_id == LIB_ENC_ISO_8859_6
          || enc_id == LIB_ENC_ISO_8859_7
          || enc_id == LIB_ENC_ISO_8859_8
          || enc_id == LIB_ENC_ISO_8859_9
          || enc_id == LIB_ENC_ISO_8859_10
          || enc_id == LIB_ENC_ISO_8859_11

          || enc_id == LIB_ENC_ISO_8859_13
          || enc_id == LIB_ENC_ISO_8859_14
          || enc_id == LIB_ENC_ISO_8859_15
          || enc_id == LIB_ENC_ISO_8859_16);
}

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

/**
 * Return true if the encoding id is UTF type 
 */
static bool lib_enc_is_utf(int enc_id) {
    return ( lib_enc_is_utf7(enc_id)
          || lib_enc_is_utf8(enc_id)
          || lib_enc_is_utf16(enc_id)
          || lib_enc_is_utf32(enc_id)
          );
}

/**
 * Return true if the encoding id is UCS type 
 */
static bool lib_enc_is_ucs(int enc_id) {
  return (   enc_id == LIB_ENC_UCS2_ID
    /**/  || enc_id == LIB_ENC_UCS2BE_ID
    /**/  || enc_id == LIB_ENC_UCS2LE_ID

    /**/  || enc_id == LIB_ENC_UCS4_ID
    /**/  || enc_id == LIB_ENC_UCS4BE_ID
    /**/  || enc_id == LIB_ENC_UCS4BE_ID);
}

/**
 * Return true if the encoding id is multi bytes type 
 */
static bool lib_enc_is_mbc(int enc_id) {
    return ( lib_enc_is_utf(enc_id)
          || lib_enc_is_ucs(enc_id)
          );
}


#endif // PLAZMA_LIB_ENCPRE_H
