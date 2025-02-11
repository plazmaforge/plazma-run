// Windows: Important lib order for UTF-8 (65001)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>

#include "getopt.h"
#include "clilib.h"
#include "fslib.h"
#include "iodef.h"
#include "fmtlib.h"
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
    bool is_verbose;
    int format;
    int sort;
    //
    bool use_mode;
    bool use_nlink;
    bool use_uname;
    bool use_gname;
    //bool use_date;
    bool use_time;
    bool use_size;

    /*
     -1: Not pretty format - bytes only
      0: Pretty format     - start with 'Kb'
      1: Flex pretty       - start with 'Mb' 
    */
    int min_size_format;
    const char* value_separator;

} run_ls_config;

/* YYYY-MM-DD */
#define DATE_PART_LEN 10

/* HH:MM:SS */
#define TIME_PART_LEN 8

/* YYYY-MM-DD HH:MM:SS */
/* 10 + 1 + 8 + 1      */ 
#define TIME_BUF_LEN 20

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
} run_ls_context;

typedef struct file_entry_t {
    lib_file_t* file;
    char* path;
    char* name;
    char mode[LIB_FS_MODE_LEN];
    int nlink;
    uint64_t size;
    time_t time;
    //
    char* uname;
    char* gname;
    //
    int nlink_len;
    int name_len;
    int uname_len;
    int gname_len;
} file_entry_t;

int run_ls_config_set_columns(run_ls_config* config, bool flag) {
        if (!config) {
        return 1;
    }
    config->use_mode       = flag;
    config->use_nlink      = flag;
    config->use_uname      = flag;
    config->use_gname      = flag;
    //config->use_date       = flag;
    config->use_time       = flag;
    config->use_size       = flag;

    return 0;
}

int run_ls_config_init(run_ls_config* config) {
    if (!config) {
        return 1;
    }
    config->is_all         = true;
    config->is_long        = true;
    config->is_human       = false;
    config->is_verbose     = false;
    config->format         = RUN_LS_FORMAT_DEF;
    config->sort           = 0;
    //
    run_ls_config_set_columns(config, true);
    config->min_size_format = 1;
    //
    config->value_separator = " ";

    return 0;
}

int run_ls_config_format_init(run_ls_config* config) {
    if (!config) {
        return 1;
    }

    if (config->is_human 
        && (config->format == RUN_LS_FORMAT_DEF || config->format == RUN_LS_FORMAT_TAB)) {
        // Set human size mode: start with 'Kb'
        config->min_size_format = 0;
    }

    if (config->format == RUN_LS_FORMAT_DEF) {
        config->value_separator = " ";
    } else if (config->format == RUN_LS_FORMAT_CSV) {
        config->value_separator = ",";
    } else if (config->format == RUN_LS_FORMAT_TSV) {
        config->value_separator = "\t";
    } else if (config->format == RUN_LS_FORMAT_JSON) {
        config->value_separator = ",\n";
    } else if (config->format == RUN_LS_FORMAT_XML) {
        config->value_separator = "\n";
    } else if (config->format == RUN_LS_FORMAT_TAB) {
        config->value_separator = " | ";
    }

    return 0;
}

int run_ls_context_init(run_ls_context* context) {
    if (!context) {
        return 1;
    }
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

    return 0;
}

