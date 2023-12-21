
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>

#include <cctype>

#include <fnmatch.h>
#include <dirent.h>

#include "iolib.h"
#include "getopt.h"

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

Positions* findBinary(char* data, size_t data_size, char* input, size_t input_size, FindConfig* config) {
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

Positions* findText(char* data, size_t data_size, char* input, size_t input_size, FindConfig* config) {
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

        bool found = ignoreCase ? icontains(data, input, pos, input_size) : contains(data, input, pos, input_size);

        // for (size_t j = 0; j < input_size; j++) {
        //     if (data[pos + j] != input[j]) {
        //         found = false;
        //         break;
        //     }
        // }

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

int match_file(const char* pattern, const char* name, int mode) {
    // PathMatchSpecA
    //printf(" %s %s\n", pattern, name);
    return fnmatch(pattern, name, FNM_PERIOD) == 0; // true id '0'
}

int match_file(const char* pattern, const char* name) {
    return match_file(pattern, name, FNM_PERIOD);
}

int check_wildcard(const char* pattern) {
    if (pattern == NULL) {
        return 0;
    }
    int len = strlen(pattern);
    char c;
    for (int i = 0; i < len; i++) {
        c = pattern[i];
        if (c == '*' || c == '?' || c == '[' || c == ']') {
            return 1;
        }
    }
    return 0;
}

char* filter_pattern;

int filter(const struct dirent *entry) {
	return match_file(filter_pattern, entry->d_name);
}

int main(int argc, char* argv[]) {

    int min_arg = 2; // <text> <file>
    if (argc < min_arg + 1) {
        printf("%s: Incorrect argument count\n", argv[0]);
        printUsage();
        return 0;
    }
    // Config
    bool binaryMode = false;
    bool findFirstOnly = false;
    bool ignoreCase = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "bil")) != -1) {
        switch (opt) {
        case 'b':
            binaryMode = true;
            break;
        case 'i':
            ignoreCase = true;
            break;
        case 'l':
            findFirstOnly = true;
            break;
        case '?':
            error = true;
            break;
        }
    }

    if (error) {
        printUsage();
        return 0;
    }

    if (argc - optind != min_arg) {
        printf("%s: Incorrect argument count\n", argv[0]);
        printUsage();
        return 0;
    }

    char* input = argv[optind];
    char* fileName = argv[++optind];

    if (check_wildcard(fileName)) {
        printf("%s: Find operation doesn't support wildcard\n", argv[0]);
        //filter_pattern = fileName;
        //char* dirname = "../";

        //struct dirent **files;
	    //int n = scandir(dirname, &files, filter, alphasort);
        //for (int i = 0; i < n; i++) {
        //    printf("%s\n", files[i]->d_name);
        //}

        // char* name;
        // int r = 0;

        // name = "file.cpp";
        // r = match_file(filter_pattern, name);
        // printf("match: %s %s %d\n", filter_pattern, name, r);

        // name = "dump.cpp";
        // r = match_file(filter_pattern, name);
        // printf("match: %s %s %d\n", filter_pattern, name, r);

        return 0;
    }

    size_t inputSize = strlen(input);
    size_t fileSize = 0;
 
    if (inputSize == 0) {
        printf("Input is empty\n");
        return 0;
    }

    char* data = readBytes(fileName, fileSize);

    if (data == NULL) {
        //printf("File '%s' not found\n", fileName);
        return 0;
    }

    if (fileSize == 0) {
        printf("File '%s' is empty\n", fileName);
        free(data);
        return 0;
    }

    Positions* positions = NULL;
    FindConfig* config = new FindConfig();
    config->binaryMode = binaryMode;
    config->findFirstOnly = findFirstOnly;
    config->ignoreCase = ignoreCase;

    if (binaryMode) {
        positions = findBinary(data, fileSize, input, inputSize, config);
    } else {
        positions = findText(data, fileSize, input, inputSize, config);
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
