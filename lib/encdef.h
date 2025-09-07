#ifndef PLAZMA_LIB_ENCDEF_H
#define PLAZMA_LIB_ENCDEF_H

#include <stdbool.h>

#define LIB_ENC_ISO_TYPE 1
#define LIB_ENC_DOS_TYPE 2
#define LIB_ENC_WIN_TYPE 3
#define LIB_ENC_UTF_TYPE 10
#define LIB_ENC_UCS_TYPE 11

#define LIB_ENC_UTF7_ID         65000

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

typedef struct lib_encoding_t {
    int id;                  /* 437           , 1251               */
    const char* name;        /* 'CP437'       , 'WINDOWS-1251'     */
    const char* description; /* 'DOS Latin US', 'Windows Cyrillic' */
    const char* alias;       /* alias or aliases                   */
} lib_encoding_t;


/**
 * Returns size of encoding 
 */
size_t lib_enc_get_encoding_size();

/**
 * Returns encoding by index 
 */
lib_encoding_t lib_enc_get_encoding(size_t index);

bool lib_enc_is_iso_encoding(int id);

bool lib_enc_is_dos_encoding(int id);

bool lib_enc_is_win_encoding(int id);

/**
 * Returns true if the encoding id is UTF type 
 */
bool lib_enc_is_utf_encoding(int id);

/**
 * Returns true if the encoding id is 
 * UTF-16/16BE/18LE or UTF-32/32BE/32LE type 
 */
bool lib_enc_is_utf16or32_encoding(int id);

/**
 * Returns true if the encoding id is UCS type 
 */
bool lib_enc_is_ucs_encoding(int id);

/**
 * Converts DOS encoding id to WIN encoding id
 */
int lib_enc_dos_to_win(int id);


/**
 * Returns encoding type by encoding id
 */
int lib_enc_get_encoding_type(int id);

/**
 * Returns name of encoding type by encoding id
 */
const char* lib_enc_get_encoding_type_name(int id);


/**
 * Returns encoding id by encoding name
 */
int lib_enc_get_encoding_id(const char* name);

void lib_enc_print_encodings();


#endif // PLAZMA_LIB_ENCDEF_H
