#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "arglib.h"

int RUN_UNIMAP_FORMAT_SPC   = 0;
int RUN_UNIMAP_FORMAT_CSV   = 1;
int RUN_UNIMAP_FORMAT_TSV   = 2;
int RUN_UNIMAP_FORMAT_TAB   = 3;
int RUN_UNIMAP_FORMAT_MAP   = 4;
int RUN_UNIMAP_FORMAT_ARRAY = 5;

typedef struct run_unimap_config_t {
    int format;
    bool use_comments;
} run_unimap_config_t;

typedef struct run_unimap_entry_t {
    char icode[16];      /* international char code */
    char ucode[16];      /* unicode char code       */
    char name[255];      /* char name               */
    char prev_name[255];
    int number;
    bool is_start;
    run_unimap_config_t* config;
} run_unimap_entry_t;

bool _is_empty(char* str) {
    return (!str || *str == '\0');
}

bool _is_blank(char* str) {
    return (*str == ' ');
}

bool _is_tab(char* str) {
    return (*str == '\t');
}

bool _is_space(char* str) {
    return _is_blank(str) || _is_tab(str);
}

bool _is_comment(char* str) {
    return (*str == '#');
}

bool _is_end(char* str) {
    return (*str == '\r' || *str == '\n');
}

bool _is_valid(char* str) {
    return !_is_empty(str) 
        && !_is_tab(str) 
        && !_is_end(str);
}

bool _is_no_data(char* str) {
    if (!str) {
        return true;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return true;
    }
    char* chr = str;
    int count = 0;
    for (int i = 0; i < len; i++) {
        if (*chr == ' ') {
            count++;
        }
        chr++;
    }
    return count == len;
}

char* _to_case(int mode, char* str) {
    if (!str) {
        return str;
    }
    size_t len = strlen(str);
    if (len == 0) {
        return str;
    }
    char* chr = str;
    for (int i = 0; i < len; i++) {
        *chr = mode ? toupper(*chr) : tolower(*chr);
        chr++;
    }
    return str;
}

/**
 * Returns true if the line is ignore 
 */
bool _is_ignore_line(char* str) {
    if (_is_empty(str)) {
        return true;
    }

    if (_is_space(str) || _is_end(str)) {

            //str++;
            // Whitespace: incorrect line
            return true;
    } else if (_is_comment(str)) {
            // Comment line (#): ignore line
            return true;
    } 

    return false;
}

void _read_line(run_unimap_entry_t* entry, char* line) {
    
    char* str = line;
    char* val = str;

    entry->icode[0]  = '\0';
    entry->ucode[0]  = '\0';
    //entry->name[0]   = '\0';

    int col = 0;
    int len = 0;
    bool blank = false;

    //    Format: Three tab-separated columns
    //        Column #1 is the Input code (in hex)
    //        Column #2 is the Unicode (in hex as 0xXXXX)
    //        Column #3 is the Unicode name (follows a comment sign, '#')

    for(;;) {

        len = 0;
        while (_is_valid(str)) {
            str++;
            len++;
        }

        col++;
        //printf("col = %d, len = %d, val = %s\n", col, len, val);

        if (col == 1) {
            // Column #1: Input code in hex
            if (strlen(val) == 0) {
                blank = true;
            }
            strncpy(entry->icode, val, len);
            entry->icode[len] = '\0';
        } else if (col == 2) {
            // Column #2: Unicode in hex
            strncpy(entry->ucode, val, len);
            entry->ucode[len] = '\0';
        } else if (col == 3 || col == 4) {
            // Column #3: Unicode name

            // Check virtual column
            // For example 8859-2.TXT: #[tab] <name>
            bool is_virt_col = (col == 3 && len == 1 && _is_comment(val));
            
            //if (col == 3 && len == 1 && _is_comment(val)) {
            //    //printf("continue\n");
            //} else {

            if (!is_virt_col) {    
                entry->prev_name[0] = '\0';
                if (len > 0) {

                    // move name to prev_name (!)
                    strcpy(entry->prev_name, entry->name);

                    // skip '#'
                    // shift first char: val + 1
                    bool shift = _is_comment(val);
                    if (shift) {
                        len--;
                    }

                    strncpy(entry->name, shift ? val + 1 : val, len);
                }
                entry->name[len] = '\0';
                if (col == 4) {
                    break;
                }
                //break;
            }
        } else {
            break;
        }

        // Check empty or end of the line
        if (_is_empty(str) || _is_end(str)) {
            break;
        }

        // Move to the next column
        val += len;

        // Check the column separator
        if (_is_tab(str)) {
            str++;
            val++;
        }

    }

    if (col < 3) {
        // move name to prev_name (!)
        strcpy(entry->prev_name, entry->name);
        entry->name[0] = '\0';
    }

    //printf("%s", line);
}

int _print_indent() {
    return printf("  ");
}

int _print_line_separator() {
    return printf("\n");
}
                
int _print_name_comment(char* name) {
    return printf(" /* %s */", name);
}

int _print_hex_comment(int hex) {
    unsigned int u = (unsigned int) hex;
    //return printf(" /* 0x%0*x */", 2, u);
    return printf(" /* 0x%02x */", u);
}

int _print_line_map(run_unimap_entry_t* entry) {
    int pos = 0;
    //if (entry->number > 1) {
    if (entry->is_start) {
        pos += printf(",");
        if (entry->config->use_comments) {
            pos += _print_name_comment(entry->prev_name);
        }
        pos += _print_line_separator();
    } else {
        entry->is_start = true;
    }
    pos += _print_indent();
    pos += printf("{%s, %s}", entry->icode, _is_no_data(entry->ucode) ? "NO_CHR" : entry->ucode);  // MAP
    return pos;
}

