#ifndef PLAZMA_LIB_ENCPRE_H
#define PLAZMA_LIB_ENCPRE_H

#include <stdbool.h>

#define LIB_ENC_UNKNOWN         -1
#define LIB_ENC_NONE            0

/*
 Predefined encodings
*/

/* ISO     */
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

/* DOS     */
#define LIB_ENC_CP437_ID          437
#define LIB_ENC_CP737_ID          737
#define LIB_ENC_CP775_ID          775
#define LIB_ENC_CP850_ID          850
#define LIB_ENC_CP852_ID          852
#define LIB_ENC_CP855_ID          855
#define LIB_ENC_CP857_ID          857
#define LIB_ENC_CP860_ID          860
#define LIB_ENC_CP861_ID          861
#define LIB_ENC_CP862_ID          862
#define LIB_ENC_CP863_ID          863
#define LIB_ENC_CP864_ID          864
#define LIB_ENC_CP865_ID          865
#define LIB_ENC_CP866_ID          866
#define LIB_ENC_CP869_ID          869

/* WIN     */
#define LIB_ENC_CP1250_ID         1250
#define LIB_ENC_CP1251_ID         1251
#define LIB_ENC_CP1252_ID         1252
#define LIB_ENC_CP1253_ID         1253
#define LIB_ENC_CP1254_ID         1254
#define LIB_ENC_CP1255_ID         1255
#define LIB_ENC_CP1256_ID         1256
#define LIB_ENC_CP1257_ID         1257
#define LIB_ENC_CP1258_ID         1258

#define LIB_ENC_CP874_ID          874

/* WIN EXT  */
#define LIB_ENC_CP932_ID          932
#define LIB_ENC_CP936_ID          936
#define LIB_ENC_CP949_ID          949
#define LIB_ENC_CP950_ID          950

/* UTF     */
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

/* UCS     */
#define LIB_ENC_UCS2_ID           1001200 /**/
#define LIB_ENC_UCS2BE_ID         1001201 /**/
#define LIB_ENC_UCS2LE_ID         1001202 /**/
#define LIB_ENC_UCS2BE_BOM_ID     1001203 /**/
#define LIB_ENC_UCS2LE_BOM_ID     1001204 /**/

#define LIB_ENC_UCS4_ID           1012000 /**/
#define LIB_ENC_UCS4BE_ID         1012001 /**/
#define LIB_ENC_UCS4LE_ID         1012002 /**/
#define LIB_ENC_UCS4BE_BOM_ID     1012003 /**/
#define LIB_ENC_UCS4LE_BOM_ID     1012004 /**/

/* OTHER   */
#define LIB_ENC_UTF1_BOM_ID       999007 /**/
#define LIB_ENC_UTF_EBCDIC_BOM_ID 999008 /**/

#define LIB_ENC_SCSU_BOM_ID       999009 /**/
#define LIB_ENC_BOCU1_BOM_ID      999010 /**/
#define LIB_ENC_GB18030_BOM_ID    999011 /**/

/* KOI    */
#define LIB_ENC_KOI8R_ID          20866
#define LIB_ENC_KOI8U_ID          21866


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
 * Return true if the encoding id is DOS type 
 */
static bool lib_enc_is_dos(int enc_id) {
  return (   enc_id == LIB_ENC_CP437_ID
          || enc_id == LIB_ENC_CP737_ID
          || enc_id == LIB_ENC_CP775_ID
          || enc_id == LIB_ENC_CP850_ID
          || enc_id == LIB_ENC_CP852_ID
          || enc_id == LIB_ENC_CP855_ID
          || enc_id == LIB_ENC_CP857_ID

          || enc_id == LIB_ENC_CP860_ID
          || enc_id == LIB_ENC_CP861_ID
          || enc_id == LIB_ENC_CP862_ID
          || enc_id == LIB_ENC_CP863_ID
          || enc_id == LIB_ENC_CP864_ID
          || enc_id == LIB_ENC_CP865_ID
          || enc_id == LIB_ENC_CP866_ID

          || enc_id == LIB_ENC_CP869_ID);
}

/**
 * Return true if the encoding id is WIN type 
 */
