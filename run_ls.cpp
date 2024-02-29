#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "getopt.h"
#include "fslib.h"

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

void printUsage() {
    printf("Usage: run-ls\n");
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

int main(int argc, char *argv[]) {

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'a':
            // TODO
            break;
        case 'l':
            // TODO
            break;
        case '?':
            error = true;
            break;
        }
    }

    char* dir_name = fs_get_current_dir();; //fs_get_current_find_path();
    //printf("Current Dir: %s\n", dir_name);

    char* pattern = NULL;
    fs_file_t** files = NULL;
    fs_file_t* file = NULL;
    int file_count = fs_scandir(dir_name, pattern, &files, FS_SCANDIR_FLAT, false);

    if (file_count <= 0) {
        fs_files_free(files);
        free(dir_name);
        return 0;
    }

    /*
    printf("SIZE_FORMAT_COUNT: %d\n" , SIZE_FORMAT_COUNT);
    printf("KILOBYTE_FACTOR  : %llu\n" , KILOBYTE_FACTOR);
    printf("MEGABYTE_FACTOR  : %llu\n" , MEGABYTE_FACTOR);
    printf("GIGABYTE_FACTOR  : %llu\n" , GIGABYTE_FACTOR);
    printf("TERABYTE_FACTOR  : %llu\n" , TERABYTE_FACTOR);
    printf("PETABYTE_FACTOR  : %llu\n" , PETABYTE_FACTOR);
    printf("EXABYTE_FACTOR   : %llu\n" , EXABYTE_FACTOR);
    */

    bool use_date = true;
    bool use_time = true;
    bool use_size = true;
    bool use_size_first = true;

    int DATE_LEN = 10; // YYYY-MM-DD
    int TIME_LEN = 8;  // HH:MM:SS
    int BUF_LEN = DATE_LEN + 1 + (use_time ? (TIME_LEN + 1) : 0);
    char buf[BUF_LEN];

    for (int i = 0; i < file_count; i++) {
        file = files[i];

        if (!file) {
            continue;
        }

        char* path = file->name;
        char* name = fs_get_base_name(path);

        /* Print Marker  */
        if (fs_file_is_dir(file)) {
            printf("[D]");
        } else {
            printf("   ");
        }
        
        /* Print Size    */
        if (use_size & use_size_first) {
            uint64_t size = fs_file_get_file_size(file);
            format_file_size(size);
        }

        /* Print DateTime */
        if (use_date) {
            time_t time = fs_file_get_file_mtime(file);
            format_file_date_time(time, buf, BUF_LEN, use_time);
        }

        /* Print Size    */
        if (use_size & !use_size_first) {
            uint64_t size = fs_file_get_file_size(file);
            format_file_size(size);
        }

        /* Print Name   */
        printf("%s\n", name);

        free(name);
    }

    fs_files_free(files);                        
    free(dir_name);

    return 0;
}
