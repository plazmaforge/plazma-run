#include <stdio.h>

#include "fslib.h"
#include "iolib.h"
#include "srhlib.h"

int getWidth(int value) {
    // abs!
    if (value < 10) {
        return 1;
    } else if (value < 100) {
        return 2;
    } else if (value < 1000) {
        return 3;
    } else if (value < 10000) {
        return 4;
    }
    return 0;
}

void calcWidth(const Positions* positions, int& indexWidth) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;
    
    int maxIndex = 0;
    while (curr != NULL) {
        if (curr->index > maxIndex) {
            maxIndex = curr->index;
        }
        curr = curr->next;
    }
    indexWidth = getWidth(maxIndex);
}

void calcWidth(const Positions* positions, int& rowWidth, int& colWidth) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;
    
    int maxRow = 0;
    int maxCol = 0;
    while (curr != NULL) {
        if (curr->row > maxRow) {
            maxRow = curr->row;
        }
        if (curr->col > maxCol) {
            maxCol = curr->col;
        }
        curr = curr->next;
    }
    rowWidth = getWidth(maxRow);
    colWidth = getWidth(maxCol);
    if (rowWidth == 0 || colWidth == 0) {
      rowWidth = 0;
      colWidth = 0;
    }
}

int skipLine(const char* data, size_t data_size, size_t pos) {
    char c = data[pos];
    if (c == '\r') {
        if (pos + 1 < data_size && data[pos + 1] == '\n') {
            // \r\n
            return 2;
        } else {
            // \r
            return 1;
        }
    } else if (c == '\n' || c == '\f') {
        return 1;
    }
    return 0;
}

////

bool contains(const char* data, const char* input, int start, int size) {
    for (size_t j = 0; j < size; j++) {
        if (data[start + j] != input[j]) {
            return false;
        }
    }
    return true;
}

bool icontains(const char* data, const char* input, int start, int size) {
    for (size_t j = 0; j < size; j++) {
        if (std::tolower(data[start + j]) != std::tolower(input[j])) {
            return false;
        }
    }
    return true;
}

////

Positions* findBinary(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    // by default
    bool findFirstOnly = false;

    // by config
    if (config != NULL) {
        findFirstOnly = config->findFirstOnly;
    }

    Positions* positions = NULL;
    Position* curr = NULL;
    
    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            return positions;
        }

        bool found = contains(data, input, pos, input_size);

        // bool found = true;
        // for (size_t j = 0; j < input_size; j++) {
        //     if (data[pos + j] != input[j]) {
        //         found = false;
        //         break;
        //     }
        // }

        if (found) {
            Position* position = new Position();
            position->index = pos;
            position->prev = NULL;
            position->next = NULL;

            if (positions == NULL) {
                curr = position;
                positions = new Positions();
                positions->first = curr;
                positions->last = curr;
                positions->size = 1;
            } else {
                Position* prev = curr;
                curr->next = position;
                curr = position;
                curr->prev = prev;
                positions->last = curr;
                positions->size++;
            }

            if (findFirstOnly) {
                return positions;
            }
        }

    }
    return positions;
}

void fixedEndRowIndex(const char* data, size_t data_size, const char* input, size_t input_size, Position* curr) {
    if (curr == NULL) {
        return;
    }
    if (curr->endRowIndex > curr->startRowIndex + input_size) {
        return;
    }
    if (curr->index >= data_size) {
        return;
    }

    //printf("input_size: %lu\n", input_size);
    //printf("startRowIndex: %d\n", curr->startRowIndex);
    //printf("endRowIndex: %d\n", curr->endRowIndex);
    for (size_t pos = curr->index; pos < data_size; pos++) {
        char c = data[pos];
        if (c == '\r' || c == '\n') {
            curr->endRowIndex = pos;
            break;
        }
    }
    //printf("endRowIndex: %d\n", curr->endRowIndex);     
}

