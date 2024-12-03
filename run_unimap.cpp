#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "getopt.h"
#include "iodef.h"

int RUN_ENCMAP_FORMAT_SPC    = 0;
int RUN_ENCMAP_FORMAT_CSV    = 1;
int RUN_ENCMAP_FORMAT_TSV    = 2;
int RUN_ENCMAP_FORMAT_TAB    = 3;
int RUN_ENCMAP_FORMAT_MAP  = 4;
int RUN_ENCMAP_FORMAT_ARRAY2 = 5;

typedef struct run_unimap_config_t {
    int mode;
    bool use_comments;
} run_unimap_config_t;

typedef struct run_unimap_record_t {
    char icode[16];      /* international char code */
    char ucode[16];      /* unicode char code       */
    char name[255];      /* char name               */
    char prev_name[255];
    int number;
    run_unimap_config_t* config;
    //int mode;
    //bool use_comments;
} run_unimap_record_t;

void usage() {
    fprintf(stderr, "Usage: run-unimap [-f csv | tsv | tab | array] file\n");
}

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
        // if (str[i] == ' ') {
        //     count++;
        // }
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

void _read_line(run_unimap_record_t* record, char* line) {
    
    char* str = line;
    char* val = str;

    record->icode[0]  = '\0';
    record->ucode[0]  = '\0';
    //record->name[0]   = '\0';

    int col = 0;
    int len = 0;
    bool blank = false;

    for(;;) {

        len = 0;
        while (_is_valid(str)) {
            str++;
            len++;
        }

        col++;
        if (col == 1) {
            if (strlen(val) == 0) {
                blank = true;
            }
            strncpy(record->icode, val, len);
            record->icode[len] = '\0';
            //printf("col = %d, len = %d, val = %s\n", col, len, val);
        } else if (col == 2) {
            strncpy(record->ucode, val, len);
            record->ucode[len] = '\0';
            //printf("col = %d, len = %d, val = %s\n", col, len, val);
        } else if (col == 3 || col == 4) {
            //printf("col = %d, len = %d, val = %s\n", col, len, val);
            if (col == 3 && len == 1 && _is_comment(val)) {
                //printf("continue\n");
            } else {
                record->prev_name[0] = '\0';
                if (len > 0) {

                    // move name to prev_name (!)
                    strcpy(record->prev_name, record->name);

                    // skip '#'
                    // shift first char: val + 1
                    bool shift = false;
                    if (_is_comment(val)) {
                        shift = true;
                        len--;
                    }

                    strncpy(record->name, shift ? val + 1 : val, len);
                    //record->name[len] = '\0';
                }
                record->name[len] = '\0';
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
        strcpy(record->prev_name, record->name);
        record->name[0] = '\0';
    }

    //printf("%s", line);
}

void _print_line(run_unimap_record_t* record) {
    if (record->config->mode == RUN_ENCMAP_FORMAT_SPC) {
        printf("[%s]-[%s]-[%s]\n", record->icode, record->ucode, record->name); // SPC
    } else if (record->config->mode == RUN_ENCMAP_FORMAT_CSV) {
        printf("%s,%s,\"%s\"\n", record->icode, record->ucode, record->name);   // CSV
    } else if (record->config->mode == RUN_ENCMAP_FORMAT_TSV) {
        printf("%s\t%s\t\"%s\"\n", record->icode, record->ucode, record->name); // TSV
    } else if (record->config->mode == RUN_ENCMAP_FORMAT_TAB) {
        printf("%6s | %6s | %s\n", record->icode, record->ucode, record->name); // TAB
    } else if (record->config->mode == RUN_ENCMAP_FORMAT_MAP) {

        if (record->number > 1) {
            //printf("%s%i\n", ",", record->number);
            
            if (record->config->use_comments) {
                printf("%s /* %s */ \n", ",", record->prev_name);
            } else {
                printf("%s\n", ",");
            }
        }

        printf("    {%s, %s}", record->icode, _is_no_data(record->ucode) ? "NO_CHR" : record->ucode);                  // MAP
    } else if (record->config->mode == RUN_ENCMAP_FORMAT_ARRAY2) {
        int index = record->number - 1;

        //if (record->number == 1) {
        //   printf("  ");
        //}

        if (record->number > 1) {
            //printf("%s%i\n", ",", record->number);
            
            printf(",");
            if (index % 8 == 0) {
                printf("\n");
                printf("  ");
            } else {
                printf(" ");
            }
        } else {
            printf("  ");
        }

        printf("%s", _is_no_data(record->ucode) ? "NO_CHR" : _to_case(0, record->ucode));                  // ARRAY
    }
}

int run_line(run_unimap_record_t* record, char* line) {
    if (!line) {
        return 1;
    }

    _read_line(record, line);
    _print_line(record);

    return 0;
}

int run_unimap(run_unimap_config_t* config, const char* file_name) {

    if (!file_name) {
        fprintf(stderr, "Unable to open file!\n");
        return 1;
    }

    FILE* file = fopen(file_name, "rb");

    if (!file) {
        fprintf(stderr, "Unable to open file %s!\n", file_name);
        return 1;
    }

    // Buffer to store each line of the file.
    char line[256];
    run_unimap_record_t record;
    record.config = config;
    record.number = 0;

    if (config->mode == RUN_ENCMAP_FORMAT_MAP) {
        printf("%s\n", "static const int unicode_map[][2] = {");
    } else if (config->mode == RUN_ENCMAP_FORMAT_ARRAY2) {
        printf("%s\n", "static const int unicode_array[] = {");
    }

    while (fgets(line, sizeof(line), file)) {

        // Skip empty, comments line
        if (_is_ignore_line(line)) {
            continue;
        }
        record.number++;

        run_line(&record, line);
    }


    if (config->mode == RUN_ENCMAP_FORMAT_MAP) {

        if (record.number > 0 && config->use_comments) {
            printf("  /* %s */ \n", record.name);
        }

        printf("\n%s\n", "};");
    } else if (config->mode == RUN_ENCMAP_FORMAT_ARRAY2) {

        //if (record.number > 0 && config->use_comments) {
        //    printf("  /* %s */ \n", record.name);
        //}

        printf("\n%s\n", "};");
    }


    fclose(file);

    return 0;
}

int main(int argc, char* argv[]) {

    int min_arg = 1; // <file>
    if (argc < min_arg + 1) {
        usage();
        return 0;
    }

    int mode = RUN_ENCMAP_FORMAT_TSV;
    bool use_comments = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            if (optarg) {
                if (strcmp(optarg, "spc") == 0) {
                    mode = RUN_ENCMAP_FORMAT_SPC;
                } else if (strcmp(optarg, "csv") == 0) {
                    mode = RUN_ENCMAP_FORMAT_CSV;
                } else if (strcmp(optarg, "tsv") == 0) {
                    mode = RUN_ENCMAP_FORMAT_TSV;
                } else if (strcmp(optarg, "tab") == 0) {
                    mode = RUN_ENCMAP_FORMAT_TAB;
                } else if (strcmp(optarg, "map") == 0) {
                    mode = RUN_ENCMAP_FORMAT_MAP;
                    use_comments = true;
                } else if (strcmp(optarg, "array") == 0) {
                    mode = RUN_ENCMAP_FORMAT_ARRAY2;
                    use_comments = true;
                } else {
                    error = true;
                }
            } else {
                error = true;
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
        usage();
        return 0;
    }

    if (argc - optind < min_arg) {
        fprintf(stderr, "%s: Incorrect argument count\n", argv[0]);
        usage();
        return 0;
    }

    run_unimap_config_t config;
    config.mode = mode;
    config.use_comments = use_comments;

    char* file_name = argv[optind];

    lib_io_buf_init();

    return run_unimap(&config, file_name);

}

//#CYRILLIC SMALL LETTER YERU 