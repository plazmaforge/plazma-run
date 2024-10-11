#ifndef PLAZMA_LIB_ASKLIB_H
#define PLAZMA_LIB_ASKLIB_H

#include <stdlib.h>

/*
 Define default config parameters
*/
#define LIB_ASK_BINARY_MODE false;
#define LIB_ASK_FIND_FIRST_ONLY false;
#define LIB_ASK_IGNORE_CASE false;
#define LIB_ASK_PRINT_FILE_NAME false;

typedef struct lib_ask_config {
    bool binary_mode;
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

lib_ask_config_t* lib_ask_config_new();

void lib_ask_find(const char* file_name, const char* input, int input_size, const lib_ask_config_t* config);

void lib_ask_print_data(const lib_ask_positions_t* positions, const char* data, int data_size, int input_size, const lib_ask_config_t* config);

lib_ask_positions_t* lib_ask_find_binary(const char* data, size_t data_size, const char* input, size_t input_size, const lib_ask_config_t* config);

lib_ask_positions_t* lib_ask_find_text(const char* data, size_t data_size, const char* input, size_t input_size, const lib_ask_config_t* config);

void lib_ask_print_binary(const lib_ask_positions_t* positions, const char* data, int data_size, int input_size);

void lib_ask_print_text(const lib_ask_positions_t* positions, const char* data, int data_size, int input_size);

#endif // PLAZMA_LIB_ASKLIB_H