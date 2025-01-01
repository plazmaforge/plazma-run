#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "iolib.h"
#include "dmplib.h"

int lib_dmp_dump_bytes(const char* data, size_t size, lib_dmp_config_t* config) {

    #ifdef LIB_DEBUG
    fprintf(stderr, ">> dmp_dump_bytes: size=%lu\n", size);
    #endif

    if (!data) {
        #ifdef LIB_ERROR
        fprintf(stderr, "ERROR: Invalid arguments: data\n");
        #endif
        return -1;
    }

    if (size == 0) {
        #ifdef LIB_DEBUG
        fprintf(stderr, ">> dmp_dump_bytes: return: size=%lu\n", size);
        #endif
        return 0;
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

    size_t buf_size = lib_io_stdout_get_buf_size();
    buf_size = lib_io_get_buf_page(buf_size);

    int offset = 0;
    int i = 0;

    int pos = 0;
    int total = 0;

    /* "0x%06X: "  2 + (6) + 2 = 10      */
    /* " %02X"     (1 + (2)) * col_count */

    /* " "         1                     */
    /* "c"         col_count             */
    /* "n"         1                     */

    int row_size = 10 + (3 * col_count) + 1;
    if (show_text) {
        row_size += (1 + col_count);
    }

    #ifdef LIB_DEBUG
    fprintf(stderr, ">> dmp_dump_bytes: config:\n");
    fprintf(stderr, ">> size=%lu\n", size);
    fprintf(stderr, ">> col_count=%d\n", col_count);
    fprintf(stderr, ">> row_count=%d\n", row_count);
    fprintf(stderr, ">> show_text=%d\n", show_text);
    fprintf(stderr, ">> row_size=%d\n", row_size);

    fprintf(stderr, ">> dmp_dump_bytes: start\n");
    #endif

    for (int row = 0; row < row_count; row++) {

        pos = 0; // reset pos for row

        #ifdef LIB_DEBUG_LL
        fprintf(stderr, ">> dmp_dump_bytes: row=%d\n", row);
        #endif

        if (buf_size > 0 && i > 0) {
            //int try_pos = (stat_pos > 0 ? stat_pos : 0);
            if (total + row_size > buf_size) {
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
            if (i >= size) {
                break;
            }
            pos += printf(" %02X", (unsigned char) data[i]);
            //printf("%02hhx", data[i]);
            //printf("%02HHx", data[i]);
        }

        if (!show_text) {
            pos += printf("\n");
            total += pos;
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
            if (i >= size) {
                break;
            }
            pos += printf("%c", isprint(data[i]) ? data[i] : '.');
        }

        pos += printf("\n");
        total += pos;

        /*
        if (stat_pos < 0) {
            stat_pos = pos;
            if (stat_pos < 0) {
                stat_pos = 0;
            }
        }
        */        
    }

    fflush(stdout);

    #ifdef LIB_DEBUG
    fprintf(stderr, ">> dmp_dump_bytes: finish\n");
    #endif

    return 0;

}

static int _lib_dmp_dump_file(const char* file_name, size_t size, lib_dmp_config_t* config) {

    #ifdef LIB_DEBUG
    fprintf(stderr, ">> dmp_dump_file: size=%lu\n", size);
    #endif

    if (!file_name) {
        #ifdef LIB_ERROR
        fprintf(stderr, "ERROR: Invalid arguments: file_name\n");
        #endif
        return -1;
    }

    char* data       = NULL;
    size_t data_size = size;
    int retval       = 0;

    if (size == 0) {
        retval = lib_io_read_all_bytes(file_name, &data, &data_size);
    } else {
        retval = lib_io_read_bytes(file_name, &data, size, &data_size);
    } 
  
    if (retval < 0) {
        free(data);
        return -1;
    }

    retval = lib_dmp_dump_bytes(data, data_size, config);

    free(data);
    return retval;
}

int lib_dmp_dump_file_all(const char* file_name, lib_dmp_config_t* config) {
    return _lib_dmp_dump_file(file_name, 0, config);
}

int lib_dmp_dump_file(const char* file_name, size_t size, lib_dmp_config_t* config) {
    if (size == 0) {
        return 0;
    }
    return _lib_dmp_dump_file(file_name, size, config);
}
