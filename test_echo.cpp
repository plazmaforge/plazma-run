
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "getopt.h"
#include "strlib.h"
#include "wstrlib.h"
#include "syslib.h"
//#include "sysinfo.h"

void usage() {
    printf("Usage: test-echo <text>\n");
}

void print_test(const char* input) {
    if (!input) {
        return;
    }
    const char* str = input;
    const wchar_t* wstr = lib_acs_to_wcs(str); // WIN32 (ANSII): acs_to_wcs() + wcs_to_acs(): correct only!
    const char* str2 = lib_wcs_to_acs(wstr);

    char* tmp = lib_straddv("abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddv   : %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(5, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(5): %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(4, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(4): %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(3, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(3): %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(2, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(2): %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(1, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(1): %s\n", tmp);
    free(tmp);

    tmp = lib_straddn(0, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("straddn(0): %s\n", tmp);
    free(tmp);

    // tmp = lib_straddn(-1, "abc", "-def", "-xyz", NULL, "-qwe");
    // printf("straddn(-): %s\n", tmp);
    // free(tmp);

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

    lib_sys_locale_init();

    print_test(argv[1]);

    lib_sys_locale_restore(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}