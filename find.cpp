
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>

#include <cctype>
#include "iolib.h"

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
    int mode;
    bool findFirstOnly;
};

void printUsage() {
    printf("Usage: run-find text file \n");
}

////

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

void calcWidth(Positions* positions, int& indexWidth) {
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

void calcWidth(Positions* positions, int& rowWidth, int& colWidth) {
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

int skipLine(char* data, size_t data_size, size_t pos) {
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

Positions* findBinary(char* data, size_t data_size, char* input, size_t input_size) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    bool findFirstOnly = true;

    Positions* positions = NULL;
    Position* curr = NULL;
    
    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            return positions;
        }

        bool found = true;

        for (size_t j = 0; j < input_size; j++) {
            if (data[pos + j] != input[j]) {
                found = false;
                break;
            }
        }

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

////

Positions* findText(char* data, size_t data_size, char* input, size_t input_size) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    bool findFirstOnly = false;

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
                return positions;
            }
        }

        if (newline) {
            col = 1;
            startRowIndex = pos; // start row position
        } else if (pos > 0) {
            col++;
        }

        bool found = true;

        for (size_t j = 0; j < input_size; j++) {
            if (data[pos + j] != input[j]) {
                found = false;
                break;
            }
        }

        if (found) {
            Position* position = new Position();
            position->index = pos;
            position->row = row;
            position->col = col;
            position->startRowIndex = startRowIndex;
            position->endRowIndex = startRowIndex + input_size;
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

void printTextPosition(Position* position, char* data, int fileSize, int inputSize, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    int startIndex = position->index; 
    //int startIndex = position->startRowIndex; // all line

    printf(format, position->row, position->col);

    int endIndex = startIndex + inputSize;
    //int endIndex = position->endRowIndex; // all line

    int delta = 10;
    int rest = fileSize - endIndex;
    delta = rest < delta ? rest : delta;
    int endIndex2 = endIndex + delta;
    char c;

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
      int row_width = std::to_string(position->row).length();
      int col_width = std::to_string(position->col).length();

      //char* str = (char*) malloc(100);
      //sprintf(str,"%i",position->row);
      //int row_width = strlen(str);

      //printf("%s", str);
      //printf("%i", row_width);

      width = 5 + row_width + col_width;
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

void printTextPositions(Positions* positions, char* data, int fileSize, int inputSize) {
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
        printTextPosition(curr, data, fileSize, inputSize, fmt, width);
        curr = curr->next;
    }
}

void printBynaryPosition(Position* position, char* data, int fileSize, int inputSize, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    int startIndex = position->index; 
    int endIndex = startIndex + inputSize;

    int delta = 10;
    int rest = fileSize - endIndex;
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

void printBynaryPositions(Positions* positions, char* data, int fileSize, int inputSize) {
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
        printBynaryPosition(curr, data, fileSize, inputSize, fmt, width);
        curr = curr->next;
    }
}

void destroy(Positions* positions) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->last;
    while (curr != NULL) {
        curr->prev = NULL;
        curr->next = NULL;
        free(curr);
        curr = curr->prev;
    }
    positions->first = NULL;
    positions->last = NULL;
    free(positions);
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printUsage();
        return 0;
    }

    char* input = argv[1];
    char* fileName = argv[2];
    size_t inputSize = strlen(input);
    size_t fileSize = 0;
 
    if (inputSize == 0) {
        printf("Input is empty\n");
        return 0;
    }

    char* data = readBytes(fileName, fileSize);

    if (fileSize == 0) {
        printf("File '%s' is empty\n", fileName);
        free(data);
        return 0;
    }

    // Config
    bool binaryMode = false;
    bool findFirstOnly = false;

    Positions* positions = NULL;

    if (binaryMode) {
        positions = findBinary(data, fileSize, input, inputSize);
    } else {
        positions = findText(data, fileSize, input, inputSize);
    }
    
    if (positions == NULL) {
        printf("Not found\n");
        free(data);
        return 0;
    }

    if (binaryMode) {
        printBynaryPositions(positions, data, fileSize, inputSize);
    } else {
        printTextPositions(positions, data, fileSize, inputSize);
    }

    destroy(positions);
    free(data);

}