static int _lib_digit_count(int val) {
    if (val == 0) {
        return 1;
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

static int _lib_digit_count_u64(uint64_t val) {
    if (val == 0) {
        return 1;
    }
    uint64_t num = val;
    int len = 0;
    while(num != 0) {
        num = num / 10;
        len++;
    }
    return len;
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
    } else if (strcmp(val, "tsv") == 0) {
        return RUN_LS_FORMAT_TSV;
    } else if (strcmp(val, "xml") == 0) {
        return RUN_LS_FORMAT_XML;
    } else if (strcmp(val, "json") == 0) {
        return RUN_LS_FORMAT_JSON;
    } else if (strcmp(val, "tab") == 0) {
        return RUN_LS_FORMAT_TAB;
    }
    return 0;
}

static const char* _get_format_name(int val) {
    if (val == RUN_LS_FORMAT_CSV) {
        return "csv";
    } else if (val == RUN_LS_FORMAT_TSV) {
        return "tsv";
    } else if (val == RUN_LS_FORMAT_XML) {
        return "xml";
    } else if (val == RUN_LS_FORMAT_JSON) {
        return "json";
    } else if (val == RUN_LS_FORMAT_TAB) {
        return "tab";
    }
    return "";
}

static int _get_sort(char* val) {
    if (!val) {
        return 0;
    }

    if (strcmp(val, "name") == 0) {
        return LIB_FILE_SORT_BY_NAME;
    } else if (strcmp(val, "size") == 0) {
        return LIB_FILE_SORT_BY_SIZE;
    } else if (strcmp(val, "time") == 0) {
        return LIB_FILE_SORT_BY_TIME;
    }
    return 0;
}

static void _get_size_format(run_ls_context* context, uint64_t size, lib_fmt_size_format_t* format) {
    int min_size_format = context->config->min_size_format;
    if (min_size_format < 0) {
        // No format. It is not pretty format. Size in bytes only
        format->factor = 0;
        format->unit   = NULL;
        return;
    }
    int index = lib_fmt_get_size_format_index(size, min_size_format);

    if (index < 0) {
        format->factor = 0;
        format->unit   = NULL;
        return;
    }

    lib_fmt_size_format_t _format = lib_fmt_get_size_format(index);
    format->factor = _format.factor;
    format->unit   = _format.unit;
}

static double _get_unit_size(uint64_t size, const lib_fmt_size_format_t* format) {
    return (double) size / (double) format->factor;
}

static uint64_t _get_unit_isize(uint64_t size, const lib_fmt_size_format_t* format) {
    return (uint64_t) _get_unit_size(size, format);
}

static const char* _bool_to_str(bool val) {
    return val ? "true" : "false";
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
    lib_fmt_size_format_t format;
    _get_size_format(context, size, &format);

    if (format.unit) {
        double value = _get_unit_size(size, &format);
        const char* unit = format.unit;
        return printf("%*.1f%c", len + 2, value, unit[0]); // (+2) for '.1'
    } else {
        return printf("%*llu ", len + 2, size);            // (+2) for '.1'
        //return printf("%*lluB ", len + 2, size);
    }
}

static char* _trim_mode(char* mode) {
    if (!mode) {
        return NULL;
    }
    if (mode[LIB_FS_MODE_LEN - 1] == ' ') {
        mode[LIB_FS_MODE_LEN - 1] = '\0';
    }
    return mode;
}

//// Print Line CSV ////

static int _print_quote(run_ls_context* context) {
    return printf("\"");
}

static int _print_value_separator(run_ls_context* context) {
    return printf("%s", context->config->value_separator);
}

static int _print_line_separator(run_ls_context* context) {
    return printf("\n");
}

static int _print_line_csv(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_file_t* file = entry->file;

    int pos = 0;
    bool has = false;

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }

        pos += _print_quote(context);
        pos += printf("%s", _trim_mode(entry->mode));
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
    if (config->use_size) {
        if (has) {
            pos += _print_value_separator(context);
        }
        uint64_t size = entry->size;
        pos += _print_size_txt(context, size);
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        time_t time = entry->time;
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

//// Print Line JSON ////

static int _print_line_json(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_file_t* file = entry->file;

    int pos = 0;
    bool has = false;

    pos += printf("%s", "  {\n");

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    \"mode\": ");

        pos += _print_quote(context);
        pos += printf("%s", _trim_mode(entry->mode));
        pos += _print_quote(context);
        has = true;
    }
        
    /* Print NLink      */
    if (config->use_nlink) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    \"nlink\": ");

        int nlink = entry->nlink;
        pos += printf("%d", nlink);
        has = true;
    }

    /* Print User Name  */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    \"uname\": ");

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
        pos += printf("%s", "    \"gname\": ");

        char* gname = entry->gname;
        pos += _print_quote(context);
        pos += printf("%s", gname);
        pos += _print_quote(context);
        has = true;
    }
        
    /* Print Size    */
    if (config->use_size) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    \"size\": ");

        uint64_t size = entry->size;
        pos += _print_size_txt(context, size);
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    \"time\": ");

        time_t time = entry->time;
        pos += _print_quote(context);
        pos += lib_fmt_print_file_date_time(time, context->time_buf, TIME_BUF_LEN, true);
        pos += _print_quote(context);
        has = true;
    }

    /* Print Name   */
    if (has) {
        pos += _print_value_separator(context);
    }
    pos += printf("%s", "    \"name\": ");

    char* name = entry->name;
    pos += _print_quote(context);
    pos += printf("%s", name);
    pos += _print_quote(context);
    free(name);

    /* Print Line  */
    //pos += _print_line_separator(context);

    pos += printf("%s", "\n  }");

    return pos;
}