static bool lib_enc_is_win(int enc_id) {
  return (   enc_id == LIB_ENC_CP1250_ID
          || enc_id == LIB_ENC_CP1251_ID
          || enc_id == LIB_ENC_CP1252_ID
          || enc_id == LIB_ENC_CP1253_ID
          || enc_id == LIB_ENC_CP1254_ID
          || enc_id == LIB_ENC_CP1255_ID
          || enc_id == LIB_ENC_CP1256_ID
          || enc_id == LIB_ENC_CP1257_ID
          || enc_id == LIB_ENC_CP1258_ID

          /* CP874 */ /* WINDOWS-874 ?*/
          || enc_id == LIB_ENC_CP874_ID
          || enc_id == LIB_ENC_CP932_ID
          || enc_id == LIB_ENC_CP936_ID
          || enc_id == LIB_ENC_CP949_ID
          || enc_id == LIB_ENC_CP950_ID
          
          );
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
    /**/  || enc_id == LIB_ENC_UTF16LE_ID     /* codepage = 1202  */
    /**/  || enc_id == LIB_ENC_UTF16BE_BOM_ID /* codepage = 1203  */
    /**/  || enc_id == LIB_ENC_UTF16LE_BOM_ID /* codepage = 1204  */
    );
}

/**
 * Return true if the encoding id is UTF-32 type 
 */
static bool lib_enc_is_utf32(int enc_id) {
  return (   enc_id == LIB_ENC_UTF32_ID
          || enc_id == LIB_ENC_UTF32BE_ID
    /**/  || enc_id == LIB_ENC_UTF32LE_ID     /* codepage = 12002  */
    /**/  || enc_id == LIB_ENC_UTF32BE_BOM_ID /* codepage = 12003  */
    /**/  || enc_id == LIB_ENC_UTF32LE_BOM_ID /* codepage = 12004  */
    );
}

/**
 * Return true if the encoding id is UTF-16 or UTF-32 type
 */
static bool lib_enc_is_utf16or32(int enc_id) {
  return (   lib_enc_is_utf16(enc_id)
          || lib_enc_is_utf32(enc_id) );
}

/**
 * Return true if the encoding id is UTF type
 */
static bool lib_enc_is_utf(int enc_id) {
    return (
          //lib_enc_is_utf7(enc_id)
          lib_enc_is_utf8(enc_id)
          || lib_enc_is_utf16(enc_id)
          || lib_enc_is_utf32(enc_id)
          );
}

/**
 * Return true if the encoding id is UCS-2 type
 */
static bool lib_enc_is_ucs2(int enc_id) {
  return (   enc_id == LIB_ENC_UCS2_ID       /* codepage = 1001200 */
    /**/  || enc_id == LIB_ENC_UCS2BE_ID     /* codepage = 1001201 */
    /**/  || enc_id == LIB_ENC_UCS2LE_ID     /* codepage = 1001202 */
    /**/  || enc_id == LIB_ENC_UCS2BE_BOM_ID /* codepage = 1001203 */
    /**/  || enc_id == LIB_ENC_UCS2LE_BOM_ID /* codepage = 1001204 */
    );
}

/**
 * Return true if the encoding id is UCS-4 type
 */
static bool lib_enc_is_ucs4(int enc_id) {
  return (   enc_id == LIB_ENC_UCS4_ID       /* codepage = 10012000 */
    /**/  || enc_id == LIB_ENC_UCS4BE_ID     /* codepage = 10012001 */
    /**/  || enc_id == LIB_ENC_UCS4LE_ID     /* codepage = 10012002 */
    /**/  || enc_id == LIB_ENC_UCS4BE_BOM_ID /* codepage = 10012003 */
    /**/  || enc_id == LIB_ENC_UCS4LE_BOM_ID /* codepage = 10012004 */
    );
}

/**
 * Return true if the encoding id is UCS-2 or UCS-4 type
 */
static bool lib_enc_is_ucs2or4(int enc_id) {
  return (   lib_enc_is_ucs2(enc_id)
          || lib_enc_is_ucs4(enc_id) );
}

/**
 * Return true if the encoding id is UCS type
 */
static bool lib_enc_is_ucs(int enc_id) {
  return (   lib_enc_is_ucs2(enc_id)
          || lib_enc_is_ucs4(enc_id) );
}  

/**
 * Return true if the encoding id is double bytes type
 */
static bool lib_enc_is_dcs(int enc_id) {
  return (enc_id == LIB_ENC_CP932_ID
       || enc_id == LIB_ENC_CP936_ID
       || enc_id == LIB_ENC_CP949_ID
       || enc_id == LIB_ENC_CP950_ID
    );
}

/**
 * Return true if the encoding id is multi bytes type 
 */
static bool lib_enc_is_mbc(int enc_id) {
    return ( lib_enc_is_utf(enc_id)
          || lib_enc_is_ucs(enc_id)
          || lib_enc_is_dcs(enc_id)
          );
}

/**
 * Return true if the encoding id is misc type
 */
static bool lib_enc_is_msc(int enc_id) {
  return (enc_id == LIB_ENC_KOI8R_ID
       || enc_id == LIB_ENC_KOI8U_ID
    );
}

#endif // PLAZMA_LIB_ENCPRE_H
