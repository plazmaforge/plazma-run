#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "iodef.h"

void usage() {
    fprintf(stderr, "Usage: run-encmap [-f csv | tsv | array] file\n");
}

int ENCMAP_SPC   = 0;
int ENCMAP_CSV   = 1;
int ENCMAP_TSV   = 2;
int ENCMAP_ARRAY = 3;

typedef struct map_record_t {
    char icode[16];   /* international char code */
    char ucode[16];   /* unicode char code       */
    char name[255];   /* char name               */
    char prev_name[255];
    int number;
    int mode;
    bool use_comments;
} map_record_t;

bool is_ignore_line(char* str) {
    if (*str == '\0') {
        // Empty string
        return true;
    }

    if (*str == ' ' 
           || *str == '\t'
           || *str == '\r'
           || *str == '\n') {

            //str++;
            // Whitespace: incorrect line
            return true;
    } else if (*str == '#') {
            // Comment line (#): ignore line
            return true;
    } 

    return false;
}

int run_line(map_record_t* record, /*int mode, int number,*/ char* line) {
    if (!line) {
        return 1;
    }
    
    //if (is_ignore_line(line)) {
    //    return 0;
    //}

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
        while (str 
            && *str != '\0'
            && *str != '\r'            
            && *str != '\n'
            && *str != '\t') {
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
        } else if (col == 2) {
            strncpy(record->ucode, val, len);
            record->ucode[len] = '\0';
        } else if (col == 3) {
            record->prev_name[0] = '\0';
            if (len > 0) {

                // move name to prev_name (!)
                strcpy(record->prev_name, record->name);

                // skip '#'
                // shift first char: val + 1
                len--; 

                strncpy(record->name, val + 1, len);
                //record->name[len] = '\0';
            }
            record->name[len] = '\0';
            break;
        } else {
            break;
        }

        if (!str) {
            break;
        }

        if (*str == '\0' 
            || *str == '\r' 
            || *str == '\n') {
            break;
        }

        val += len;

        if (*str == '\t') {
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

    if (record->mode == ENCMAP_SPC) {
        printf("[%s]-[%s]-[%s]\n", record->icode, record->ucode, record->name); // SPC
    } else if (record->mode == ENCMAP_CSV) {
        printf("%s,%s,\"%s\"\n", record->icode, record->ucode, record->name);   // CSV
    } else if (record->mode == ENCMAP_TSV) {
        printf("%s\t%s\t\"%s\"\n", record->icode, record->ucode, record->name); // TSV
    } else if (record->mode == ENCMAP_ARRAY) {

        if (record->number > 1) {
            //printf("%s%i\n", ",", number);
            
            if (record->use_comments) {
                printf("%s /* %s */ \n", ",", record->prev_name);
            } else {
                printf("%s\n", ",");
            }
        }

        printf("    {%s, %s}", record->icode, record->ucode);                  // ARRAY
    }

    return 0;
}

int run_encmap(int mode, const char* file_name) {

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
    //int number = 0;
    map_record_t record;
    record.mode = mode;
    record.number = 0;

    if (mode == ENCMAP_ARRAY) {
        printf("%s\n", "static const int ENCODING_MAP[][2] = {");
    }

    while (fgets(line, sizeof(line), file)) {

        // Skip empty, comments line
        if (is_ignore_line(line)) {
            continue;
        }
        //number++;
        record.number++;

        run_line(&record /*mode, number*/, line);
    }


    if (mode == ENCMAP_ARRAY) {

        if (record.number > 0 && record.use_comments) {
            printf("  /* %s */ \n", record.name);
        }

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

    int mode = ENCMAP_TSV;
    bool use_comments = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "f:")) != -1) {
        switch (opt) {
        case 'f':
            if (optarg) {
                if (strcmp(optarg, "spc") == 0) {
                    mode = ENCMAP_SPC;
                } else if (strcmp(optarg, "csv") == 0) {
                    mode = ENCMAP_CSV;
                } else if (strcmp(optarg, "tsv") == 0) {
                    mode = ENCMAP_TSV;
                } else if (strcmp(optarg, "array") == 0) {
                    mode = ENCMAP_ARRAY;
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

    char* file_name = argv[optind];

    lib_io_buf_init();

    return run_encmap(mode, file_name);

}

//#CYRILLIC SMALL LETTER YERU 