//// Print Line XML ////

static int _print_line_xml(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_file_t* file = entry->file;

    int pos = 0;
    bool has = false;

    pos += printf("%s", "  <file>\n");

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <mode>");
        
        //pos += _print_quote(context);
        pos += printf("%s", _trim_mode(entry->mode));
        //pos += _print_quote(context);
        pos += printf("%s", "</mode>");
        has = true;
    }
        
    /* Print NLink      */
    if (config->use_nlink) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <nlink>");
        int nlink = entry->nlink;
        pos += printf("%d", nlink);
        pos += printf("%s", "</nlink>");
        has = true;
    }

    /* Print User Name  */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <uname>");
        char* uname = entry->uname;
        //pos += _print_quote(context);
        pos += printf("%s", uname);
        //pos += _print_quote(context);
        pos += printf("%s", "</uname>");
        has = true;
    }

    /* Print Group Name */
    if (config->use_uname) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <gname>");
        char* gname = entry->gname;
        //pos += _print_quote(context);
        pos += printf("%s", gname);
        //pos += _print_quote(context);
        pos += printf("%s", "</gname>");
        has = true;
    }
        
    /* Print Size    */
    if (config->use_size) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <size>");
        uint64_t size = entry->size;
        pos += _print_size_txt(context, size);
        pos += printf("%s", "</size>");
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        pos += printf("%s", "    <time>");

        time_t time = entry->time;
        //pos += _print_quote(context);
        pos += lib_fmt_print_file_date_time(time, context->time_buf, TIME_BUF_LEN, true);
        //pos += _print_quote(context);
        pos += printf("%s", "</time>");
        has = true;
    }

    /* Print Name   */
    if (has) {
        pos += _print_value_separator(context);
    }
    pos += printf("%s", "    <name>");

    char* name = entry->name;
    //pos += _print_quote(context);
    pos += printf("%s", name);
    //pos += _print_quote(context);
    pos += printf("%s", "</name>");

    free(name);

    /* Print Line  */
    //pos += _print_line_separator(context);

    pos += printf("%s", "\n  </file>");

    return pos;
}

//// Print Line FIX ////

static int _print_line_fix(run_ls_context* context, file_entry_t* entry) {

    run_ls_config* config = context->config;
    lib_file_t* file = entry->file;

    int pos  = 0;
    bool has = false;

    /* Print Mode      */
    if (config->use_mode) {
        if (has) {
            pos += _print_value_separator(context);
        }
        
        pos += printf("%s", entry->mode);
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
    if (config->use_size) {
        if (has) {
            pos += _print_value_separator(context);
        }
        uint64_t size = entry->size;
        pos += _print_size_fix(context, size);
        has = true;
    }

    /* Print DateTime */
    if (config->use_time) {
        if (has) {
            pos += _print_value_separator(context);
        }
        time_t time = entry->time;
        pos += lib_fmt_print_file_date_time(time, context->time_buf, TIME_BUF_LEN, true);
        has = true;
    }

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
    }
    if (config->format == RUN_LS_FORMAT_TSV) {
        return _print_line_tsv(context, entry);
    }
    if (config->format == RUN_LS_FORMAT_JSON) {
        return _print_line_json(context, entry);
    }
    if (config->format == RUN_LS_FORMAT_XML) {
        return _print_line_xml(context, entry);
    }

    return _print_line_fix(context, entry);;
}

////

