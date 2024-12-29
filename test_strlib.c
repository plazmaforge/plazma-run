#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strlib.h"

void test_strapp() {

    //// strappv()

    char* tmp = lib_strappv("abc", "-def", "-xyz", "-qwe");
    printf(">> lib_strappv(\"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappv("abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strappv(\"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappv("abc", "-def");
    printf("\n");
    printf(">> lib_strappv(\"abc\", \"-def\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappv("abc");
    printf("\n");
    printf(">> lib_strappv(\"abc\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappv("abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappv(\"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    //// strappn()

    tmp = lib_strappn(5, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(5, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappn(4, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(4, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappn(3, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(3, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappn(2, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(2, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappn(1, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(1, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    tmp = lib_strappn(0, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strappn(0, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", tmp);
    free(tmp);

    // tmp = lib_strappn(-1, "abc", "-def", "-xyz", NULL, "-qwe");
    // printf("strappn(-): %s\n", tmp);
    // free(tmp);

}

int main(int argc, char* argv[]) {

    test_strapp();

    return 0;
}