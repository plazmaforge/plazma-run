#include <string.h>
#include <stdio.h>

#include "fmtlib.h"

int lib_fmt_get_size_format_index(uint64_t size, int min_index) {
    if (size <= LIB_FMT_KILOBYTE_FACTOR) {
        return -1;
    }
    if (min_index < 0) {
        min_index = 0;
    }
    lib_fmt_size_format_t size_format;
    for (int i = LIB_FMT_SIZE_FORMAT_COUNT - 1; i >= min_index; i--) {
        size_format = LIB_FMT_SIZE_FORMATS[i];
        if (size > size_format.factor) {
            return i;
        }
    }
    return  -1;
}

int lib_fmt_print_file_date_time(time_t time, char* buf, int buf_len, bool use_time) {
    if (!buf) {
        return 0;
    }
    struct tm *timev = localtime(&time);
    memset(buf, 0, buf_len);

    if (use_time) {
        strftime(buf, buf_len, "%Y-%m-%d %H:%M:%S", timev);
    } else {
        strftime(buf, buf_len, "%Y-%m-%d", timev);
    }
    return printf("%s ", buf);
}

int lib_fmt_print_file_size(uint64_t size) {
    int index = lib_fmt_get_size_format_index(size, 1);
    const lib_fmt_size_format_t* format = index >= 0 ? &(LIB_FMT_SIZE_FORMATS[index]) : NULL;

    if (format) {
        double value = (double) size / (double) format->factor;
        const char* unit = format->unit;
        return printf("%12.1f %s ", value, unit);
    } else {
        return printf("%12llu    ", size);
    }
}
