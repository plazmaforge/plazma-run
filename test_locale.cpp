
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
// #include "strlib.h"
#include "wstrlib.h"
#include "syslib.h"
//#include "sysinfo.h"

void usage() {
    printf("Usage: test-locale <text>\n");
}

void print_test(const char* input) {
    if (!input) {
        return;
    }
    const char* str = input;
    const wchar_t* wstr = achar_wchar(str); // WIN32 (ANSII): achar_wchar + wchar_achar - correct only!
    const char* str2 = wchar_achar(wstr);

    wprintf(L"\n");
    printf("MB-c-STR: %s\n", str);
    //wprintf(L"MB-w-STR: %s\n", str);
    wprintf(L"MB-LEN  : %d\n", strlen(str));
    wprintf(L"WC-STR  : %ls\n", wstr);
    wprintf(L"WC-LEN  : %d\n", wcslen(wstr));
    printf("MB-c-STR: %s\n", str2);
    //wprintf(L"MB-w-STR: %s\n", str2);
    wprintf(L"MB-LEN  : %d\n", strlen(str2));
    wprintf(L"\n");

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }

    init_locale();

    //print_test(argv[1]);

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
    
    const char* str = "\xe4\xbd\xa0\xe5\xa5\xbd";
    wchar_t* wstr = char_wchar(str);

    printf("MB-c-STR: %s\n", str);
    //wprintf(L"MB-w string: %s\n", str);
    wprintf(L"WC-STR  : %ls\n", wstr);
    wprintf(L"WC-LEN  : %d\n", wcslen(wstr));    
        
    for (int i = 0; i < argc; i++) {
        char* str = argv[i];
        wchar_t* wstr = achar_wchar(str); // WIN32 (ANSII): achar_wchar + wchar_achar - correct only!
        char* str2 = wchar_achar(wstr);

        wprintf(L"\n");

        printf("%d: MB-c-STR: %s\n", i, str);
        //wprintf(L"%d: MB-w-STR %s\n", i, str);
        wprintf(L"%d: MB-LEN  : %d\n", i, strlen(str));
        wprintf(L"%d: WC-STR  : %ls\n", i, wstr);
        wprintf(L"%d: WC-LEN  : %d\n", i, wcslen(wstr));
        printf("%d: MB-c-STR: %s\n", i, str2);
        //wprintf(L"%d: MB-w-STR %s\n", i, str2);
        wprintf(L"%d: MB-LEN  : %d\n", i, strlen(str2));
    }
       
    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}