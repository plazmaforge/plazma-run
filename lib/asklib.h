#ifndef PLAZMA_LIB_ASKLIB_H
#define PLAZMA_LIB_ASKLIB_H

#include <stdlib.h>

typedef struct ask_position_t {
    int index;
    int col;
    int row;
    int start_row_index;
    int end_row_index;
    ask_position_t* prev;
    ask_position_t* next;
} ask_position_t;

typedef struct ask_positions_t {
    ask_position_t* first;
    ask_position_t* last;
    int size;
} ask_positions_t;

typedef struct ask_config {
    bool binary_mode;
    bool find_first_only;
    bool ignore_case;
    bool print_file_name;
} ask_config_t;

ask_config_t* ask_config_new();

void ask_find(const char* file_name, const char* input, int input_size, const ask_config_t* config);

void ask_print_data(const ask_positions_t* positions, const char* data, int data_size, int input_size, const ask_config_t* config);

ask_positions_t* ask_find_binary(const char* data, size_t data_size, const char* input, size_t input_size, const ask_config_t* config);

ask_positions_t* ask_find_text(const char* data, size_t data_size, const char* input, size_t input_size, const ask_config_t* config);

void ask_print_binary(const ask_positions_t* positions, const char* data, int data_size, int input_size);

void ask_print_text(const ask_positions_t* positions, const char* data, int data_size, int input_size);

#endif // PLAZMA_LIB_ASKLIB_H