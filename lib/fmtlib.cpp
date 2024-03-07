#include <string.h>
#include <stdio.h>

#include "fmtlib.h"

int get_size_format_index(uint64_t size, int min_index) {
    if (size <= KILOBYTE_FACTOR) {
        return -1;
    }
    if (min_index < 0) {
        min_index = 0;
    }
    size_format_t size_format;
    for (int i = SIZE_FORMAT_COUNT - 1; i >= min_index; i--) {
        size_format = SIZE_FORMATS[i];
        if (size > size_format.factor) {
            return i;
        }
    }
    return  -1;
}

void format_file_date_time(time_t time, char* buf, int buf_len, bool use_time) {
    struct tm *timev = localtime(&time);
    memset(buf, 0, buf_len);

    if (use_time) {
        strftime(buf, buf_len, "%Y-%m-%d %H:%M:%S", timev);
    } else {
        strftime(buf, buf_len, "%Y-%m-%d", timev);
    }
    printf("%s ", buf);
}

void format_file_size(uint64_t size) {
    int format_index = get_size_format_index(size, 1);
    const size_format_t *format = format_index >= 0 ? &(SIZE_FORMATS[format_index]) : NULL;

    if (format) {
        double value = (double)size / (double)format->factor;
        const char *unit = format->unit;
        printf("%12.1f %s ", value, unit);
    } else {
        printf("%12llu    ", size);
    }
}
