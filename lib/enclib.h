#ifndef PLAZMA_LIB_ENCLIB_H
#define PLAZMA_LIB_ENCLIB_H

#include "encdef.h"
#include "unimap.h"

/**
 * Converts data by Encoding IDs 
 */
char* lib_enc_conv_by_id(int from_id, int to_id, char* data, size_t len, size_t& to_len);

/**
 * Converts data to UTF-8 by Encoding ID
 */
char* lib_enc_conv_to_utf8_by_id(int from_id, char* data, size_t len, size_t& to_len);

/**
 * Converts data to UTF-8 by Encoding Map
 */
char* lib_enc_conv_to_utf8_by_map(lib_unimap_t* from_map , char* data, size_t len, size_t& to_len);

#endif // PLAZMA_LIB_ENCLIB_H
