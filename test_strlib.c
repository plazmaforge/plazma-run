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

void test_strcmp() {

    char* a = "abc";
    char* b = "xyz";

    printf("\n");
    printf(">> strcmp(a, b)\n");
    printf("<< %d\n", strcmp(a, b));

    printf(">> strcmp(b, a)\n");
    printf("<< %d\n", strcmp(b, a));

    printf(">> strcmp(\"ab\", \"abc\")\n");
    printf("<< %d\n", strcmp("ab", "abc"));

    //

    printf(">> lib_strcmp(a, b)\n");
    printf("<< %d\n", lib_strcmp(a, b));

    printf(">> lib_strcmp(b, a)\n");
    printf("<< %d\n", lib_strcmp(b, a));

    printf(">> lib_strcmp(a, a)\n");
    printf("<< %d\n", lib_strcmp(a, a));

    printf(">> lib_strcmp(\"ab\", \"abc\")\n");
    printf("<< %d\n", lib_strcmp("ab", "abc"));

    printf(">> lib_strcmp(\"abc\", \"abc\")\n");
    printf("<< %d\n", lib_strcmp("abc", "abc"));

    printf(">> lib_strcmp(NULL, b)\n");
    printf("<< %d\n", lib_strcmp(NULL, b));

    printf(">> lib_strcmp(a, NULL)\n");
    printf("<< %d\n", lib_strcmp(a, NULL));

    printf(">> lib_strcmp(NULL, NULL)\n");
    printf("<< %d\n", lib_strcmp(NULL, NULL));

}

void test_strncmp() {

    char* a = "abc";
    char* b = "xyz";

    printf("\n");
    printf(">> strncmp(a, b, 3)\n");
    printf("<< %d\n", strncmp(a, b, 3));

    printf(">> strcmp(b, a, 3)\n");
    printf("<< %d\n", strncmp(b, a, 3));

    printf(">> strncmp(\"ab\", \"abc\", 2)\n");
    printf("<< %d\n", strncmp("ab", "abc", 2));

    printf(">> strncmp(\"ab\", \"abc\", 3)\n");
    printf("<< %d\n", strncmp("ab", "abc", 3));

    printf(">> lib_strncmp(a, b, 3)\n");
    printf("<< %d\n", lib_strncmp(a, b, 3));

    printf(">> lib_strncmp(b, a, 3)\n");
    printf("<< %d\n", lib_strncmp(b, a, 3));

    printf(">> lib_strncmp(a, a, 3)\n");
    printf("<< %d\n", lib_strncmp(a, a, 3));

    printf(">> lib_strncmp(\"abc\", \"abc\")\n");
    printf("<< %d\n", lib_strncmp("abc", "abc", 3));

    printf(">> lib_strncmp(\"ab\", \"abc\", 2)\n");
    printf("<< %d\n", lib_strncmp("ab", "abc", 2));

    printf(">> lib_strncmp(\"ab\", \"abc\", 3)\n");
    printf("<< %d\n", lib_strncmp("ab", "abc", 3));

    printf(">> lib_strncmp(NULL, b, 3)\n");
    printf("<< %d\n", lib_strncmp(NULL, b, 3));

    printf(">> lib_strncmp(a, NULL, 3)\n");
    printf("<< %d\n", lib_strncmp(a, NULL, 3));

    printf(">> lib_strncmp(NULL, NULL, 3)\n");
    printf("<< %d\n", lib_strncmp(NULL, NULL, 3));

}

void test_stricmp() {

    printf("\n");
    printf(">> lib_stricmp(\"abc\", \"abc\")\n");
    printf("<< %d\n", lib_stricmp("abc", "abc"));

    printf(">> lib_stricmp(\"aBc\", \"Abc\")\n");
    printf("<< %d\n", lib_stricmp("aBc", "Abc"));

    printf(">> lib_stricmp(\"aBc\", \"xYz\")\n");
    printf("<< %d\n", lib_stricmp("aBc", "xYz"));

    printf(">> lib_stricmp(\"xYz\", \"aBc\" )\n");
    printf("<< %d\n", lib_stricmp("xYz", "aBc"));

    printf(">> lib_stricmp(NULL, \"Abc\")\n");
    printf("<< %d\n", lib_stricmp(NULL, "Abc"));

    printf(">> lib_stricmp(\"xYz\", NULL)\n");
    printf("<< %d\n", lib_stricmp("xYz", NULL));

    printf(">> lib_stricmp(NULL, NULL)\n");
    printf("<< %d\n", lib_stricmp(NULL, NULL));

    //

    printf(">> lib_stricmp(\"ab\", \"abc\")\n");
    printf("<< %d\n", lib_stricmp("ab", "abc"));

    printf(">> lib_stricmp(\"aB\", \"abC\")\n");
    printf("<< %d\n", lib_stricmp("aB", "abC"));

}

void test_strnicmp() {

    printf("\n");
    printf(">> lib_strnicmp(\"abc\", \"abc\", 3)\n");
    printf("<< %d\n", lib_strnicmp("abc", "abc", 3));

    printf(">> lib_strnicmp(\"aBc\", \"Abc\", 3)\n");
    printf("<< %d\n", lib_strnicmp("aBc", "Abc", 3));

    printf(">> lib_strnicmp(\"aBc\", \"xYz\", 3)\n");
    printf("<< %d\n", lib_strnicmp("aBc", "xYz", 3));

    printf(">> lib_strnicmp(\"xYz\", \"aBc\", 3)\n");
    printf("<< %d\n", lib_strnicmp("xYz", "aBc", 3));

    printf(">> lib_strnicmp(NULL, \"Abc\", 3)\n");
    printf("<< %d\n", lib_strnicmp(NULL, "Abc", 3));

    printf(">> lib_strnicmp(\"xYz\", NULL, 3)\n");
    printf("<< %d\n", lib_strnicmp("xYz", NULL, 3));

    printf(">> lib_strnicmp(NULL, NULL, 3)\n");
    printf("<< %d\n", lib_strnicmp(NULL, NULL, 3));

    //

    printf(">> lib_strnicmp(\"ab\", \"abc\")\n");
    printf("<< %d\n", lib_strnicmp("ab", "abc", 2));

    printf(">> lib_strnicmp(\"aB\", \"abC\")\n");
    printf("<< %d\n", lib_strnicmp("aB", "abC", 3));

}

int main(int argc, char* argv[]) {

    test_strapp();

    test_strcmp();
    test_strncmp();

    test_stricmp();
    test_strnicmp();

    return 0;
}