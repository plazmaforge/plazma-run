
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
// #include "strlib.h"
#include "wstrlib.h"
#include "syslib.h"
//#include "sysinfo.h"

void usage() {
    printf("Usage: run-echo <text>\n");
}

void print_input(const char* input) {
    if (!input) {
        return;
    }
    printf("%s\n", input);
}

void print_test(const char* input) {
    if (!input) {
        return;
    }
    const char* str = input;
    const wchar_t* wstr = achar_wchar(str); // WIN32 (ANSII): achar_wchar + wchar_achar - correct only!
    const char* str2 = wchar_achar(wstr);

    wprintf(L"\n");
    printf("MB-c-STR %s\n", str);
    //wprintf(L"MB-w-STR %s\n", str);
    wprintf(L"MB-LEN %d\n", strlen(str));
    wprintf(L"WC-STR %ls\n", wstr);
    wprintf(L"WC-LEN %d\n", wcslen(wstr));
    printf("MB-c-STR %s\n", str2);
    //wprintf(L"MB-w-STR %s\n", str2);
    wprintf(L"MB-LEN %d\n", strlen(str2));
    wprintf(L"\n");

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        usage();
        return 0;
    }

    init_locale();

    print_input(argv[1]);

    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}