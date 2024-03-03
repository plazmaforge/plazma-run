#ifndef PLAZMA_LIB_ASKLIB_H
#define PLAZMA_LIB_ASKLIB_H

#include <stdlib.h>

struct Position {
    int index;
    int col;
    int row;
    int startRowIndex;
    int endRowIndex;
    Position* prev;
    Position* next;
};

struct Positions {
    Position* first;
    Position* last;
    int size;
};

struct FindConfig {
    bool binaryMode;
    bool findFirstOnly;
    bool ignoreCase;
    bool printFileName;
};

void ask_find(const char* file_name, const char* input, int input_size, const FindConfig* config);

void ask_print_data(const Positions* positions, const char* data, int data_size, int input_size, const FindConfig* config);

Positions* ask_find_binary(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config);

Positions* ask_find_text(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config);

void ask_print_binary(const Positions* positions, const char* data, int data_size, int input_size);

void ask_print_text(const Positions* positions, const char* data, int data_size, int input_size);

#endif // PLAZMA_LIB_ASKLIB_H