#ifndef PLAZMA_LIB_DMPLIB_H
#define PLAZMA_LIB_DMPLIB_H

#include <stdlib.h>
#include <stdbool.h>

#define LIB_DMP_MIN_COL_COUNT 16
#define LIB_DMP_MAX_COL_COUNT 32
#define LIB_DMP_DEF_COL_COUNT 16

#define LIB_DMP_DEF_SHOW_TEXT true

typedef struct {
    int col_count;
    bool show_text;
} lib_dmp_config_t;

int lib_dmp_dump_file_all(lib_dmp_config_t* config, const char* file_name);

int lib_dmp_dump_file(lib_dmp_config_t* config, const char* file_name, size_t size);

#endif // PLAZMA_LIB_DMPLIB_H