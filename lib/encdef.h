#ifndef PLAZMA_LIB_ENCDEF_H
#define PLAZMA_LIB_ENCDEF_H

#include <stdbool.h>
#include "encpre.h"

#define LIB_ENC_ISO_TYPE 1
#define LIB_ENC_DOS_TYPE 2
#define LIB_ENC_WIN_TYPE 3
#define LIB_ENC_UTF_TYPE 10
#define LIB_ENC_UCS_TYPE 11

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
bool lib_enc_is_utf_encoding(int enc_id);

bool lib_enc_is_utf7_encoding(int enc_id);

bool lib_enc_is_utf8_encoding(int enc_id);

bool lib_enc_is_utf16_encoding(int enc_id);

bool lib_enc_is_utf32_encoding(int enc_id);

/**
 * Returns true if the encoding id is 
 * UTF-16/16BE/18LE or UTF-32/32BE/32LE type 
 */
bool lib_enc_is_utf16or32_encoding(int enc_id);

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
