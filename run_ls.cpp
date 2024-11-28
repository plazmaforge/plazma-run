// Windows: Important lib order for UTF-8 (65001)
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#include "getopt.h"
#include "fslib.h"
#include "iodef.h"
#include "fmtlib.h"
#include "strlib.h"
#include "syslib.h"

int RUN_LS_FORMAT_DEF  = 0; // by default
int RUN_LS_FORMAT_TAB  = 1; // Table with value separator: | 
int RUN_LS_FORMAT_CSV  = 2; // CSV
int RUN_LS_FORMAT_TSV  = 3; // TSV
int RUN_LS_FORMAT_XML  = 4; // XML
int RUN_LS_FORMAT_JSON = 5; // JSON

typedef struct run_ls_config {
    bool is_all;
    bool is_long;
    bool is_human;
    int format;
    //
    bool use_mode;
    bool use_nlink;
    bool use_uname;
    bool use_gname;
    //bool use_date;
    bool use_time;
    bool use_size;
    bool use_size_first;

    /*
     -1: Not pretty format - bytes only
      0: Pretty format     - start with 'Kb'
      1: Flex pretty       - start with 'Mb' 
    */
    int min_size_format;
    const char* value_separator;

} run_ls_config;

static const int DATE_PART_LEN = 10; // YYYY-MM-DD
static const int TIME_PART_LEN = 8;  // HH:MM:SS
//static const int TIME_BUF_LEN = DATE_PART_LEN + 1 + (config->use_time ? (TIME_PART_LEN + 1) : 0);
static const int TIME_BUF_LEN = DATE_PART_LEN + 1 + TIME_PART_LEN + 1; // (+1): With '\0'

typedef struct run_ls_context {
    run_ls_config* config;
    //
    int max_nlink_len;
    int max_name_len;
    int max_uname_len;
    int max_gname_len;
    int max_size_len;
    //
    int pos;
    int stat_pos;
    int total;
    //
    size_t buf_size;
    //
    char time_buf[TIME_BUF_LEN];
} run_config;

typedef struct file_entry_t {
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
} file_entry_t;

void run_ls_config_set_columns(run_ls_config* config, bool flag) {
    config->use_mode       = flag;
    config->use_nlink      = flag;
    config->use_uname      = flag;
    config->use_gname      = flag;
    //config->use_date       = flag;
    config->use_time       = flag;
    config->use_size       = flag;
}

void run_ls_config_init(run_ls_config* config) {
    config->is_all         = true;
    config->is_long        = true;
    config->is_human       = false;
    config->format         = RUN_LS_FORMAT_DEF;
    //
    run_ls_config_set_columns(config, true);
    config->use_size_first = true;
    config->min_size_format = 1;
    //
    config->value_separator = " ";
}

void run_ls_context_init(run_ls_context* context) {
    context->max_nlink_len = 0;
    context->max_name_len  = 0;
    context->max_uname_len = 0;
    context->max_gname_len = 0;
    context->max_size_len  = 0;
    //
    context->pos = 0;
    context->stat_pos = -1;
    context->total = 0;
    //
    context->buf_size = 0;
}

void usage() {
    fprintf(stderr, "Usage: run-ls\n");
}

