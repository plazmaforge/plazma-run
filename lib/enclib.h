#ifndef PLAZMA_LIB_ENCLIB_H
#define PLAZMA_LIB_ENCLIB_H

#include "encdef.h"
#include "unimap.h"

#define LIB_ENC_ERR_CONV_USUPPORTED           -2000
#define LIB_ENC_ERR_CONV_FROM_USUPPORTED      -2001
#define LIB_ENC_ERR_CONV_TO_USUPPORTED        -2002

/**
 * Converts data by Encoding IDs 
 */
int lib_enc_conv_by_id(int from_id, int to_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-8 by Encoding ID
 */
int lib_enc_conv_to_utf8_by_id(int from_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-8 by Encoding Map
 */
int lib_enc_conv_to_utf8_by_map(lib_unimap_t* from_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len);

#endif // PLAZMA_LIB_ENCLIB_H
