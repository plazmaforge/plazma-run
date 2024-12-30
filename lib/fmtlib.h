#ifndef PLAZMA_LIB_FMTLIB_H
#define PLAZMA_LIB_FMTLIB_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define LIB_FMT_KILOBYTE_FACTOR ((uint64_t) 1024)
#define LIB_FMT_MEGABYTE_FACTOR (LIB_FMT_KILOBYTE_FACTOR * LIB_FMT_KILOBYTE_FACTOR)
#define LIB_FMT_GIGABYTE_FACTOR (LIB_FMT_MEGABYTE_FACTOR * LIB_FMT_KILOBYTE_FACTOR)
#define LIB_FMT_TERABYTE_FACTOR (LIB_FMT_GIGABYTE_FACTOR * LIB_FMT_KILOBYTE_FACTOR)
#define LIB_FMT_PETABYTE_FACTOR (LIB_FMT_TERABYTE_FACTOR * LIB_FMT_KILOBYTE_FACTOR)
#define LIB_FMT_EXABYTE_FACTOR  (LIB_FMT_PETABYTE_FACTOR * LIB_FMT_KILOBYTE_FACTOR)

typedef struct {
    uint64_t factor;
    const char* unit;    
} lib_fmt_size_format_t;

lib_fmt_size_format_t lib_fmt_get_size_format(int index);

int lib_fmt_get_size_format_index(uint64_t size, int min_index);

int lib_fmt_print_file_date_time(time_t time, char* buf, int buf_len, bool use_time);

int lib_fmt_print_file_size(uint64_t size);

int lib_fmt_print_file_size_fixed(uint64_t size, int len);

#endif // PLAZMA_LIB_FMTLIB_H