Positions* findText(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    // by default
    bool findFirstOnly = false;
    bool ignoreCase = false;

    // by config
    if (config != NULL) {
        findFirstOnly = config->findFirstOnly;
        ignoreCase = config->ignoreCase;
    }

    int row = 1;
    int col = 1;
    int skip = 0;
    bool newline = false;

    Positions* positions = NULL;
    Position* curr = NULL;
    
    int startRowIndex = 0;
    int endRowIndex = 0;

    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            fixedEndRowIndex(data, data_size, input, input_size, curr);
            return positions;
        }

        skip = 0;
        newline = false;
        while ((skip = skipLine(data, data_size, pos)) > 0) {
            
            if (curr != NULL && !newline) {
                curr->endRowIndex = pos;
            }
            newline = true;
            pos += skip;
            row++;
            if (pos >= data_size) {
                fixedEndRowIndex(data, data_size, input, input_size, curr);
                return positions;
            }
        }

        if (newline) {
            col = 1;
            startRowIndex = pos; // start row position
        } else if (pos > 0) {
            col++;
        }

        bool found = ignoreCase ? icontains(data, input, pos, input_size) : contains(data, input, pos, input_size);

        if (found) {
            Position* position = new Position();
            position->index = pos;
            position->row = row;
            position->col = col;
            position->startRowIndex = startRowIndex;
            position->endRowIndex = startRowIndex + input_size;
            position->prev = NULL;
            position->next = NULL;

            if (curr != NULL && curr->row == row) {
                fixedEndRowIndex(data, data_size, input, input_size, curr);
            }

            if (positions == NULL) {
                curr = position;
                positions = new Positions();
                positions->first = curr;
                positions->last = curr;
                positions->size = 1;
            } else {
                Position* prev = curr;
                curr->next = position;
                curr = position;
                curr->prev = prev;
                positions->last = curr;
                positions->size++;
            }

            if (findFirstOnly) {
                fixedEndRowIndex(data, data_size, input, input_size, curr);
                return positions;
            }
        }

    }

    fixedEndRowIndex(data, data_size, input, input_size, curr);
    return positions;
}

void printTextPosition(const Position* position, const char* data, int data_size, int input_size, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    bool printFullLine = true;
    bool printUnderLine = false;

    int startIndex = 0;
    int endIndex = 0;
    int endIndex2 = 0;
    int pad = 0;
    int count = input_size; // TODO: count char - not len

    if (printFullLine) {
        startIndex = position->startRowIndex; // full line
        endIndex = position->endRowIndex;     // full line
        endIndex2 = endIndex;
        pad = position->index - position->startRowIndex; // TODO: count char - not len
    } else {
        startIndex = position->index;
        endIndex = startIndex + input_size;

        int delta = 10;
        int rest = data_size - endIndex;
        delta = rest < delta ? rest : delta;
        endIndex2 = endIndex + delta;
    }

    char c;

    printf(format, position->row, position->col);
    for (size_t i = startIndex; i < endIndex2; i++)  {
        c = data[i];
        if (c == '\r' || c == '\n') {
            break;
        }
        //if (!std::isprint(c)) {
            //c = '?';
            //break;
        //}
        printf("%c", c);
    }

    // Start new underline string
    printf("\n");

    // Whitespace before underline
    if (width <= 0) {
      int row_width = std::to_string(position->row).length();
      int col_width = std::to_string(position->col).length();

      //char* str = (char*) malloc(100);
      //sprintf(str,"%i",position->row);
      //int row_width = strlen(str);

      //printf("%s", str);
      //printf("%i", row_width);

      width = 5 + row_width + col_width;
    }

    if (!printUnderLine) {
        return;
    }

    for (int i = 0; i < width; i++)  {
        printf(" ");
    }

    for (int i = 0; i < pad; i++)  {
        printf(" ");
    }

    // Underline word
    for (int i = 0; i < count; i++)  {
        printf("_");
    }

    printf("\n");

}

