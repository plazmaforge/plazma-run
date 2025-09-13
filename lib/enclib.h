#ifndef PLAZMA_LIB_ENCLIB_H
#define PLAZMA_LIB_ENCLIB_H

#include <stdbool.h>

#include "encdef.h"
#include "unimap.h"

#define LIB_ENC_ERR_CONV_USUPPORTED           -2000
#define LIB_ENC_ERR_CONV_FROM_USUPPORTED      -2001
#define LIB_ENC_ERR_CONV_TO_USUPPORTED        -2002


int lib_enc_bom_len(int id);

int lib_enc_set_bom(int id, char* str);

/**
 * Returns true if the Encoding ID supports conversion
 */
bool lib_enc_supports_conv(int id);

/**
 * Returns (conversion only) encoding id by encoding name 
 */
int lib_enc_get_conv_encoding_id(const char* name);

/**
 * Converts data by Encoding IDs 
 */
int lib_enc_conv_by_id(int from_id, int to_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-8 by Encoding ID
 */
int lib_enc_conv_to_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

int lib_enc_conv_from_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-8 by Encoding Map
 */
int lib_enc_conv_to_utf8_by_map(struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-8 by Encoding Map
 */
int lib_enc_conv_from_utf8_by_map(struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

//// UTF

int lib_enc_conv_to_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);


/**
 * Converts data from UTF-[ID] by Encoding Map
 */
int lib_enc_conv_from_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-[ID] by Encoding ID
 */
int lib_enc_conv_to_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-[ID] by Encoding ID
 */
int lib_enc_conv_from_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-[ID] to UTF-[ID]
 */
int lib_enc_conv_utf2utf(int from_id, int to_id, char* from_data, size_t from_len, char** to_data, size_t* to_len);

int lib_enc_test();

#endif // PLAZMA_LIB_ENCLIB_H
