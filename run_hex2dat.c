#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "arglib.h"
#include "iolib.h"

static int _check_byte(char b) {
    return ((b >= '0' && b <= '9') 
      || (b >= 'a' && b <= 'f')
      || (b >= 'A' && b <= 'F')
      );
}

int run_hex2dat(const char* file_name) {
    
    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    ////
    char* data  = NULL;
    size_t size = 0;

    int retval = lib_io_read_all_bytes(file_name, &data, &size);
    if (retval < 0) {
        fprintf(stderr, "%s: I/O error\n", prog_name);
        free(data);
        return 1;
    }
    ////

    //fprintf(stderr, "DEBUG: file_size=%lu\n", size);

    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        free(data);
        return 1;
    }

    if (size % 2 > 0) {
        fprintf(stderr, "%s: Invalid input data: file_size=%lu, mod(file_size, 2)=%lu > 0\n", prog_name, size, size % 2);
        //fprintf(stderr, "Invalid input data: file size [mod] 2 > 0\n");
        //fprintf(stderr, "Invalid input data: file size [mod] 2 > 0. Ignore last byte.\n");
        free(data);
        return 1;
    }

    size_t out_size = size / 2;
    char* out_data = (char*) malloc(out_size);
    int i = 0;
    int j = 0;

    char hex_str[3];
    hex_str[2] = '\0';

    unsigned long hex_val;
    char c;

    while (i < size) {
        char b1 = data[i];
        char b2 = data[++i];

        //printf("%c", b1);
        //printf("%c", b2);

        i++;
        j++;

        if (!_check_byte(b1) || !_check_byte(b2)) {
           fprintf(stderr, "%s: Invalid byte data\n", prog_name);
           //fprintf(stderr, "Invalid byte data. Ignore following data\n");
           free(data);
           free(out_data);
           return 1;
           //break;
        }

        hex_str[0] = b1;
        hex_str[1] = b2;

        hex_val = strtoul(hex_str, NULL, 16);
        c = (char) hex_val;
        out_data[j] = c;

        //printf("%02X", c);
        printf("%c", c);
    }

    return 0;
}

void usage() {
    fprintf(stderr, "Usage: run-hex2dat file\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    prog_name = lib_arg_prog_name(argv);

    const char* file_name = argv[1];
    
    //lib_io_buf_init();

    return run_hex2dat(file_name);

}