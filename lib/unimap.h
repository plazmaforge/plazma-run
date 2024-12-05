#ifndef PLAZMA_LIB_UNIMAP_H
#define PLAZMA_LIB_UNIMAP_H

typedef struct lib_unimap_t {
    int* map;     /* data  map       */
    size_t start; /* start index     */
    size_t len;   /* lenght of array */
} lib_unimap_t;

int* lib_unimap_get_map_id(int id);

int lib_unimap_get_unimap_id(lib_unimap_t* unimap, int id);

int lib_unimap_conv_id(int from, int to, char* data, size_t len);

#endif // PLAZMA_LIB_UNIMAP_H
