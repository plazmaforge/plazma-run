#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "getopt.h"
#include "fslib.h"
#include "iodef.h"
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

    lib_io_buf_init();

    // #ifdef _WIN32
    // char _buf[65536];
    // setvbuf(stdout, _buf, _IOFBF, 65536);
    // #endif


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

    int pos = 0;
    int stat_pos = -1;
    int total = 0;

    size_t buf_size = lib_io_stdout_get_buf_size();
    buf_size = lib_io_get_buf_page(buf_size);

    //printf("buf size: %lu\n", buf_size);
    //fflush(stdout);
    //int max = 0;

    for (int i = 0; i < file_count; i++) {
        file = files[i];

        if (!file) {
            continue;
        }

        pos = 0;
        char* path = file->name;
        char* name = lib_fs_get_base_name(path);
        int name_len = strlen(name);

        if (buf_size > 0 && i > 0) {
            int try_pos = (stat_pos > 0 ? stat_pos : 0) + name_len;
            if (total + try_pos > buf_size) {
                fflush(stdout);
                total = 0;
                //pos = printf("%s\n", "FLUSH!!");
            }
        }

        /* Print Marker  */
        if (lib_fs_file_is_dir(file)) {
            pos += printf("[D]");
        } else {
            pos += printf("   ");
        }
        pos++;
        
        /* Print Size    */
        if (use_size & use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += format_file_size(size);
            pos++;
        }

        /* Print DateTime */
        if (use_date) {
            time_t time = lib_fs_file_get_file_mtime(file);
            pos += format_file_date_time(time, buf, BUF_LEN, use_time);
            pos++;
        }

        /* Print Size    */
        if (use_size & !use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += format_file_size(size);
            pos++;
        }

        if (stat_pos < 0) {
            stat_pos = pos;
            if (stat_pos < 0) {
                stat_pos = 0;
            }
        }

        /* Print Name   */
        pos += printf("%s\n", name);
        total += pos;
        //if (pos > max) {
        //    max = pos;
        //}

        free(name);
    }

    //printf("\n\nmax: %i\n", max);

    ////

    /*
    fflush(stdout);
    int num = 0;
    int maxcur = 0;
    for (int i = 0; i < 10000; i++) {
        int cur = 0;
        int tot = 0;
        if (num > buf_size) {
            num = 0;
            fflush(stdout);
            cur = printf("%s\n", "FLUSH!");
        }
        cur += printf("\n\nnum: %i\n", num);
        cur += printf("%i: Test Line\n", i + 1);
        cur += printf("cur: %i\n", cur);
        tot = num + cur;
        cur += printf("tot: %i\n", tot);
        num += cur;
        if (cur > maxcur) {
            maxcur = cur;
        }        
    }
    fflush(stdout);
    printf("\n\nmax: %i\n", maxcur);
    */

    ////

    lib_fs_files_free(files);                        
    free(dir_name);

    return 0;
}
