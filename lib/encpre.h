#ifndef PLAZMA_LIB_ENCPRE_H
#define PLAZMA_LIB_ENCPRE_H

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

#endif // PLAZMA_LIB_ENCPRE_H
