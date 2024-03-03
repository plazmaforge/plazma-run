
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//#include <string>
//#include <vector>

#include "getopt.h"
#include "strlib.h"
#include "wstrlib.h"
#include "pathlib.h"
#include "wclib.h"
#include "fslib.h"
#include "syslib.h"
#include "asklib.h"

void printUsage() {
    printf("Usage: run-find <text> <file> \n");
}

int main(int argc, char* argv[]) {

    /*
    init_locale();        
    printf("Locale: %s\n", setlocale(LC_ALL, NULL));  
    //setlocale(LC_ALL, "en_US.UTF-8"); // OK
    //setlocale(LC_ALL, "UTF-8");       // FAIL
    //setlocale(LC_ALL, "");              // OK
    //setlocale(LC_ALL, "English_United States.1251");
    printf("Locale: %s\n", setlocale(LC_ALL, NULL));
    */

    //for (int i = 0; i < argc; i++) {
    //   printf("%d: %s\n", i, argv[i]);
    //}

    /*
    const char* str = "\xe4\xbd\xa0\xe5\xa5\xbd";
    wchar_t* wstr = char_wchar(str);

    printf("MB-c-STR: %s\n", str);
    //wprintf(L"MB-w string: %s\n", str);
    wprintf(L"WC-STR: %ls\n", wstr);
    wprintf(L"WC-LEN: %d\n", wcslen(wstr));
    */
        
    /*
    for (int i = 0; i < argc; i++) {
        char* str = argv[i];
        wchar_t* wstr = achar_wchar(str); // WIN32 (ANSII): achar_wchar + wchar_achar - correct only!
        char* str2 = wchar_achar(wstr);

        wprintf(L"\n");

        printf("%d: MB-c-STR %s\n", i, str);
        //wprintf(L"%d: MB-w-STR %s\n", i, str);
        wprintf(L"%d: MB-LEN %d\n", i, strlen(str));
        wprintf(L"%d: WC-STR %ls\n", i, wstr);
        wprintf(L"%d: WC-LEN %d\n", i, wcslen(wstr));
        printf("%d: MB-c-STR %s\n", i, str2);
        //wprintf(L"%d: MB-w-STR %s\n", i, str2);
        wprintf(L"%d: MB-LEN %d\n", i, strlen(str2));
    }
    reset_locale();
    */ 
       
    int min_arg = 2; // <text> <file>
    if (argc < min_arg + 1) {
        printf("%s: Incorrect argument count\n", argv[0]);
        printUsage();
        return 0;
    }

    // Config
    bool binary_mode = false;
    bool find_first_only = false;
    bool ignore_case = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "bil")) != -1) {
        switch (opt) {
        case 'b':
            binary_mode = true;
            break;
        case 'i':
            ignore_case = true;
            break;
        case 'l':
            find_first_only = true;
            break;
        case '?':
            error = true;
            break;
        }
    }

    if (error) {
        printUsage();
        return 0;
    }

    if (argc - optind != min_arg) {
        printf("%s: Incorrect argument count\n", argv[0]);
        printUsage();
        return 0;
    }
                                        
    char* input = lib_strdup_uq(argv[optind]);
    char* file_name = lib_strdup_uq(argv[++optind]);

    //printf("input: %s\n", input);
    //printf("file : %s\n", file_name);

    size_t input_size = strlen(input);
    if (input_size == 0) {
        //printf("Input is empty\n");
        return 0;
    }

    init_locale();

    ask_config* config = ask_config_new();
    config->binary_mode = binary_mode;
    config->find_first_only = find_first_only;
    config->ignore_case = ignore_case;
    config->print_file_name = true; // TODO: add optional

    int wildcard_index = wc_get_wildcard_index(file_name);

    if (wildcard_index >= 0) {

        int path_index = wc_get_wildcard_path_index(wildcard_index, file_name);
        char* dir_name = NULL;

        if (path_index >= 0) {
            dir_name = lib_strndup(file_name, path_index + 1);
            file_name = file_name + path_index + 1;
        } else {
            dir_name = lib_strdup(fs_get_current_find_path());            
        }

        //printf("dir  : %s\n", dir_name);
        //printf("file : %s\n", file_name);

        char* pattern = file_name;         
        fs_file_t** files = NULL;
        fs_file_t* file = NULL;
        int file_count = fs_scandir(dir_name, pattern, &files, FS_SCANDIR_RECURSIVE, true);

        for (int i = 0; i < file_count; i++) {
            file = files[i];
            ask_find(files[i]->name, input, input_size, config);

            //free(file);
            //fs_file_free(file);
        }

        fs_files_free(files);
                
        free(dir_name);
        free(config);
        restore_locale(); // Important for WIN32: The locale was changed for the terminal
        return 0;
    }

    ask_find(file_name, input, input_size, config);

    free(config);
    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;

}
