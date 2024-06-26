#ifndef PLAZMA_LIB_DUMPLIB_H
#define PLAZMA_LIB_DUMPLIB_H

#include <stdlib.h>

#define DUMP_MIN_COL_COUNT 16
#define DUMP_MAX_COL_COUNT 32
#define DUMP_DEF_COL_COUNT 16

#define DUMP_DEF_SHOW_TEXT true

typedef struct {
    int col_count;
    bool show_text;
} dump_config_t;

void dump_file_def(const char* file_name, dump_config_t* config);

void dump_file(const char* file_name, size_t& size, dump_config_t* config);

#endif // PLAZMA_LIB_DUMPLIB_H