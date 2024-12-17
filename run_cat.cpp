
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include "iolib.h"
#include "wstrlib.h"
#include "syslib.h"

void usage() {
    fprintf(stderr, "Usage: run-cat file ...\n");
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        usage();
        return 0;
    }

    int file_count = argc - 1;

    const char* file_names[file_count];
    for (int i = 1; i < argc; i++) {
        file_names[i - 1] = argv[i];
    }

    char* total_data = NULL;
    size_t total_size = 0;

    //char* total_data = lib_io_read_cat_bytes(file_names, file_count, &total_size);
    int retval = lib_io_read_cat_bytes(file_names, file_count, &total_data, &total_size);
    if (retval != 0 || !total_data) {
        free(total_data);
        return 1;
    }

    lib_sys_locale_init(); // WIN32 fast output with setvbuf(?)
    lib_io_buf_init();

    // incorrect last chars: non printable char
    //printf("%s", total_data);
    //printf("%ls", lib_char_wchar(total_data));   // IMPORTANT for WIN32 
    //wprintf(L"%ls", lib_char_wchar(total_data)); // IMPORTANT for WIN32

    for (int i = 0; i < total_size; i++) {
        printf("%c", total_data[i]);
    }

    lib_sys_locale_restore(); // Important for WIN32: The locale was changed for the terminal

    return 0;
}
