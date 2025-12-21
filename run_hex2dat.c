#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "getopt.h"
#include "clilib.h"
#include "iolib.h"

static int _check_byte(char b) {
    return ((b >= '0' && b <= '9') 
      || (b >= 'a' && b <= 'f')
      || (b >= 'A' && b <= 'F')
      );
}

int run_hex2dat_data(char* data, size_t size) {
    
    if (size == 0 || !data) {
        fprintf(stderr, "%s: No input data\n", prog_name);
        return 1;
    }

    if (size % 2 > 0) {
        fprintf(stderr, "%s: Invalid input data: file_size=%lu, mod(file_size, 2)=%lu > 0\n", prog_name, size, size % 2);
        //fprintf(stderr, "Invalid input data: file size [mod] 2 > 0\n");
        //fprintf(stderr, "Invalid input data: file size [mod] 2 > 0. Ignore last byte.\n");
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

int run_hex2dat_file(const char* file_name) {
    
    if (!file_name) {
        fprintf(stderr, "%s: File name is empty\n", prog_name);
        return 1;
    }

    char* data  = NULL;
    size_t size = 0;
    int error = lib_io_read_all_bytes(file_name, &data, &size);
    if (error != 0) {
        fprintf(stderr, "%s: I/O error\n", prog_name);
        if (data) {
            free(data);
        }        
        return 1;
    }

    int i = size - 1;
    while (i >= 0) {

        // Truncate data by CR/LF.

        // Classic MacOS  : CR   = \r, 
        // Unix and MacOS : LF   = \n, 
        // Windows        : CRLF = \r\n.

        if (data[i] == '\n') {
            size--;          // NIX/MAC, WIN (\r\n?)
            if (i - 1 >= 0) {
                if (data[i - 1] == '\r') {
                    size--; // WIN
                    i--;
                }
            }
        } else if (data[i] == '\r') {
            size--;          // MAC (Classic)
        }

        i--;
    }

    error = run_hex2dat_data(data, size);
    if (data) {
        free(data);
    }
    return error;    
}

void usage() {
    fprintf(stderr, "Usage: run-hex2dat -s string | file\n");
}

int main(int argc, char* argv[]) {

    prog_name = lib_cli_prog_name(argv);

    if (argc < 2) {
        usage();
        return 0;
    }

    int error = 0;
    int opt;

    bool flag_string       = false;
    char* data             = NULL;
    size_t size            = 0;
    const char* file_name  = NULL;

    while ((opt = lib_getopt(argc, argv, "s:")) != -1) {        
        switch (opt) {
        case 's':
            flag_string = true;
            data = optarg;
            break;
        case '?':
            error = 1;
            break;
        case ':':
            error = 1;
            break;
        }
    }

    if (error) {
        usage();
        return 1;
    }

    if (flag_string) {
        //BY_STRING
        //fprintf(stderr, ">> BY_STRING: optind=%d\n", optind);

        if (argc - optind >= 1) {
            fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
            usage();
            return 1;
        }

        size = strlen(data);
        return run_hex2dat_data(data, size);

    } else {
        //BY_FILE
        //fprintf(stderr, ">> BY_FILE: optind=%d\n", optind);

        //if (argc - optind < 1) {
        //    fprintf(stderr, "%s: Incorrect argument count\n", prog_name);
        //    usage(config);
        //    return 1;
        //}

        const char* file_name = argv[1]; // argv[optind];
        return run_hex2dat_file(file_name);
    }
}