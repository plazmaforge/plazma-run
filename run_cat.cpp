
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "iolib.h"
#include "wstrlib.h"
#include "syslib.h"

struct file_data_t {
    char* data;
    size_t size;
};

void usage() {
    printf("Usage: run-cat <file...>\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    int file_count = argc - 1;

    file_data_t** file_list = (file_data_t**) malloc(sizeof(file_data_t) * file_count);

    char* file_name = NULL;
    file_data_t* file_data = NULL;
    char* allData = NULL;
    size_t file_size = 0;
    size_t total_size = 0;

    for (int i = 1; i < argc; i++) {
        file_name = argv[i];
        file_size = 0;

        file_data = (file_data_t*) malloc(sizeof(file_data_t));
        file_data->data = lib_io_read_bytes_size(file_name, file_size);
        file_data->size = file_size;

        file_list[i - 1] = file_data;
        total_size += file_size; 
    }

    // V1
    char* total_data = (char*) malloc(total_size * sizeof(char));
    int offset = 0;

    for (int j = 0; j < file_count; j++) {

        file_data = file_list[j];
        
        for (int i = 0; i < file_data->size; i++) {
            total_data[i + offset] = file_data->data[i];
        }

        offset += file_data->size;

    }

    init_locale(); // WIN32 fast output with setvbuf(?)

    #ifdef _WIN32
    char buf[65536];
    setvbuf(stdout, buf, _IOFBF, 65536);
    #endif

    // incorrect last chars: non printable char
    //printf("%s", total_data);
    //printf("%ls", char_wchar(total_data));   // IMPORTANT for WIN32 
    //wprintf(L"%ls", char_wchar(total_data)); // IMPORTANT for WIN32

    for (int i = 0; i < total_size; i++) {
        printf("%c", total_data[i]);
    }

    restore_locale(); // Important for WIN32: The locale was changed for the terminal
}