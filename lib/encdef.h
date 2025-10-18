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
size_t lib_enc_get_size();

/**
 * Returns encoding by index 
 */
lib_encoding_t lib_enc_get_encoding(size_t index);

//bool lib_enc_is_iso(int enc_id);

bool lib_enc_is_dos(int enc_id);

bool lib_enc_is_win(int enc_id);

/**
 * Returns true if the encoding id is multi bytes type 
 */
bool lib_enc_is_mbc(int enc_id);

/**
 * Returns true if the encoding id is UTF type 
 */
bool lib_enc_is_utf(int enc_id);

/**
 * Returns true if the encoding id is UCS type 
 */
bool lib_enc_is_ucs(int enc_id);

/**
 * Converts DOS encoding id to WIN encoding id
 */
int lib_enc_dos_to_win(int enc_id);


/**
 * Returns encoding type by encoding id
 */
int lib_enc_get_type(int enc_id);

/**
 * Returns name of encoding type by encoding id
 */
const char* lib_enc_get_type_name(int enc_id);


/**
 * Returns encoding id by encoding name
 */
int lib_enc_get_id(const char* name);

bool lib_enc_has_id(int enc_id);

lib_encoding_t lib_enc_get_by_id(int enc_id);

lib_encoding_t lib_enc_get_by_name(const char* name);

void lib_enc_print_encodings();


#endif // PLAZMA_LIB_ENCDEF_H
