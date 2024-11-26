// Windows: Important lib order for UTF-8 (65001)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "getopt.h"
#include "fslib.h"
#include "iodef.h"
#include "fmtlib.h"
#include "strlib.h"

void usage() {
    fprintf(stderr, "Usage: run-ls\n");
}

int _lib_len_counter(int val) {
    if (val == 0) {
        return 0;
    }
    long num = val;
    int len = 0;
    if(val < 0) {
        num *= -1;
    }        
    while(num != 0) {
        num = num / 10;
        len++;
    }
    return len;
}

int _lib_len_counter_u64(uint64_t val) {
    if (val == 0) {
        return 0;
    }
    long num = val;
    int len = 0;
    //if(val < 0) {
    //    num *= -1;
    //}        
    while(num != 0) {
        num = num / 10;
        len++;
    }
    return len;
}

char* _lib_itoa(int val) {
    if(val == 0) {
        return lib_strdup("0");
    }
        
    char* ret = lib_strnew(100);
    int len = _lib_len_counter(val);

    if (val < 0) {
        len++;
        ret[0] = '-';
        val *= -1;
    }

    while (val != 0) {
        ret[len - 1] = val % 10 + 48;
        val = val / 10;
        len--;
    }
    return ret;
}

////

bool _is_heap(void* ptr) {
    int x;
    uint64_t* pi = (uint64_t*) ptr;
    uint64_t* px = (uint64_t*) &x;
    return (pi < px);
}

////

/*
 -1: Not pretty format - bytes only
  0: Pretty format     - start with 'Kb'
  1: Flex pretty       - start with 'Mb' 
*/
static int min_size_format = 1;

const lib_fmt_size_format_t* _get_size_format(uint64_t size) {
    if (min_size_format < 0) {
        return NULL; // No format. It is not pretty format. Size in bytes only
    }
    int index = lib_fmt_get_size_format_index(size, min_size_format);
    return index >= 0 ? &(LIB_FMT_SIZE_FORMATS[index]) : NULL;
}

double _get_unit_size(uint64_t size, const lib_fmt_size_format_t* format) {
    return (double) size / (double) format->factor;
}

uint64_t _get_unit_isize(uint64_t size, const lib_fmt_size_format_t* format) {
    return (uint64_t) _get_unit_size(size, format);
}

