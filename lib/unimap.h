#ifndef PLAZMA_LIB_UNIMAP_H
#define PLAZMA_LIB_UNIMAP_H

#include <stdbool.h>

#define LIB_UNIMAP_ERR_CONV_USUPPORTED       -1000
#define LIB_UNIMAP_ERR_CONV_FROM_USUPPORTED  -1001
#define LIB_UNIMAP_ERR_CONV_TO_USUPPORTED    -1002

typedef struct lib_unimap_t {
    int id;       /* encoding id     */
    int* map;     /* data  map       */
    size_t start; /* start index     */
    size_t len;   /* lenght of array */
} lib_unimap_t;

/**
 * Returns unimap array by Encoding ID 
 */
int* lib_unimap_get_map_by_id(int id);

/**
 * Loads unimap by Encoding ID
 */
int lib_unimap_get_unimap_by_id(lib_unimap_t* unimap, int id);

/**
 * Converts data by Encoding IDs 
 * b2b convert only
 */
int lib_unimap_conv_by_id(int from_id, int to_id, char* data, size_t len);

/**
 * Converts data by Encoding Maps 
 * b2b convert only
 */
int lib_unimap_conv_by_map(lib_unimap_t* from_map, lib_unimap_t* to_map, char* data, size_t len);

/**
 * Returns true if the Encoding ID supports conversion map 
 */
bool lib_unimap_supports_map(int id);

/**
 * Return UTF-8 code by map and code 
 */
int lib_unimap_get_ucode(lib_unimap_t* from_map, int icode);

////

int lib_unimap_conv_icode(lib_unimap_t* conv_map, int icode);

int lib_unimap_conv_ucode(lib_unimap_t* conv_map, int ucode);

#endif // PLAZMA_LIB_UNIMAP_H
