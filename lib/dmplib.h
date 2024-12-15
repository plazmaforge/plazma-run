#ifndef PLAZMA_LIB_DMPLIB_H
#define PLAZMA_LIB_DMPLIB_H

#include <stdlib.h>

#define LIB_DMP_MIN_COL_COUNT 16
#define LIB_DMP_MAX_COL_COUNT 32
#define LIB_DMP_DEF_COL_COUNT 16

#define LIB_DMP_DEF_SHOW_TEXT true

typedef struct {
    int col_count;
    bool show_text;
} lib_dmp_config_t;

lib_dmp_config_t* lib_dmp_file_new();

void lib_dmp_file_free(lib_dmp_config_t* config);

int lib_dmp_dump_file_def(const char* file_name, lib_dmp_config_t* config);

int lib_dmp_dump_file(const char* file_name, size_t& size, lib_dmp_config_t* config);

#endif // PLAZMA_LIB_DMPLIB_H