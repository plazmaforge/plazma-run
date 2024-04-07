#include <stdio.h>
#include <math.h>

#include "fslib.h"
#include "iolib.h"
#include "strlib.h"
#include "asklib.h"

int get_digit_count(int n) {
    if (n == 0) {
         return 1;
    }
    int value = abs(n);
    int count = (int) log10(value);
    count++;
    if (n < 0) {
        count++;
    }
    return count;
}

int get_width(int value) {
    int count = get_digit_count(value);
    return (count > 4) ? 0 : count;
}

void calc_width_index(const ask_positions_t* positions, int& index_width) {
    if (positions == NULL) {
        return;
    }
    ask_position_t* curr = positions->first;
    int max_index = 0;
    while (curr != NULL) {
        if (curr->index > max_index) {
            max_index = curr->index;
        }
        curr = curr->next;
    }
    index_width = get_width(max_index);
}

void calc_width_cell(const ask_positions_t* positions, int& row_width, int& col_width) {
    if (positions == NULL) {
        return;
    }
    ask_position_t* curr = positions->first;
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

ask_config_t* ask_config_new() {
    ask_config_t* config = (ask_config_t*) malloc(sizeof(ask_config_t));
    if (!config) {
        return NULL;
    }
    config->binary_mode = false;
    config->find_first_only = false;
    config->ignore_case = false;
    config->print_file_name = false;
    return config;
}

ask_position_t* ask_position_new() {
    ask_position_t* position = (ask_position_t*) malloc(sizeof(ask_position_t));
    if (!position) {
        return NULL;
    }
    position->index = 0;
    position->col = 0;
    position->row = 0;
    position->start_row_index = 0;
    position->end_row_index = 0;
    position->prev = NULL;
    position->next = NULL;
    return position;
}

ask_positions_t* ask_positions_new() {
    ask_positions_t* positions = (ask_positions_t*) malloc(sizeof(ask_positions_t));
    if (!positions) {
        return NULL;
    }
    positions->first = NULL;
    positions->last = NULL;
    positions->size = 0;
    return positions;
}

////

ask_positions_t* ask_find_binary(const char* data, size_t data_size, const char* input, size_t input_size, const ask_config* config) {
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
        find_first_only = config->find_first_only;
    }

    ask_positions_t* positions = NULL;
    ask_position_t* curr = NULL;
    
    for (size_t pos = 0; pos < data_size; pos++) {

        if (pos + input_size > data_size) {
            return positions;
        }

        bool found = contains(data, input, pos, input_size);

        if (found) {
            ask_position_t* position = ask_position_new();
            if (!position) {
                // error
                return positions;
            }
            position->index = pos;
            position->prev = NULL;
            position->next = NULL;

            if (positions == NULL) {
                curr = position;
                positions = ask_positions_new();
                if (!positions) {
                    // error
                    return positions;
                }
                positions->first = curr;
                positions->last = curr;
                positions->size = 1;
            } else {
                ask_position_t* prev = curr;
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

void fixed_end_row_index(const char* data, size_t data_size, const char* input, size_t input_size, ask_position_t* curr) {
    if (curr == NULL) {
        return;
    }
    if (curr->end_row_index > curr->start_row_index + input_size) {
        return;
    }
    if (curr->index >= data_size) {
        return;
    }

    //printf("input_size: %lu\n", input_size);
    //printf("startRowIndex: %d\n", curr->start_row_index);
    //printf("endRowIndex: %d\n", curr->end_row_index);

    for (size_t pos = curr->index; pos < data_size; pos++) {
        char c = data[pos];
        if (c == '\r' || c == '\n') {
            curr->end_row_index = pos;
            break;
        }
    }
    //printf("endRowIndex: %d\n", curr->end_row_index);     
}

ask_positions_t* ask_find_text(const char* data, size_t data_size, const char* input, size_t input_size, const ask_config* config) {
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
        find_first_only = config->find_first_only;
        ignore_case = config->ignore_case;
    }

    int row = 1;
    int col = 1;
    int skip = 0;
    bool newline = false;

    ask_positions_t* positions = NULL;
    ask_position_t* curr = NULL;
    
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
                curr->end_row_index = pos;
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
            ask_position_t* position = ask_position_new();
            if (!position) {
                // error
                return positions;
            }
            position->index = pos;
            position->row = row;
            position->col = col;
            position->start_row_index = start_row_index;
            position->end_row_index = start_row_index + input_size;
            position->prev = NULL;
            position->next = NULL;

            if (curr != NULL && curr->row == row) {
                fixed_end_row_index(data, data_size, input, input_size, curr);
            }

            if (positions == NULL) {
                curr = position;
                positions = ask_positions_new();
                if (!positions) {
                    // error
                    return NULL;
                }
                positions->first = curr;
                positions->last = curr;
                positions->size = 1;
            } else {
                ask_position_t* prev = curr;
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

void ask_print_text_position(const ask_position_t* position, const char* data, int data_size, int input_size, const char* format, int width) {
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
        start_index = position->start_row_index; // full line
        end_index = position->end_row_index;     // full line
        end_index2 = end_index;
        pad = position->index - position->start_row_index; // TODO: count char - not len
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
      int row_width = get_digit_count(position->row);
      int col_width = get_digit_count(position->col);

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

void ask_print_text(const ask_positions_t* positions, const char* data, int data_size, int input_size) {
    if (!positions) {
        return;
    }
    ask_position_t* curr = positions->first;
    if (!curr) {
        return;
    }

    bool fixed_format = true;
    int width = 0;
    char format[50];

    if (fixed_format) {
        strcpy(format, "[%d: %d] ");
    } else {
        int row_width = 0; 
        int col_width = 0;
        calc_width_cell(positions, row_width, col_width);
        width = row_width + col_width + 5;

        sprintf(format, "[_%id: _%id] ", row_width, col_width);
        lib_replace_len(format, sizeof(format) / sizeof(char), '_', '%');
    }

    while (curr) {
        ask_print_text_position(curr, data, data_size, input_size, format, width);
        curr = curr->next;
    }
}

void ask_print_binary_position(const ask_position_t* position, const char* data, int data_size, int input_size, const char* format, int width) {
    if (!position) {
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
      width = get_digit_count(position->index);
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

void ask_print_binary(const ask_positions_t* positions, const char* data, int data_size, int input_size) {
    if (!positions) {
        return;
    }
    ask_position_t* curr = positions->first;
    if (!curr) {
        return;
    }

    bool fixed_format = true;
    int width = 0;
    char format[50];

    if (fixed_format) {
        strcpy(format, "[%d] ");
    } else {
        int index_width = 0; 
        calc_width_index(positions, index_width);
        width = index_width + 3;
        sprintf(format, "[_%id] ", index_width);
        lib_replace_len(format, sizeof(format) / sizeof(char), '_', '%');
    }

    while (curr) {
        ask_print_binary_position(curr, data, data_size, input_size, format, width);
        curr = curr->next;
    }
}

void ask_positions_destroy(ask_positions_t* positions) {
    if (!positions) {
        return;
    }
    ask_position_t* curr = positions->first;
    ask_position_t* next = NULL;
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

ask_positions_t* ask_find_data(const char* data, size_t data_size, const char* input, size_t input_size, const ask_config* config) {
    if (config->binary_mode) {
        return ask_find_binary(data, data_size, input, input_size, config);
    } else {
        return ask_find_text(data, data_size, input, input_size, config);
    }
}

void ask_print_data(const ask_positions_t* positions, const char* data, int data_size, int input_size, const ask_config* config) {
    if (config->binary_mode) {
        ask_print_binary(positions, data, data_size, input_size);
    } else {
        ask_print_text(positions, data, data_size, input_size);
    }
}

////

void ask_find(const char* file_name, const char* input, int input_size, const ask_config* config) {

    size_t file_size = 0;
    char* data = read_bytes_size(file_name, file_size);

    if (!data) {
        //printf("File '%s' not found\n", fileName);
        return;
    }

    if (file_size == 0) {
        //printf("File '%s' is empty\n", fileName);
        free(data);
        return;
    }

    ask_positions_t* positions = ask_find_data(data, file_size, input, input_size, config);
    
    if (!positions) {
        //printf("Not found\n");
        free(data);
        return;
    }

    if (config->print_file_name) {
        char* real_path = lib_fs_get_real_path(file_name);
        printf(">> %s\n", real_path);
        free(real_path);
    }

    ask_print_data(positions, data, file_size, input_size, config);

    ask_positions_destroy(positions);
    free(data);

}