static int _lib_len_counter(int val) {
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

static int _lib_len_counter_u64(uint64_t val) {
    if (val == 0) {
        return 0;
    }
    uint64_t num = val;
    int len = 0;
    while(num != 0) {
        num = num / 10;
        len++;
    }
    return len;
}

static char* _lib_itoa(int val) {
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

static bool _is_heap(void* ptr) {
    int x;
    uint64_t* pi = (uint64_t*) ptr;
    uint64_t* px = (uint64_t*) &x;
    return (pi < px);
}

////

static int _get_format(char* val) {
    if (!val) {
        return 0;
    }

    if (strcmp(val, "csv") == 0) {
        return RUN_LS_FORMAT_CSV;
    } else if (strcmp(optarg, "tsv") == 0) {
        return RUN_LS_FORMAT_TSV;
    } else if (strcmp(optarg, "xml") == 0) {
        return RUN_LS_FORMAT_XML;
    } else if (strcmp(optarg, "json") == 0) {
        return RUN_LS_FORMAT_JSON;
    } else if (strcmp(optarg, "tab") == 0) {
        return RUN_LS_FORMAT_TAB;
    }
    return 0;
}

static const lib_fmt_size_format_t* _get_size_format(run_ls_context* context, uint64_t size) {
    int min_size_format = context->config->min_size_format;
    if (min_size_format < 0) {
        return NULL; // No format. It is not pretty format. Size in bytes only
    }
    int index = lib_fmt_get_size_format_index(size, min_size_format);
    return index >= 0 ? &(LIB_FMT_SIZE_FORMATS[index]) : NULL;
}

static double _get_unit_size(uint64_t size, const lib_fmt_size_format_t* format) {
    return (double) size / (double) format->factor;
}

static uint64_t _get_unit_isize(uint64_t size, const lib_fmt_size_format_t* format) {
    return (uint64_t) _get_unit_size(size, format);
}

//// Print Value

static int _print_size_txt(run_ls_context* context, uint64_t size) {
    return printf("%llu", size);
}

static int _print_size_fix(run_ls_context* context, uint64_t size) {

    // Get max len
    int len = context->max_size_len;

    // In bytes only without unit
    //if (!context->config->is_human) {
    //    return printf("%*llu", len, size);
    //}

    // Find size format
    const lib_fmt_size_format_t* format = _get_size_format(context, size);

    if (format) {
        double value = _get_unit_size(size, format);
        const char* unit = format->unit;
        return printf("%*.1f%c", len + 2, value, unit[0]); // (+2) for '.1'
    } else {
        return printf("%*llu ", len + 2, size);            // (+2) for '.1'
        //return printf("%*lluB ", len + 2, size);
    }
}

//// Print Line CSV ////

static int _print_quote(run_ls_context* context) {
    return printf("\"");
}

static int _print_value_separator(run_ls_context* context) {
    //return printf(", ");
    return printf("%s", context->config->value_separator);
}

static int _print_line_separator(run_ls_context* context) {
    return printf("\n");
}

static int _print_line_csv(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_fs_file_t* file = entry->file;

    int pos = 0;
    bool has = false;

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char mode[11 + 1];
        memset(mode, '-', 10);
        mode[10] = ' ';
        mode[11] = '\0';

        lib_fs_file_add_attr(file, mode);
        pos += _print_quote(context);
        pos += printf("%s", mode);
        pos += _print_quote(context);
        has = true;
    }
        
    /* Print NLink      */
    if (config->use_nlink) {
        if (has) {
            pos += _print_value_separator(context);
        }
        int nlink = entry->nlink;
        pos += printf("%d", nlink);
        has = true;
    }

    /* Print User Name  */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char* uname = entry->uname;
        pos += _print_quote(context);
        pos += printf("%s", uname);
        pos += _print_quote(context);
        has = true;
    }

    /* Print Group Name */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char* gname = entry->gname;
        pos += _print_quote(context);
        pos += printf("%s", gname);
        pos += _print_quote(context);
        has = true;
    }
        
    /* Print Size    */
    if (config->use_size & config->use_size_first) {
        if (has) {
            pos += _print_value_separator(context);
        }
        uint64_t size = lib_fs_file_get_file_size(file);
        pos += _print_size_txt(context, size);
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        time_t time = lib_fs_file_get_file_mtime(file);
        pos += _print_quote(context);
        pos += lib_fmt_print_file_date_time(time, context->time_buf, TIME_BUF_LEN, true);
        pos += _print_quote(context);
        has = true;
    }

    /* Print Name   */
    if (has) {
        pos += _print_value_separator(context);
    }
    char* name = entry->name;
    pos += _print_quote(context);
    pos += printf("%s", name);
    pos += _print_quote(context);
    free(name);

    /* Print Line  */
    pos += _print_line_separator(context);

    return pos;
}

//// Print Line TSV ////
static int _print_line_tsv(run_ls_context* context, file_entry_t* entry) {
    return _print_line_csv(context, entry);
}

//// Print Line FIX ////

static int _print_line_fix(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_fs_file_t* file = entry->file;

    int pos = 0;
    bool has = false;

    //char* name = entry->name;
    //int name_len = entry->name_len;

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char mode[11 + 1];
        memset(mode, '-', 10);
        mode[10] = ' ';
        mode[11] = '\0';

        lib_fs_file_add_attr(file, mode);
        pos += printf("%s", mode);
        has = true;
    }
        
    /* Print NLink      */
    if (config->use_nlink) {
        if (has) {
            pos += _print_value_separator(context);
        }
        int nlink = entry->nlink;
        pos += printf("%*d", context->max_nlink_len, nlink);
        has = true;
    }

    /* Print User Name  */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char* uname = entry->uname;
        pos += printf("%-*s ", context->max_uname_len, uname);
        has = true;
    }

    /* Print Group Name */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        char* gname = entry->gname;
        pos += printf("%-*s", context->max_gname_len, gname);
        has = true;
    }
        
    /* Print Size    */
    if (config->use_size & config->use_size_first) {
        if (has) {
            pos += _print_value_separator(context);
        }
        uint64_t size = lib_fs_file_get_file_size(file);
        pos += _print_size_fix(context, size);
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        time_t time = lib_fs_file_get_file_mtime(file);
        pos += lib_fmt_print_file_date_time(time, context->time_buf, TIME_BUF_LEN, true);
        has = true;
    }

    /* Print Size    */
    //if (config->use_size & !config->use_size_first) {
    //    uint64_t size = lib_fs_file_get_file_size(file);
    //    pos += _print_size(context, size);
    //}

    // if (context->stat_pos < 0) {
    //     context->stat_pos = context->pos;
    //     if (context->stat_pos < 0) {
    //         context->stat_pos = 0;
    //     }
    // }

    /* Print Name   */
    if (has) {
        pos += _print_value_separator(context);
    }
    char* name = entry->name;
    pos += printf("%s", name);
    free(name);

    /* Print Line  */
    pos += _print_line_separator(context);

    return pos;
}

