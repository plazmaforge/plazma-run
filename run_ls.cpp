#include <stdlib.h>
#include <stdio.h>

#include "getopt.h"
#include "fslib.h"

void printUsage() {
    printf("Usage: run-ls\n");
}

int main(int argc, char* argv[]) {
    
    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "al")) != -1) {
        switch (opt) {
        case 'a':
            // TODO
            break;
        case 'l':
            // TODO
            break;
        case '?':
            error = true;
            break;
        }
    }

    char* dir_name = fs_get_current_dir();
    char* pattern = NULL;
    fs_file_t** files = NULL;
    fs_file_t* file = NULL;
    int file_count = fs_scandir(dir_name, pattern, &files, FS_SCANDIR_FLAT);

    if (file_count <= 0) {
        fs_files_free(files);
        free(dir_name);
        return 0;
    }

    for (int i = 0; i < file_count; i++) {
        file = files[i];
        char* path = file->name;
        char* name = fs_get_base_name(path);
        uint64_t size = fs_file_get_file_size(file);

        // 1 Kilobyte = 1,024 Bytes
        // 1 Megabyte = 1,048,576 Bytes 
        // 1 Gigabyte = 1,073,741,824 Bytes

        const char* GB = "Gb";
        const char* MB = "Mb";
        const char* KB = "Kb";

        const char* unit = NULL;

        if (size > 1073741824) {
            size = size / 1073741824;
            unit = GB;
        } else if (size > 1048576) {
            size = size / 1048576;
            unit = MB;
        //} else if (size > 1024) {
        //    size = size / 1024;
        //    unit = KB;
        } else {
            unit = "";
        }

        printf("%10llu %s %s\n", size, unit, name);
        free(name);
    }

    fs_files_free(files);                        
    free(dir_name);

    return 0;
}
