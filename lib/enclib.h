#ifndef PLAZMA_LIB_ENCLIB_H
#define PLAZMA_LIB_ENCLIB_H

#include <stdbool.h>

#include "encdef.h"
#include "unimap.h"

#define LIB_ENC_ERR_CONV_USUPPORTED           -2000
#define LIB_ENC_ERR_CONV_FROM_USUPPORTED      -2001
#define LIB_ENC_ERR_CONV_TO_USUPPORTED        -2002

typedef struct lib_enc_context_t {

    /* From                */
    int from_id;
    char* from_data;
    size_t from_len;
    lib_unimap_t* from_map;
    bool from_is_utf;

    /* To                  */
    int to_id;    
    char** to_data;
    size_t* to_len;
    lib_unimap_t* to_map;
    bool to_is_utf;

} lib_enc_context_t;

/**
 * Return lenght of BOM
 */
size_t lib_enc_bom_len(int enc_id);

/**
 * Set BOM and return lenght of BOM
 */
size_t lib_enc_set_bom(int enc_id, char* str);

/**
 * Return true if the Encoding ID supports conversion
 */
bool lib_enc_supports_conv(int enc_id);

/**
 * Return (conversion only) encoding id by encoding name 
 */
int lib_enc_get_conv_encoding_id(const char* name);

/**
 * Convert data by Encoding IDs 
 */
int lib_enc_conv_by_id(int from_id, int to_id, char* from_data, size_t from_len, char** to_data, size_t* to_len);

int lib_enc_test();

#endif // PLAZMA_LIB_ENCLIB_H
