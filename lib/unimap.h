#ifndef PLAZMA_LIB_UNIMAP_H
#define PLAZMA_LIB_UNIMAP_H

#define LIB_UNIMAP_ERR_MAP_ALL_USUPPORTED   -1112
#define LIB_UNIMAP_ERR_MAP_FROM_USUPPORTED  -11
#define LIB_UNIMAP_ERR_MAP_TO_USUPPORTED    -12

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

#endif // PLAZMA_LIB_UNIMAP_H
