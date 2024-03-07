#ifndef PLAZMA_LIB_FMTLIB_H
#define PLAZMA_LIB_FMTLIB_H

#include <stdlib.h>
#include <time.h>

#define KILOBYTE_FACTOR ((uint64_t) 1024)
#define MEGABYTE_FACTOR (KILOBYTE_FACTOR * KILOBYTE_FACTOR)
#define GIGABYTE_FACTOR (MEGABYTE_FACTOR * KILOBYTE_FACTOR)
#define TERABYTE_FACTOR (GIGABYTE_FACTOR * KILOBYTE_FACTOR)
#define PETABYTE_FACTOR (TERABYTE_FACTOR * KILOBYTE_FACTOR)
#define EXABYTE_FACTOR  (PETABYTE_FACTOR * KILOBYTE_FACTOR)

typedef struct {
    uint64_t factor;
    const char* unit;    
} size_format_t;

const size_format_t SIZE_FORMATS[6] = {
    {KILOBYTE_FACTOR, "Kb"},
    {MEGABYTE_FACTOR, "Mb"},
    {GIGABYTE_FACTOR, "Gb"},
    {TERABYTE_FACTOR, "Tb"},
    {PETABYTE_FACTOR, "Pb"},
    {EXABYTE_FACTOR, "Eb"}
    };

const int SIZE_FORMAT_COUNT = sizeof(SIZE_FORMATS) / sizeof(size_format_t);

int get_size_format_index(uint64_t size, int min_index);

void format_file_date_time(time_t time, char* buf, int buf_len, bool use_time);

void format_file_size(uint64_t size);

#endif // PLAZMA_LIB_FMTLIB_H