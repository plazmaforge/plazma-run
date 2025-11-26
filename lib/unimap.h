#ifndef PLAZMA_LIB_UNIMAP_H
#define PLAZMA_LIB_UNIMAP_H

#include <stdbool.h>

#define NO_CHR 0xFFFD
#define NO_DAT '?'

#define LIB_UNIMAP_ERR_CONV_USUPPORTED       -1000
#define LIB_UNIMAP_ERR_CONV_FROM_USUPPORTED  -1001
#define LIB_UNIMAP_ERR_CONV_TO_USUPPORTED    -1002

typedef struct lib_unimap_t {
    int id;            /* encoding id      */
    int* map;          /* data  map        */
    size_t start;      /* start index      */
    size_t len;        /* lenght of array  */
    size_t lead_start; /* lead start index */
    size_t lead_end;   /* lead end index   */  
} lib_unimap_t;

/**
 * Return unimap array by Encoding ID 
 */
int* lib_unimap_get_map_by_id(int enc_id);

/**
 * Load unimap by Encoding ID
 */
int lib_unimap_get_unimap_by_id(lib_unimap_t* unimap, int enc_id);

/**
 * Convert data by Encoding IDs 
 * b2b convert only
 */
int lib_unimap_conv_by_id(int from_id, int to_id, char* data, size_t len);

/**
 * Convert data by Encoding Maps 
 * b2b convert only
 */
int lib_unimap_conv_by_map(lib_unimap_t* from_map, lib_unimap_t* to_map, char* data, size_t len);

/**
 * Return true if the Encoding ID supports conversion map 
 */
bool lib_unimap_supports_map(int enc_id);

////

int lib_unimap_conv_icode(lib_unimap_t* conv_map, int icode);

int lib_unimap_conv_ucode(lib_unimap_t* conv_map, int ucode);

#endif // PLAZMA_LIB_UNIMAP_H
