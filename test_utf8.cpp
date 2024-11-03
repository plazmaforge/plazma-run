
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopt.h"
//#include "strlib.h"
//#include "wstrlib.h"
#include "syslib.h"
#include "utf8lib.h"

void usage() {
    printf("Usage: test-utf8 <text>\n");
}

const char* to_bool_str(bool x) {
    return (x ? "true" : "false");
}

void print_str(const char* str, int i) {
    int len = strlen(str);
    int cpl = lib_utf8_get_str_len(str);
    bool is_asc = lib_utf8_is_ascii(str);
    bool is_utf8 = lib_utf8_is_utf8(str); 
    int bom = lib_utf8_get_bom_n(str, len);

    //printf("%s", str);

    printf("\n");
    printf("str-%i-val  = '%s'\n", i, str);
    printf("str-%i-len  = %i\n", i, len);
    printf("str-%i-cpl  = %i\n", i, cpl);
    printf("str-%i-asc  = %s\n", i, to_bool_str(is_asc));
    printf("str-%i-utf8 = %s\n", i, to_bool_str(is_utf8));
    printf("str-%i-bom  = %s\n", i, lib_utf8_to_bom_str(bom));
}

void print_test(const char* input) {
    if (input) {
        print_str(input, 0);

        char* input_u = strdup(input);
        lib_utf8_to_upper(input_u);
        print_str(input_u, 0);

        char* input_l = strdup(input);
        lib_utf8_to_lower(input_l);
        print_str(input_l, 0);

    }

    //const char* str = input;
    //const wchar_t* wstr = lib_acs_to_wcs(str); // WIN32 (ANSII): acs_to_wcs() + wcs_to_acs(): correct only!
    //const char* str2 = lib_wcs_to_acs(wstr);

    const char* str1 = "hello";
    print_str(str1, 1);

    const char* str2 = "\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82";
    print_str(str2, 2);

    const char* str3 = "1\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82";
    print_str(str3, 3);

    char* str4 = strdup(str2);

    lib_utf8_to_upper(str4);
    print_str(str4, 4);
    free(str4);

    const char* str5 = "\xEF\xBB\xBF\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82";
    print_str(str5, 5);

}

int main(int argc, char* argv[]) {
    //if (argc < 2) {
    //    usage();
    //    return 0;
    //}

    lib_sys_locale_init();

    char* input = argc < 2 ? NULL : argv[1];

    print_test(input);

    int val = 0;

    //for (int i = 0; i < 1000000000; i++) {
    //for (int i = 0; i < 10000000; i++) {    
        for (int u = 0x00; u <= 0xFF; u++) {
            
            char c = (char) u;
            int i1 = 0;
            int i2 = 0;
            int i3 = 0;

            //printf("u = %i, c = %c\n", u, c);
            i1 = lib_utf8_get_byte_sequence_len_array(c);
            i2 = lib_utf8_get_byte_sequence_len_strong(c);
            i3 = lib_utf8_get_byte_sequence_len_range(c);
            if (i1 != i2 || i1 != i3 || i2 != i3) {
                printf("u = %i, len1 = %i, len2 = %i, len3 = %i\n", u, i1, i2, i3);
            }
            val += i1;

            //val += lib_utf8_get_byte_sequence_len_array(c);
            //val += lib_utf8_get_byte_sequence_len_strong(c);
            //val += lib_utf8_get_byte_sequence_len_range(c);
            //val += c;
        }
    //}

    // array: val = -1734967296
    //        val = -1734967296
    //        val = -1734967296
    //        val = -1734967296

    // strong val = -1744967296
    //        val = -1744967296

    //        val = -1264967296
    //        val = -1264967296

    printf("val = %i\n", val);


    lib_sys_locale_restore(); // Important for WIN32: The locale was changed for the terminal
    return 0;
}