int run_ls(run_ls_context* context) {

    run_ls_config* config = context->config;
    char* dir_name = lib_fs_get_current_dir(); //fs_get_current_find_path();

    if (config->is_verbose) {

        printf("flag_all     : %s\n", _bool_to_str(config->is_all));
        printf("flag_long    : %s\n", _bool_to_str(config->is_long));
        printf("flag_human   : %s\n", _bool_to_str(config->is_human));
        printf("flag_verbose : %s\n", _bool_to_str(config->is_verbose));
        printf("format       : %s\n", _get_format_name(config->format));
        printf("dir          : %s\n", dir_name);
        printf("\n");
    }
    
    char* pattern = NULL;
    lib_file_t** files = NULL;
    lib_file_t* file = NULL;

    lib_fs_scan_config_t cfg;
    lib_fs_scan_config_init(&cfg);
    cfg.max_depth = LIB_SCANDIR_FLAT;
    cfg.use_all = config->is_all;
    if (config->sort > 0) {
        cfg.sort = config->sort;
    }
    
    //int file_count = lib_fs_scandir(dir_name, pattern, &files, LIB_SCANDIR_FLAT, false);
    int file_count = lib_fs_scandir_cfg(&cfg, dir_name, pattern, &files);

    if (file_count <= 0) {
        lib_files_free(files);
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

        lib_fs_init_mode(entry->mode);
        lib_file_add_attr(file, entry->mode);

        entry->nlink = lib_file_get_nlink(file);
        entry->uname = lib_file_get_uname(file);
        entry->gname = lib_file_get_gname(file);
        entry->size = lib_file_get_size(file);
        entry->time = lib_file_get_mtime(file);

        entry->name_len = entry->name ? strlen(entry->name) : 0;
        entry->nlink_len = _lib_digit_count(entry->nlink);
        entry->uname_len = entry->uname ? strlen(entry->uname) : 0;
        entry->gname_len = entry->gname ? strlen(entry->gname) : 0;
        int size_len = _lib_digit_count_u64(entry->size);

        lib_fmt_size_format_t format;
        _get_size_format(context, entry->size, &format);

        if (format.unit) {
            uint64_t unit_size = _get_unit_isize(entry->size, &format);
            int unit_size_len = _lib_digit_count_u64(unit_size);
            size_len = unit_size_len;
        }

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

    if (config->format == RUN_LS_FORMAT_JSON) {
        printf("%s", "[\n");
    } else if (config->format == RUN_LS_FORMAT_XML) {
        printf("%s", "<files>\n");
    }

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

        if (i > 0 ) {
            if (config->format == RUN_LS_FORMAT_JSON) {
                printf("%s", ", \n");
            } else if (config->format == RUN_LS_FORMAT_XML) {
                printf("%s", "\n");
            }            
        }
        
        context->pos = _print_line(context, entry);
        context->total += context->pos;
        
        free(entry);
    }

    if (config->format == RUN_LS_FORMAT_JSON) {
        printf("%s", "\n]\n");
    } else if (config->format == RUN_LS_FORMAT_XML) {
        printf("%s", "\n</files>\n");
    }

    lib_files_free(files);
    free(dir_name);

    lib_sys_locale_restore();

    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-ls [-alh] [-format name]\n");
}

int main(int argc, char *argv[]) {

    prog_name = lib_cli_prog_name(argv);
    int error = 0;
    int opt;
    int long_ind;

    bool is_all     = false;
    bool is_long    = true;
    bool is_human   = false;
    bool is_verbose = false;
    int format      = 0;
    int sort        = 0;

    static lib_option long_options[] = {
          {"format", required_argument, 0, 1001}, // 'f'
          {"sort", required_argument, 0, 1002},
          {NULL,     0,                 0, 0 }
    };

    while ((opt = lib_getopt_long(argc, argv, "alhv", long_options, &long_ind)) != -1) {
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
        case 'v':
            is_verbose = true;
            break;
        case 1001: // 'f'
            format = _get_format(optarg);
            if (format == 0) {
                error = 1;
                fprintf(stderr, "%s: Unsupported format: %s\n", prog_name, optarg);
            }
            break;
        case 1002: // 's'
            sort = _get_sort(optarg);
            if (sort == 0) {
                error = 1;
                fprintf(stderr, "%s: Unsupported sorting: %s\n", prog_name, optarg);
            }
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    run_ls_config config;
    run_ls_config_init(&config);

    config.is_all     = is_all;
    config.is_long    = is_long;
    config.is_human   = is_human;
    config.is_verbose = is_verbose;
    config.format     = format;
    config.sort       = sort;

    run_ls_config_format_init(&config);

    run_ls_context context;
    run_ls_context_init(&context);
    context.config = &config;

    return run_ls(&context);
}
