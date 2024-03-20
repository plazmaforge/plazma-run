#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "iolib.h"

void usage() {
    printf("Usage: run-hex-byte <file>\n");
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
    size_t size = 0;
    char* data = read_bytes_size(file_name, size);
    if (size == 0 || !data) {
        fprintf(stderr, "No input data\n");
        return 1;
    }
    if (size % 2 > 0) {
        free(data);
        fprintf(stderr, "Invalid input data: file size % 2 > 0\n");
        return 1;
    }

    size_t out_size = size / 2;
    char* out_data = (char*) malloc(out_size);
    int i = 0;
    int j = 0;
    char hex_str[2];
    unsigned long hex_val;
    char c;

    while (i < size) {
        char b1 = data[i];
        char b2 = data[++i];
        i++;
        j++;

        if (!check_byte(b1) || !check_byte(b2)) {
           free(data);
           free(out_data);
           fprintf(stderr, "Invalid byte data\n");
           return 1;
        }
        hex_str[0] = b1;
        hex_str[1] = b2;

        hex_val = strtoul(hex_str, NULL, 16);
        c = (char) hex_val;
        out_data[j] = c;
        //printf("%02X", c);
        printf("%c", c);
    }
    
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }
    const char* file_name = argv[1];
    
    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    run_hex_byte(file_name);

}