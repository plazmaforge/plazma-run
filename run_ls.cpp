#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "getopt.h"
#include "fslib.h"
#include "fmtlib.h"

void usage() {
    fprintf(stderr, "Usage: run-ls\n");
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

    char* dir_name = lib_fs_get_current_dir();; //fs_get_current_find_path();
    //printf("Current Dir: %s\n", dir_name);

    char* pattern = NULL;
    lib_fs_file_t** files = NULL;
    lib_fs_file_t* file = NULL;
    int file_count = lib_fs_scandir(dir_name, pattern, &files, LIB_FS_SCANDIR_FLAT, false);

    if (file_count <= 0) {
        lib_fs_files_free(files);
        free(dir_name);
        return 0;
    }

    #ifdef _WIN32
    char _buf[65536];
    setvbuf(stdout, _buf, _IOFBF, 65536);
    #endif


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
        char* name = lib_fs_get_base_name(path);

        /* Print Marker  */
        if (lib_fs_file_is_dir(file)) {
            printf("[D]");
        } else {
            printf("   ");
        }
        
        /* Print Size    */
        if (use_size & use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            format_file_size(size);
        }

        /* Print DateTime */
        if (use_date) {
            time_t time = lib_fs_file_get_file_mtime(file);
            format_file_date_time(time, buf, BUF_LEN, use_time);
        }

        /* Print Size    */
        if (use_size & !use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            format_file_size(size);
        }

        /* Print Name   */
        printf("%s\n", name);

        free(name);
    }

    lib_fs_files_free(files);                        
    free(dir_name);

    return 0;
}
