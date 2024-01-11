
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <cctype>
#include <vector>

#include <locale.h>

#include "getopt.h"
#include "strlib.h"
#include "wstrlib.h"
#include "pathlib.h"
#include "wclib.h"
#include "fslib.h"
#include "srhlib.h"
#include "syslib.h"

#ifdef _WIN32
#include <windows.h>
#endif

void printSysInfo() {
    printf("\n");
    printf("user.name: %s\n", getUserName());
    printf("user.home: %s\n", getUserHome());
    printf("user.dir : %s\n", getUserDir());
    printf("tmp.dir  : %s\n", getTmpDir());
}

void printUsage() {
    printf("Usage: run-find text file \n");
    printSysInfo();
}

////

static char* _locale;

#ifdef _WIN32
static UINT _cp;
static UINT _out_cp;
#endif

void init_locale() {
    int debug = 1;
    int check = 1;
    _locale = setlocale(LC_ALL, NULL);
    if (debug) {
      printf("Get Locale: %s\n", _locale);
    }
    
    if (debug && check) {
      printf("Def Locale: %s\n", setlocale(LC_ALL, ""));
      setlocale(LC_ALL, _locale);
    }
   
    #ifdef _WIN32
    _cp = GetConsoleCP();
    _out_cp = GetConsoleOutputCP();
    if (debug) {
      printf("Get ConsoleCP      : %d\n", _cp);
      printf("Get ConsoleOutputCP: %d\n", _out_cp);
    }

    if (_out_cp == 65001) {
        // UTF-8
        return;
    }

    UINT _new_cp = 0;
    if (_out_cp == 866) {
       _new_cp = 1251;       
    }

    if (_new_cp > 0) {
       SetConsoleCP(_new_cp);
       SetConsoleOutputCP(_new_cp);
       if (debug) {
         printf("Set ConsoleCP      : %d\n", _new_cp);
         printf("Set ConsoleOutputCP: %d\n", _new_cp);
       }
    } else {
       setlocale(LC_ALL, ""); // set default locale
       if (debug) {
         printf("Set Locale: %s\n", setlocale(LC_ALL, NULL));
       }
    }

    #else
    setlocale(LC_ALL, ""); // set default locale
    if (debug) {
       printf("Set Locale: %s\n", setlocale(LC_ALL, NULL));
    }
    #endif
}

void reset_locale() {
    int debug = 1;
    #ifdef _WIN32
    if (_out_cp == 65001) {
        // UTF-8
        return;
    }

    SetConsoleCP(_cp);
    SetConsoleOutputCP(_out_cp);
    if (debug) {
       printf("Set ConsoleCP      : %d\n", _cp);
       printf("Set ConsoleOutputCP: %d\n", _out_cp);
    }
    #else
    setlocale(LC_ALL, _locale); // reset locale
    if (debug) {
       printf("Set Locale: %s\n", setlocale(LC_ALL, NULL));
    }
    #endif
}

int main(int argc, char* argv[]) {

    init_locale();
    
    /*       
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
    wchar_t* wstr = char2wchar(str);

    printf("MB-c-STR: %s\n", str);
    //wprintf(L"MB-w string: %s\n", str);
    wprintf(L"WC-STR: %ls\n", wstr);
    wprintf(L"WC-LEN: %d\n", wcslen(wstr));
    */    

    /*
    for (int i = 0; i < argc; i++) {
        char* str = argv[i];
        wchar_t* wstr = achar2wchar(str); // WIN32 (ANSII): achar2wchar + wchar2achar - correct only!
        char* str2 = wchar2achar(wstr);

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
    */

    reset_locale();

    int min_arg = 2; // <text> <file>
    if (argc < min_arg + 1) {
        printf("%s: Incorrect argument count\n", argv[0]);
        printUsage();
        return 0;
    }

    // Config
    bool binaryMode = false;
    bool findFirstOnly = false;
    bool ignoreCase = false;

    bool error = false;
    int opt;
    while ((opt = getopt(argc, argv, "bil")) != -1) {
        switch (opt) {
        case 'b':
            binaryMode = true;
            break;
        case 'i':
            ignoreCase = true;
            break;
        case 'l':
            findFirstOnly = true;
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
    char* fileName = lib_strdup_uq(argv[++optind]);

    //printf("input: %s\n", input);
    //printf("file : %s\n", fileName);


    size_t inputSize = strlen(input);
    if (inputSize == 0) {
        //printf("Input is empty\n");
        return 0;
    }

    FindConfig* config = new FindConfig();
    config->binaryMode = binaryMode;
    config->findFirstOnly = findFirstOnly;
    config->ignoreCase = ignoreCase;
    config->printFileName = true; // TODO: add optional

    int wildcardIndex = getWildcardIndex(fileName);

    if (wildcardIndex >= 0) {

        //printf("%s: Find operation doesn't support wildcard\n", argv[0]);
        int pathIndex = getWildcardPathIndex(wildcardIndex, fileName);
        char* dirName = NULL;

        if (pathIndex >= 0) {
            //printf("found '/': %d\n", pathIndex);
            dirName = lib_strndup(fileName, pathIndex + 1);
            fileName = fileName + pathIndex + 1;
        } else {
            dirName = lib_strdup(getCurrentFindPath().c_str());            
        }

        //printf("dir  : %s\n", dirName);
        //printf("file : %s\n", fileName);

        //config->printFileName = true;
        char* pattern = fileName;

        std::vector<std::string> files;

        scandir(dirName, pattern, files, FS_SCANDIR_RECURSIVE);

        for (int i = 0; i < files.size(); i++) {
            //printf("%s\n", files[i].c_str());
            find(files[i].c_str(), input, inputSize, config);
        }

        free(dirName);
        free(config);
        return 0;
    }

    //config->printFileName = false;
    find(fileName, input, inputSize, config);

    free(config);
    return 0;

}
