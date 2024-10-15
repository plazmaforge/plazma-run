#ifndef PLAZMA_LIB_DUMPLIB_H
#define PLAZMA_LIB_DUMPLIB_H

#include <stdlib.h>

#define LIB_DUMP_MIN_COL_COUNT 16
#define LIB_DUMP_MAX_COL_COUNT 32
#define LIB_DUMP_DEF_COL_COUNT 16

#define LIB_DUMP_DEF_SHOW_TEXT true

typedef struct {
    int col_count;
    bool show_text;
} lib_dump_config_t;

void lib_dump_file_def(const char* file_name, lib_dump_config_t* config);

void lib_dump_file(const char* file_name, size_t& size, lib_dump_config_t* config);

#endif // PLAZMA_LIB_DUMPLIB_H