int _print_size(uint64_t size, int len) {
    const lib_fmt_size_format_t* format = _get_size_format(size);

    if (format) {
        double value = _get_unit_size(size, format);
        const char* unit = format->unit;
        return printf("%*.1f%c ", len + 2, value, unit[0]);
    } else {
        return printf("%*llu  ", len + 2, size);
        //return printf("%*lluB ", len + 2, size);
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

    char* dir_name = lib_fs_get_current_dir(); //fs_get_current_find_path();
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

    //lib_io_buf_init();

    bool use_mode = true;
    bool use_nlink = true;
    bool use_uname = true;
    bool use_gname = true;
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

    size_t buf_size = 0; //lib_io_stdout_get_buf_size();
    //buf_size = lib_io_get_buf_page(buf_size);

    //printf("buf size: %lu\n", buf_size);
    //fflush(stdout);
    //int max = 0;

    //int err = 0;
    //if ((err = fflush(stdout)) != 0) {
    //   fprintf(stderr, "error-1: %d\n", err);
    //   perror("fflush error\n");
    //}

    struct file_entry_t {
        lib_fs_file_t* file;
        char* path;
        char* name;
        int nlink;
        char* uname;
        char* gname;
        //
        int nlink_len;
        int name_len;
        int uname_len;
        int gname_len;
    };

    file_entry_t* entry = NULL;
    file_entry_t* entries[file_count];

    int max_nlink_len = 0;
    int max_name_len = 0;
    int max_uname_len = 0;
    int max_gname_len = 0;
    int max_size_len = 0;

    for (int i = 0; i < file_count; i++) {
        file = files[i];
        entry = (file_entry_t*) malloc(sizeof(file_entry_t)); 
        entries[i] = entry;

        if (!file) {
            continue;
        }

        entry->file = file;
        entry->path = file->name;
        entry->name = lib_fs_get_base_name(entry->path); // allocate
        entry->nlink = file->stat->st_nlink;
        entry->uname = lib_fs_file_get_uname(file);
        entry->gname = lib_fs_file_get_gname(file);

        entry->name_len = entry->name ? strlen(entry->name) : 0;
        entry->nlink_len = _lib_len_counter(entry->nlink);
        entry->uname_len = entry->uname ? strlen(entry->uname) : 0;
        entry->gname_len = entry->gname ? strlen(entry->gname) : 0;

        uint64_t size = lib_fs_file_get_file_size(file);
        int size_len = _lib_len_counter_u64(size);

        const lib_fmt_size_format_t* format = _get_size_format(size);
        if (format) {
            uint64_t unit_size = _get_unit_isize(size, format);
            int unit_size_len = _lib_len_counter_u64(unit_size);

            //printf("name: %s\t, size: %llu, size_len: %d, unit_size: %llu, unit_size_len: %d\n"
            //, entry->name, size, size_len, unit_size, unit_size_len);

            size_len = unit_size_len;
        } else {
            //printf("name: %s\t, size: %llu, size_len: %d\n"
            //, entry->name, size, size_len);
        }
        //printf(">>size_len: %d\n", size_len);

        if (entry->name_len > max_name_len) {
            max_name_len = entry->name_len;
        }
        if (entry->nlink_len > max_nlink_len) {
            max_nlink_len = entry->nlink_len;
        }
        if (entry->uname_len > max_uname_len) {
            max_uname_len = entry->uname_len;
        }
        if (entry->gname_len > max_gname_len) {
            max_gname_len = entry->gname_len;
        }

        if (size_len > max_size_len) {
            max_size_len = size_len;
        }

    }

    //printf("max_name_len  : %d\n", max_name_len);
    //printf("max_nlink_len : %d\n", max_nlink_len);
    //printf("max_uname_len : %d\n", max_uname_len);
    //printf("max_gname_len : %d\n\n", max_gname_len);
    //printf("max_size_len : %d\n\n", max_size_len);

    for (int i = 0; i < file_count; i++) {

        entry = entries[i];
        file = entry->file; // files[i];

        if (!file) {
            continue;
        }

        pos = 0;
        char* name = entry->name;
        int name_len = entry->name_len;

        if (buf_size > 0 && i > 0) {
            int try_pos = (stat_pos > 0 ? stat_pos : 0) + name_len;
            if (total + try_pos > buf_size) {
                fflush(stdout);
                //pos = printf("%s: %d, %d, %d, %d\n", "FLUSH (!)", total, try_pos, stat_pos, name_len);
                total = 0;
            }
        }

        /* Print Mode      */
        if (use_mode) {
            char mode[11 + 1];
            memset(mode, '-', 10);
            mode[10] = ' ';
            mode[11] = '\0';

            lib_fs_file_add_attr(file, mode);
            pos += printf("%s ", mode);
        }
        
        /* Print NLink      */
        if (use_nlink) {
            int nlink = entry->nlink;
            pos += printf("%*d ", max_nlink_len, nlink);
        }

        /* Print User Name  */
        if (use_uname) {
            char* uname = entry->uname;
            pos += printf("%-*s  ", max_uname_len, uname);
        }

        /* Print Group Name */
        if (use_uname) {
            char* gname = entry->gname;
            pos += printf("%-*s ", max_gname_len, gname);
        }
        
        /* Print Size    */
        if (use_size & use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += _print_size(size, max_size_len);
        }

        /* Print DateTime */
        if (use_date) {
            time_t time = lib_fs_file_get_file_mtime(file);
            pos += lib_fmt_print_file_date_time(time, buf, BUF_LEN, use_time);
        }

        /* Print Size    */
        if (use_size & !use_size_first) {
            uint64_t size = lib_fs_file_get_file_size(file);
            pos += _print_size(size, max_size_len);
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
        free(entry);
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
    //err = 0;
    //if ((err = fflush(stdout)) != 0 ) {
    //   fprintf(stderr, "error-2: %d\n", err);
    //   perror("fflush error\n");
    //}

    //fclose(stdout);
    //fseek(stdout, 0, SEEK_SET);
    //fflush(stdout);
    //printf("\nTOTAL FLUSH (!)\n");

    lib_fs_files_free(files);                        
    free(dir_name);

    return 0;
}
