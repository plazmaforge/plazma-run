#include <stdio.h>
#include <cctype>

#include "iolib.h"
#include "dmplib.h"

lib_dmp_config_t* lib_dmp_file_new() {
    return (lib_dmp_config_t*) malloc(sizeof(lib_dmp_config_t));
}

void lib_dmp_file_free(lib_dmp_config_t* config) {
    if (!config) {
        return;
    }
    free(config);
}

void lib_dmp_dump_bytes(const char* data, size_t size, lib_dmp_config_t* config) {
    if (data == NULL || size == 0) {
        return;
    }

    int col_count = config ? config->col_count : LIB_DMP_DEF_COL_COUNT;
    if (col_count < LIB_DMP_MIN_COL_COUNT || col_count > LIB_DMP_MAX_COL_COUNT) {
      col_count = LIB_DMP_DEF_COL_COUNT;
    }

    int row_count = size / col_count;
    if (size % col_count > 0) {
        row_count++;
    }

    bool show_text = config ? config->show_text : LIB_DMP_DEF_SHOW_TEXT;

    //printf("size: %lu\n", size);
    //printf("cols: %d\n", col_count);
    //printf("rows: %d\n", row_count);
    //printf("text: %d\n", show_text);

    int offset = 0;
    int i = 0;

    int pos = 0;
    int stat_pos = -1;
    int total = 0;

    size_t buf_size = lib_io_stdout_get_buf_size();
    buf_size = lib_io_get_buf_page(buf_size);

    for (int row = 0; row < row_count; row++) {

        if (buf_size > 0 && i > 0) {
            int try_pos = (stat_pos > 0 ? stat_pos : 0);
            if (total + try_pos > buf_size) {
                fflush(stdout);
                total = 0;
                //pos = printf("%s\n", "FLUSH!!");
            }
        }

        offset = row * col_count;
        pos += printf("0x%06X: ", (unsigned int) offset);

        // print data
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            pos += printf(" %02X", (unsigned char) data[i]);
            //printf("%02hhx", data[i]);
            //printf("%02HHx", data[i]);
        }

        if (!show_text) {
            pos += printf("\n");
            continue;
        }

        // data/text separator
        pos += printf(" ");

        // last row:: fill rest size '___'
        int reserve_size = col_count * row_count;
        int rest_size = reserve_size - size;
        if (row == row_count - 1 &&  rest_size > 0) {
            for (int i = 0 ; i < rest_size; i++) {
                pos += printf("   "); // _XX
            }
        }

        // print text
        for (int col = 0; col < col_count; col++) {
            i = col + offset;
            if (i == size) {
                break;
            }
            pos += printf("%c", std::isprint(data[i]) ? data[i] : '.');
        }

        pos += printf("\n");

        if (stat_pos < 0) {
            stat_pos = pos;
            if (stat_pos < 0) {
                stat_pos = 0;
            }
        }

        total += pos;

    }

    /*
    for (int i = 0; i < size; i++) {
          printf(" %02X", (unsigned char) data[i]);
          //printf("%02hhx", data[i]);
          //printf("%02HHx", data[i]);
    }*/

}

void lib_dmp_dump_file_def(const char* file_name, lib_dmp_config_t* config) {
    size_t size = 0;
    lib_dmp_dump_file(file_name, size, config);
}

void lib_dmp_dump_file(const char* file_name, size_t& size, lib_dmp_config_t* config) {

  char* data = lib_io_read_bytes(file_name, size);
  lib_dmp_dump_bytes(data, size, config);

  free(data);  
}