////

static int _print_line(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;

    if (config->format == RUN_LS_FORMAT_CSV) {
        return _print_line_csv(context, entry);
    } else if (config->format == RUN_LS_FORMAT_TSV) {
        return _print_line_tsv(context, entry);
    }

    return _print_line_fix(context, entry);;
}

////
int run_ls(run_ls_context* context) {

    run_ls_config* config = context->config;
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
    lib_sys_locale_init_utf8();

    context->pos = 0;
    context->stat_pos = -1;
    context->total = 0;

    context->buf_size = 0; //lib_io_stdout_get_buf_size();
    //context->buf_size = lib_io_get_buf_page(buf_size);

    //printf("buf size: %lu\n", context->buf_size);
    //fflush(stdout);
    //int max = 0;

    //int err = 0;
    //if ((err = fflush(stdout)) != 0) {
    //   fprintf(stderr, "error-1: %d\n", err);
    //   perror("fflush error\n");
    //}

    file_entry_t* entry = NULL;
    file_entry_t* entries[file_count];

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

        const lib_fmt_size_format_t* format = _get_size_format(context, size);
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

        if (entry->name_len > context->max_name_len) {
            context->max_name_len = entry->name_len;
        }
        if (entry->nlink_len > context->max_nlink_len) {
            context->max_nlink_len = entry->nlink_len;
        }
        if (entry->uname_len > context->max_uname_len) {
            context->max_uname_len = entry->uname_len;
        }
        if (entry->gname_len > context->max_gname_len) {
            context->max_gname_len = entry->gname_len;
        }

        if (size_len > context->max_size_len) {
            context->max_size_len = size_len;
        }

    }

    //printf("max_name_len  : %d\n", context->max_name_len);
    //printf("max_nlink_len : %d\n", context->max_nlink_len);
    //printf("max_uname_len : %d\n", context->max_uname_len);
    //printf("max_gname_len : %d\n", context->max_gname_len);
    //printf("max_size_len : %d\n\n", context->max_size_len);

    for (int i = 0; i < file_count; i++) {

        entry = entries[i];
        file = entry->file;

        if (!file) {
            continue;
        }
        
        if (context->buf_size > 0 && i > 0) {
            int try_pos = (context->stat_pos > 0 ? context->stat_pos : 0) + entry->name_len;
            if (context->total + try_pos > context->buf_size) {
                fflush(stdout);
                //context->pos = printf("%s: %d, %d, %d, %d\n", "FLUSH (!)", context->total, try_pos, context->stat_pos, entry->name_len);
                context->total = 0;
            }
        }
        
        // if (config->format == RUN_LS_FORMAT_CSV) {
        //     context->pos = _print_line_csv(context, entry);
        // } else {
        //     context->pos = _print_line_fix(context, entry);            
        // }

        context->pos = _print_line(context, entry);
        context->total += context->pos;
        

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

    lib_sys_locale_restore();

    return 0;
}

int main(int argc, char *argv[]) {

    bool error = false;
    const char* error_msg = NULL;
    const char* error_arg = NULL;

    int opt;
    int long_ind;

    bool is_all   = true;
    bool is_long  = true;
    bool is_human = false;    
    int format    = 0;

    static struct option long_options[] = {
          {"format", required_argument, 0, 'f'},
          {NULL,     0,                 0, 0 }
    };

    while ((opt = getopt_long(argc, argv, "alhf:", long_options, &long_ind)) != -1) {
        switch (opt) {
        case 'a':
            is_all = true;
            break;
        case 'l':
            is_long = true;
            break;
        case 'h':
            is_human = true;
            break;
        case 'f':
            format = _get_format(optarg);
            if (format == 0) {
                error = true;
                error_msg = "Unsupported format";
                //error_arg = optarg;
            }
            break;
        case '?':
            error = true;
            break;
        case ':':
            error = true;
            break;
        }
    }

    if (error) {
        if (error_msg) {
            fprintf(stderr, "%s\n", error_msg);
        }
        usage();
        return 1;
    }

    run_ls_config config;
    run_ls_config_init(&config);

    config.is_all   = is_all;
    config.is_long  = is_long;
    config.is_human = is_human;
    config.format   = format;

    if (config.is_human 
        && (config.format == RUN_LS_FORMAT_DEF || config.format == RUN_LS_FORMAT_TAB)) {
        // Set human size mode: start with 'Kb'
        config.min_size_format = 0;
    }

    if (config.format == RUN_LS_FORMAT_DEF) {
        config.value_separator = " ";
    } else if (config.format == RUN_LS_FORMAT_CSV) {
        config.value_separator = ",";
    } else if (config.format == RUN_LS_FORMAT_TSV) {
        config.value_separator = "\t";
    } else if (config.format == RUN_LS_FORMAT_TAB) {
        config.value_separator = " | ";
    }

    run_ls_context context;
    run_ls_context_init(&context);
    context.config = &config;

    return run_ls(&context);
}
