#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "iolib.h"

void usage() {
    fprintf(stderr, "Usage: run-hex-byte file\n");
}

int check_byte(char b) {
    return ((b >= '0' && b <= '9') 
      || (b >= 'a' && b <= 'f')
      || (b >= 'A' && b <= 'F')
      );
}

int run_hex_byte(const char* file_name) {
    
    if (!file_name) {
        fprintf(stderr, "File name is empty\n");
        return 1;
    }

    //size_t size = 0;
    //char* data = lib_io_read_bytes(file_name, size);

    ////
    char* data = NULL;
    size_t size = 0;
    int retval = lib_io_read_all_bytes(file_name, &data);
    if (retval < 0) {
        // error
        return 1;
    }
    size = retval;
    ////

    if (size == 0 || !data) {
        fprintf(stderr, "No input data\n");
        if (data) {
            free(data);
        }        
        return 1;
    }

    if (size % 2 > 0) {
        fprintf(stderr, "Invalid input data: file size [mod] 2 > 0\n");
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
        i++;
        j++;

        if (!check_byte(b1) || !check_byte(b2)) {
           fprintf(stderr, "Invalid byte data\n");
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

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }
    const char* file_name = argv[1];
    
    lib_io_buf_init();

    return run_hex_byte(file_name);

}