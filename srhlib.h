#ifndef PLAZMA_LIB_SRHLIB_H
#define PLAZMA_LIB_SRHLIB_H

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

void find(const char* fileName, const char* input, int inputSize, const FindConfig* config);

void printData(const Positions* positions, const char* data, int data_size, int input_size, const FindConfig* config);

Positions* findBinary(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config);

Positions* findText(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config);

void printBynary(const Positions* positions, const char* data, int data_size, int input_size);

void printText(const Positions* positions, const char* data, int data_size, int input_size);

#endif // PLAZMA_LIB_SRHLIB_H