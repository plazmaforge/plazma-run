#ifndef PLAZMA_LIB_ASKLIB_H
#define PLAZMA_LIB_ASKLIB_H

#include <stdlib.h>

/*
 Define default config parameters
*/
#define LIB_ASK_BIN_MODE         false
#define LIB_ASK_FIND_FIRST_ONLY  false
#define LIB_ASK_IGNORE_CASE      false
#define LIB_ASK_PRINT_FILE_NAME  false

typedef struct lib_ask_config_t {
    bool bin_mode;
    bool find_first_only;
    bool ignore_case;
    bool print_file_name;
} lib_ask_config_t;

typedef struct lib_ask_position_t {
    int index;
    int col;
    int row;
    int start_row_index;
    int end_row_index;
    lib_ask_position_t* prev;
    lib_ask_position_t* next;
} lib_ask_position_t;

typedef struct lib_ask_positions_t {
    lib_ask_position_t* first;
    lib_ask_position_t* last;
    int size;
} lib_ask_positions_t;

typedef struct lib_ask_result_t {
    lib_ask_positions_t* positions;
} lib_ask_result_t;

void lib_ask_config_init(lib_ask_config_t* config);

void lib_ask_find(const char* file_name, const char* input, size_t input_size, lib_ask_config_t* config);

lib_ask_result_t* lib_ask_find_data(const char* data, size_t data_size, const char* input, size_t input_size, lib_ask_config_t* config);

lib_ask_result_t* lib_ask_find_bin(const char* data, size_t data_size, const char* input, size_t input_size, lib_ask_config_t* config);

lib_ask_result_t* lib_ask_find_txt(const char* data, size_t data_size, const char* input, size_t input_size, lib_ask_config_t* config);

void lib_ask_print_data(const lib_ask_result_t* result, const char* data, size_t data_size, size_t input_size, lib_ask_config_t* config);

void lib_ask_print_bin(const lib_ask_result_t* result, const char* data, size_t data_size, size_t input_size);

void lib_ask_print_txt(const lib_ask_result_t* result, const char* data, size_t data_size, size_t input_size);

#endif // PLAZMA_LIB_ASKLIB_H