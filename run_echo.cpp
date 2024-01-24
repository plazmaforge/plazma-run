
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
// #include "strlib.h"
#include "wstrlib.h"
#include "syslib.h"
//#include "sysinfo.h"

void printUsage() {
    printf("Usage: run-echo <text>\n");
}

void printInput(const char* input) {
    if (!input) {
        return;
    }
    printf("%s\n", input);
}

void printTest(const char* input) {
    if (!input) {
        return;
    }
    const char* str = input;
    const wchar_t* wstr = achar2wchar(str); // WIN32 (ANSII): achar2wchar + wchar2achar - correct only!
    const char* str2 = wchar2achar(wstr);

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
        printUsage();
        return 0;
    }

    init_locale();

    printInput(argv[1]);

    restore_locale(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}