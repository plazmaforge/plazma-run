#include <stdio.h>

#include "fslib.h"
#include "iolib.h"
#include "asklib.h"

int get_width(int value) {
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

void calc_width_index(const Positions* positions, int& index_width) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;    
    int max_index = 0;
    while (curr != NULL) {
        if (curr->index > max_index) {
            max_index = curr->index;
        }
        curr = curr->next;
    }
    index_width = get_width(max_index);
}

void calc_width_cell(const Positions* positions, int& row_width, int& col_width) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;    
    int max_row = 0;
    int max_col = 0;
    while (curr != NULL) {
        if (curr->row > max_row) {
            max_row = curr->row;
        }
        if (curr->col > max_col) {
            max_col = curr->col;
        }
        curr = curr->next;
    }
    row_width = get_width(max_row);
    col_width = get_width(max_col);
    if (row_width == 0 || col_width == 0) {
      row_width = 0;
      col_width = 0;
    }
}

int skip_line(const char* data, size_t data_size, size_t pos) {
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
    for (size_t i = 0; i < size; i++) {
        if (data[start + i] != input[i]) {
            return false;
        }
    }
    return true;
}

bool icontains(const char* data, const char* input, int start, int size) {
    for (size_t i = 0; i < size; i++) {
        if (std::tolower(data[start + i]) != std::tolower(input[i])) {
            return false;
        }
    }
    return true;
}

////

Positions* ask_find_binary(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    // by default
    bool find_first_only = false;

    // by config
    if (config != NULL) {
        find_first_only = config->findFirstOnly;
    }

    Positions* positions = NULL;
    Position* curr = NULL;
    
    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            return positions;
        }

        bool found = contains(data, input, pos, input_size);

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

            if (find_first_only) {
                return positions;
            }
        }

    }
    return positions;
}

void fixed_end_row_index(const char* data, size_t data_size, const char* input, size_t input_size, Position* curr) {
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

Positions* ask_find_text(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (data == NULL || input == NULL || data_size == 0 || input_size == 0) {
        return NULL;
    }

    if (input_size > data_size) {
        return NULL;
    }

    // by default
    bool find_first_only = false;
    bool ignore_case = false;

    // by config
    if (config != NULL) {
        find_first_only = config->findFirstOnly;
        ignore_case = config->ignoreCase;
    }

    int row = 1;
    int col = 1;
    int skip = 0;
    bool newline = false;

    Positions* positions = NULL;
    Position* curr = NULL;
    
    int start_row_index = 0;
    int end_row_index = 0;

    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            fixed_end_row_index(data, data_size, input, input_size, curr);
            return positions;
        }

        skip = 0;
        newline = false;
        while ((skip = skip_line(data, data_size, pos)) > 0) {
            
            if (curr != NULL && !newline) {
                curr->endRowIndex = pos;
            }
            newline = true;
            pos += skip;
            row++;
            if (pos >= data_size) {
                fixed_end_row_index(data, data_size, input, input_size, curr);
                return positions;
            }
        }

        if (newline) {
            col = 1;
            start_row_index = pos; // start row position
        } else if (pos > 0) {
            col++;
        }

        bool found = ignore_case ? icontains(data, input, pos, input_size) : contains(data, input, pos, input_size);

        if (found) {
            Position* position = new Position();
            position->index = pos;
            position->row = row;
            position->col = col;
            position->startRowIndex = start_row_index;
            position->endRowIndex = start_row_index + input_size;
            position->prev = NULL;
            position->next = NULL;

            if (curr != NULL && curr->row == row) {
                fixed_end_row_index(data, data_size, input, input_size, curr);
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

            if (find_first_only) {
                fixed_end_row_index(data, data_size, input, input_size, curr);
                return positions;
            }
        }

    }

    fixed_end_row_index(data, data_size, input, input_size, curr);
    return positions;
}