int _print_line_array(run_unimap_entry_t* entry) {
    int pos = 0;
    //if (entry->number > 1) {
    if (entry->is_start) {    
        int index = entry->number - 1;
        int column = 8;
        pos += printf(",");
        if (index % column == 0) {
            if (entry->config->use_comments) {
                pos += _print_hex_comment(index - column); // move to prev row (-column)
            }
            pos += _print_line_separator();
            pos += _print_indent();
        } else {
            pos += printf(" ");
        }
    } else {
        entry->is_start = true;
        pos += _print_indent();
    }
    pos += printf("%s", _is_no_data(entry->ucode) ? "NO_CHR" : _to_case(0, entry->ucode));          // ARRAY
    return pos;
}

int _print_line(run_unimap_entry_t* entry) {

    int format = entry->config->format;

    if (format == RUN_UNIMAP_FORMAT_SPC) {
        return printf("[%s]-[%s]-[%s]\n", entry->icode, entry->ucode, entry->name);  // SPC
    } else if (format == RUN_UNIMAP_FORMAT_CSV) {
        return printf("%s,%s,\"%s\"\n", entry->icode, entry->ucode, entry->name);    // CSV
    } else if (format == RUN_UNIMAP_FORMAT_TSV) {
        return printf("%s\t%s\t\"%s\"\n", entry->icode, entry->ucode, entry->name);  // TSV
    } else if (format == RUN_UNIMAP_FORMAT_TAB) {
        return printf("%6s | %6s | %s\n", entry->icode, entry->ucode, entry->name);  // TAB
    } else if (format == RUN_UNIMAP_FORMAT_MAP) {
        return _print_line_map(entry);                                               // MAP
    } else if (format == RUN_UNIMAP_FORMAT_ARRAY) {
        return _print_line_array(entry);                                             // ARRAY
    }
    return 0;
}

static int _get_format(char* val) {
    if (!val) {
        return 0;
    }

    if (strcmp(val, "spc") == 0) {
        return RUN_UNIMAP_FORMAT_SPC;
    } else if (strcmp(val, "csv") == 0) {
        return RUN_UNIMAP_FORMAT_CSV;
    } else if (strcmp(val, "tsv") == 0) {
        return RUN_UNIMAP_FORMAT_TSV;
    } else if (strcmp(val, "tab") == 0) {
        return RUN_UNIMAP_FORMAT_TAB;
    } else if (strcmp(val, "map") == 0) {
        return RUN_UNIMAP_FORMAT_MAP;
    } else if (strcmp(val, "array") == 0) {
        return RUN_UNIMAP_FORMAT_ARRAY;
    }
    return 0;
}

int run_line(run_unimap_entry_t* entry, char* line) {
    if (!line) {
        return 1;
    }

    _read_line(entry, line);
    _print_line(entry);

    return 0;
}

int run_unimap(run_unimap_config_t* config, const char* file_name) {

    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    FILE* file = fopen(file_name, "rb");

    if (!file) {
        fprintf(stderr, "%s: %s: Unable to open file\n", prog_name, file_name);
        return 1;
    }

    // Buffer to store each line of the file.
    char line[256];
    run_unimap_entry_t entry;
    entry.config   = config;
    entry.number   = 0;
    entry.is_start = false;

    if (config->format == RUN_UNIMAP_FORMAT_MAP) {
        printf("static const int unicode_map[][2] = {");
        _print_line_separator();
    } else if (config->format == RUN_UNIMAP_FORMAT_ARRAY) {
        printf("static const int unicode_array[] = {");
        _print_line_separator();
    }

    while (fgets(line, sizeof(line), file)) {

        // Skip empty, comments line
        if (_is_ignore_line(line)) {
            continue;
        }

        entry.number++;
        
        //if (entry.number <= 128) {
        //    continue;
        //}

        run_line(&entry, line);
    }


    if (config->format == RUN_UNIMAP_FORMAT_MAP) {

        if (entry.number > 0 && config->use_comments) {
            printf(" ");
            _print_name_comment(entry.name);
        }

        _print_line_separator();
        printf("};");
        _print_line_separator();
    } else if (config->format == RUN_UNIMAP_FORMAT_ARRAY) {

        if (entry.number > 0 && config->use_comments) {
            
            int index = entry.number - 1;
            int column = 8;
            int mod = index % column;
            printf(" ");
            _print_hex_comment(index - mod); // move to prev row (-mod)
            
        }

        _print_line_separator();
        printf("};");
        _print_line_separator();
    }

    fclose(file);

    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-unimap [-f csv | tsv | tab | map | array] file\n");
}

int main(int argc, char* argv[]) {

    int min_arg = 1; // <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    prog_name = lib_arg_get_prog_name(argv);
    int error = 0;
    int opt;

    // config
    int format = RUN_UNIMAP_FORMAT_TSV;
    bool use_comments = false;

    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            format = _get_format(optarg);
            if (format == 0) {
                error = 1;
                fprintf(stderr, "%s: Unsupported format: %s\n", prog_name, optarg);
            } else {
                if (format == RUN_UNIMAP_FORMAT_MAP 
                    || format == RUN_UNIMAP_FORMAT_ARRAY) {
                    use_comments = true;
                }
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
        return 0;
    }

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        usage();
        return 0;
    }

    run_unimap_config_t config;
    config.format = format;
    config.use_comments = use_comments;

    char* file_name = argv[optind];

    return run_unimap(&config, file_name);

}