void printText(const Positions* positions, const char* data, int data_size, int input_size) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;

    bool fixedFormat = true;
    int width = 0;
    std::string format;

    if (fixedFormat) {
        format = "[%d: %d] ";
    } else {
        int rowWidth = 0; 
        int colWidth = 0;
        calcWidth(positions, rowWidth, colWidth);
        width = rowWidth + colWidth + 5;
        format = "[%" + std::to_string(rowWidth) + "d: %" + std::to_string(colWidth) + "d] ";

    }
    const char* fmt = format.c_str();
    while (curr != NULL) {
        printTextPosition(curr, data, data_size, input_size, fmt, width);
        curr = curr->next;
    }
}

void printBynaryPosition(const Position* position, const char* data, int data_size, int input_size, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    int startIndex = position->index; 
    int endIndex = startIndex + input_size;

    int delta = 10;
    int rest = data_size - endIndex;
    delta = rest < delta ? rest : delta;
    int endIndex2 = endIndex + delta;
    char c;

    printf(format, position->index);

    for (size_t i = startIndex; i < endIndex2; i++)  {
        c = data[i];
        if (!std::isprint(c)) {
            //c = ' ';
            break;
        }
        printf("%c", c);
    }

    // Start new underline string
    printf("\n");

    // Whitespace before underline
    if (width <= 0) {
      width = std::to_string(position->index).length();
      width = 3 + width;
    }

    bool underline = false;

    if (!underline) {
        return;
    }

    for (int i = 0; i < width; i++)  {
        printf(" ");
    }

    // Underline word
    for (size_t i = startIndex; i < endIndex; i++)  {
        printf("_");
    }

    printf("\n");

}

void printBynary(const Positions* positions, const char* data, int data_size, int input_size) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;

    bool fixedFormat = true;
    int width = 0;
    std::string format;

    if (fixedFormat) {
        format = "[%d] ";
    } else {
        int indexWidth = 0; 
        calcWidth(positions, indexWidth);
        width = indexWidth + 3;
        format = "[%" + std::to_string(indexWidth) + "d] ";

    }
    const char* fmt = format.c_str();
    while (curr != NULL) {
        printBynaryPosition(curr, data, data_size, input_size, fmt, width);
        curr = curr->next;
    }
}

void destroy(Positions* positions) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;
    Position* next = NULL;
    while (curr != NULL) {
        next = curr->next;
        curr->prev = NULL;
        curr->next = NULL;
        free(curr);
        curr = next;
    }
    positions->first = NULL;
    positions->last = NULL;
    free(positions);
}

////

Positions* findData(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (config->binaryMode) {
        return findBinary(data, data_size, input, input_size, config);
    } else {
        return findText(data, data_size, input, input_size, config);
    }
}

void printData(const Positions* positions, const char* data, int data_size, int input_size, const FindConfig* config) {
    if (config->binaryMode) {
        printBynary(positions, data, data_size, input_size);
    } else {
        printText(positions, data, data_size, input_size);
    }
}

////

void find(const char* fileName, const char* input, int inputSize, const FindConfig* config) {

    size_t fileSize = 0;
    char* data = readBytes(fileName, fileSize);

    if (data == NULL) {
        //printf("File '%s' not found\n", fileName);
        return;
    }

    if (fileSize == 0) {
        //printf("File '%s' is empty\n", fileName);
        free(data);
        return;
    }

    Positions* positions = findData(data, fileSize, input, inputSize, config);
    
    if (positions == NULL) {
        //printf("Not found\n");
        free(data);
        return;
    }

    if (config->printFileName) {
        char* real_path = fs_get_real_path(fileName);
        printf(">> %s\n", real_path);
        free(real_path);
    }

    printData(positions, data, fileSize, inputSize, config);

    // if (config->binaryMode) {
    //     printBynary(positions, data, fileSize, inputSize);
    // } else {
    //     printText(positions, data, fileSize, inputSize);
    // }

    destroy(positions);
    free(data);

}