void ask_print_text_position(const Position* position, const char* data, int data_size, int input_size, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    bool print_full_line = true;
    bool print_under_line = false;

    int start_index = 0;
    int end_index = 0;
    int end_index2 = 0;
    int pad = 0;
    int count = input_size; // TODO: count char - not len

    if (print_full_line) {
        start_index = position->startRowIndex; // full line
        end_index = position->endRowIndex;     // full line
        end_index2 = end_index;
        pad = position->index - position->startRowIndex; // TODO: count char - not len
    } else {
        start_index = position->index;
        end_index = start_index + input_size;

        int delta = 10;
        int rest = data_size - end_index;
        delta = rest < delta ? rest : delta;
        end_index2 = end_index + delta;
    }

    char c;

    printf(format, position->row, position->col);
    for (size_t i = start_index; i < end_index2; i++)  {
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

    if (!print_under_line) {
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

void ask_print_text(const Positions* positions, const char* data, int data_size, int input_size) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;

    bool fixed_format = true;
    int width = 0;
    std::string format;

    if (fixed_format) {
        format = "[%d: %d] ";
    } else {
        int rowWidth = 0; 
        int colWidth = 0;
        calc_width_cell(positions, rowWidth, colWidth);
        width = rowWidth + colWidth + 5;
        format = "[%" + std::to_string(rowWidth) + "d: %" + std::to_string(colWidth) + "d] ";

    }
    const char* fmt = format.c_str();
    while (curr != NULL) {
        ask_print_text_position(curr, data, data_size, input_size, fmt, width);
        curr = curr->next;
    }
}

void ask_print_binary_position(const Position* position, const char* data, int data_size, int input_size, const char* format, int width) {
    if (position == NULL) {
        return;
    }
    int start_index = position->index; 
    int end_index = start_index + input_size;

    int delta = 10;
    int rest = data_size - end_index;
    delta = rest < delta ? rest : delta;
    int end_index2 = end_index + delta;
    char c;

    printf(format, position->index);

    for (size_t i = start_index; i < end_index2; i++)  {
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
    for (size_t i = start_index; i < end_index; i++)  {
        printf("_");
    }

    printf("\n");

}

void ask_print_binary(const Positions* positions, const char* data, int data_size, int input_size) {
    if (positions == NULL) {
        return;
    }
    Position* curr = positions->first;

    bool fixed_format = true;
    int width = 0;
    std::string format;

    if (fixed_format) {
        format = "[%d] ";
    } else {
        int index_width = 0; 
        calc_width_index(positions, index_width);
        width = index_width + 3;
        format = "[%" + std::to_string(index_width) + "d] ";
    }
    const char* fmt = format.c_str();
    while (curr != NULL) {
        ask_print_binary_position(curr, data, data_size, input_size, fmt, width);
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

Positions* ask_find_data(const char* data, size_t data_size, const char* input, size_t input_size, const FindConfig* config) {
    if (config->binaryMode) {
        return ask_find_binary(data, data_size, input, input_size, config);
    } else {
        return ask_find_text(data, data_size, input, input_size, config);
    }
}

void ask_print_data(const Positions* positions, const char* data, int data_size, int input_size, const FindConfig* config) {
    if (config->binaryMode) {
        ask_print_binary(positions, data, data_size, input_size);
    } else {
        ask_print_text(positions, data, data_size, input_size);
    }
}

////

void ask_find(const char* file_name, const char* input, int input_size, const FindConfig* config) {

    size_t file_size = 0;
    char* data = read_bytes_size(file_name, file_size);

    if (data == NULL) {
        //printf("File '%s' not found\n", fileName);
        return;
    }

    if (file_size == 0) {
        //printf("File '%s' is empty\n", fileName);
        free(data);
        return;
    }

    Positions* positions = ask_find_data(data, file_size, input, input_size, config);
    
    if (positions == NULL) {
        //printf("Not found\n");
        free(data);
        return;
    }

    if (config->printFileName) {
        char* real_path = fs_get_real_path(file_name);
        printf(">> %s\n", real_path);
        free(real_path);
    }

    ask_print_data(positions, data, file_size, input_size, config);

    destroy(positions);
    free(data);

}
