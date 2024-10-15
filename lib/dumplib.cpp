#include <stdio.h>
#include <cctype>

#include "iolib.h"
#include "dumplib.h"

void lib_dump_bytes(const char* data, size_t size, lib_dump_config_t* config) {
    if (data == NULL || size == 0) {
        return;
    }

    int col_count = config ? config->col_count : LIB_DUMP_DEF_COL_COUNT;
    if (col_count < LIB_DUMP_MIN_COL_COUNT || col_count > LIB_DUMP_MAX_COL_COUNT) {
      col_count = LIB_DUMP_DEF_COL_COUNT;
    }

    int row_count = size / col_count;
    if (size % col_count > 0) {
        row_count++;
    }

    bool show_text = config ? config->show_text : LIB_DUMP_DEF_SHOW_TEXT;

    //printf("size: %lu\n", size);
    //printf("cols: %d\n", col_count);
    //printf("rows: %d\n", row_count);
    //printf("text: %d\n", show_text);

    int offset = 0;
    int i = 0;
    for (int row = 0; row < row_count; row++) {
        offset = row * col_count;
        printf("0x%06X: ", (unsigned int) offset);

        // print data
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            printf(" %02X", (unsigned char) data[i]);
            //printf("%02hhx", data[i]);
            //printf("%02HHx", data[i]);
        }

        if (!show_text) {
            printf("\n");
            continue;
        }

        // data/text separator
        printf(" ");

        // last row:: fill rest size '___'
        int reserve_size = col_count * row_count;
        int rest_size = reserve_size - size;
        if (row == row_count - 1 &&  rest_size > 0) {
            for (int i = 0 ; i < rest_size; i++) {
                printf("   "); // _XX
            }
        }

        // print text
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            printf("%c", std::isprint(data[i]) ? data[i] : '.');
        }

        printf("\n");

    }

    /*
    for (int i = 0; i < size; i++) {
          printf(" %02X", (unsigned char) data[i]);
          //printf("%02hhx", data[i]);
          //printf("%02HHx", data[i]);
    }*/

}

void lib_dump_file_def(const char* file_name, lib_dump_config_t* config) {
    size_t size = 0;
    lib_dump_file(file_name, size, config);
}

void lib_dump_file(const char* file_name, size_t& size, lib_dump_config_t* config) {

  char* data = lib_io_read_bytes(file_name, size);
  lib_dump_bytes(data, size, config);

  free(data);  
}
