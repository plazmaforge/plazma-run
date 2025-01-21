#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "strlib.h"

void test_strjoin() {

    //// strjoin()

    char* str;

    str = lib_strjoin("abc", "-def", "-xyz", "-qwe");
    printf(">> lib_strjoin(\"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strjoin("abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strjoin(\"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strjoin("abc", "-def");
    printf("\n");
    printf(">> lib_strjoin(\"abc\", \"-def\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strjoin("abc");
    printf("\n");
    printf(">> lib_strjoin(\"abc\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strjoin("abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strjoin(\"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);


}

void test_strcjoin() {

    //// strcjoin()

    char* str;

    str = lib_strcjoin(5, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(5, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strcjoin(4, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(4, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strcjoin(3, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(3, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strcjoin(2, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(2, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strcjoin(1, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(1, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strcjoin(0, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcjoin(0, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    // str = lib_strcjoin(-1, "abc", "-def", "-xyz", NULL, "-qwe");
    // printf("strcjoin(-): %s\n", str);
    // free(str);

}

void test_strcatv() {

    //// strcatv()

    char* str;

    str = lib_strnew(15);
    str = lib_strcatv(str, "abc", "-def", "-xyz", "-qwe");
    printf(">> lib_strcatv(str, \"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(11);
    str = lib_strcatv(str, "abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strcatv(str, \"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", str);
    free(str);

    ////

    str = lib_strnew(15 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcatv(str, "abc", "-def", "-xyz", "-qwe");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcatv(str, \"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(11 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcatv(str, "abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcatv(str, \"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(15 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcatv(str, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcatv(str, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

}

void test_strcpyv() {

    //// strcpyv()

    char* str;

    str = lib_strnew(15);
    str = lib_strcpyv(str, "abc", "-def", "-xyz", "-qwe");
    printf(">> lib_strcpyv(str, \"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(11);
    str = lib_strcpyv(str, "abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strcpyv(str, \"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", str);
    free(str);

    ////

    str = lib_strnew(15 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcpyv(str, "abc", "-def", "-xyz", "-qwe");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcpyv(str, \"abc\", \"-def\", \"-xyz\", \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(11 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcpyv(str, "abc", "-def", "-xyz");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcpyv(str, \"abc\", \"-def\", \"-xyz\")\n");
    printf("<< %s\n", str);
    free(str);

    str = lib_strnew(15 + 4);
    str = lib_strcpy(str, "123-");
    str = lib_strcpyv(str, "abc", "-def", "-xyz", NULL, "-qwe");
    printf("\n");
    printf(">> lib_strcpy(str, \"123-\")\n");
    printf(">> lib_strcpyv(str, \"abc\", \"-def\", \"-xyz\", NULL, \"-qwe\")\n");
    printf("<< %s\n", str);
    free(str);

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

void test_strstr() {

    printf("\n");
    printf(">> strstr(\"abcdef\", \"cde\")\n");
    printf("<< %s\n", strstr("abcdef", "cde"));

    printf(">> strstr(\"abcdef\", \"def\")\n");
    printf("<< %s\n", strstr("abcdef", "def"));

    printf(">> strstr(\"abcdef\", \"xyz\")\n");
    printf("<< %s\n", strstr("abcdef", "xyz"));

    //

    printf(">> lib_strstr(\"abcdef\", \"cde\")\n");
    printf("<< %s\n", lib_strstr("abcdef", "cde"));

    printf(">> lib_strstr(\"abcdef\", \"def\")\n");
    printf("<< %s\n", lib_strstr("abcdef", "def"));

    printf(">> lib_strstr(\"abcdef\", \"f\")\n");
    printf("<< %s\n", lib_strstr("abcdef", "f"));

    printf(">> lib_strstr(\"abcdef\", \"xyz\")\n");
    printf("<< %s\n", lib_strstr("abcdef", "xyz"));

    printf(">> lib_strstr(\"abcdef\", NULL)\n");
    printf("<< %s\n", lib_strstr("abcdef", NULL));

    printf(">> lib_strstr(NULL, \"cde\")\n");
    printf("<< %s\n", lib_strstr(NULL, "cde"));

    printf(">> lib_strstr(NULL, NULL)\n");
    printf("<< %s\n", lib_strstr(NULL, NULL));

}

void test_strnstr() {

    //printf("\n");
    //printf(">> strnstr(\"abcdef\", \"cde\")\n");
    //printf("<< %s\n", strnstr("abcdef", "cde", 6));

    //printf(">> strnstr(\"abcdef\", \"def\", 6)\n");
    //printf("<< %s\n", strnstr("abcdef", "def", 6));

    //printf(">> strnstr(\"abcdef\", \"def\", 3)\n");
    //printf("<< %s\n", strnstr("abcdef", "def", 3));

    //printf(">> strnstr(\"abcdef\", \"bc\", 3)\n");
    //printf("<< %s\n", strnstr("abcdef", "bc", 3));

    //printf(">> strnstr(\"abcdef\", \"def\", 10)\n");
    //printf("<< %s\n", strnstr("abcdef", "def", 10));

    //printf(">> strnstr(\"abcdef\", \"xyz\", 6)\n");
    //printf("<< %s\n", strnstr("abcdef", "xyz", 6));

    //

    printf(">> lib_strnstr(\"abcdef\", \"cde\", 6)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "cde", 6));

    printf(">> lib_strnstr(\"abcdef\", \"def\", 6)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "def", 6));

    printf(">> lib_strnstr(\"abcdef\", \"def\", 3)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "def", 3));

    printf(">> lib_strnstr(\"abcdef\", \"bc\", 3)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "bc", 3));

    printf(">> lib_strnstr(\"abcdef\", \"def\", 10)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "def", 10));

    printf(">> lib_strnstr(\"abcdef\", \"f\", 6)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "f", 6));

    printf(">> lib_strnstr(\"abcdef\", \"xyz\", 6)\n");
    printf("<< %s\n", lib_strnstr("abcdef", "xyz", 6));

    printf(">> lib_strnstr(\"abcdef\", NULL, 6)\n");
    printf("<< %s\n", lib_strnstr("abcdef", NULL, 6));

    printf(">> lib_strnstr(NULL, \"cde\", 6)\n");
    printf("<< %s\n", lib_strnstr(NULL, "cde", 6));

    printf(">> lib_strnstr(NULL, NULL, 6)\n");
    printf("<< %s\n", lib_strnstr(NULL, NULL, 6));

}

void test_strlwr() {

    //// strlwr()

    char* str;

    str = lib_strdup("aBcDef");
    printf("\n");
    printf(">> lib_strlwr(\"aBcDef\")\n");
    printf("<< %s\n", lib_strlwr(str));
    free(str);

    str = lib_strdup("");
    printf(">> lib_strlwr(\"\")\n");
    printf("<< %s\n", lib_strlwr(str));
    free(str);

    printf(">> lib_strlwr(NULL)\n");
    printf("<< %s\n", lib_strlwr(NULL));

}

void test_strupr() {

    //// strupr()

    char* str;

    str = lib_strdup("aBcDef");
    printf("\n");
    printf(">> lib_strupr(\"aBcDef\")\n");
    printf("<< %s\n", lib_strupr(str));
    free(str);

    str = lib_strdup("");
    printf(">> lib_strupr(\"\")\n");
    printf("<< %s\n", lib_strupr(str));
    free(str);

    printf(">> lib_strupr(NULL)\n");
    printf("<< %s\n", lib_strupr(NULL));

}

void test_strrev() {

    //// strrev()

    char* str;

    str = lib_strdup("aBcDef");
    printf("\n");
    printf(">> lib_strrev(\"aBcDef\")\n");
    printf("<< %s\n", lib_strrev(str));
    free(str);

    str = lib_strdup("aB");
    printf(">> lib_strrev(\"aB\")\n");
    printf("<< %s\n", lib_strrev(str));
    free(str);

    str = lib_strdup("a");
    printf(">> lib_strrev(\"a\")\n");
    printf("<< %s\n", lib_strrev(str));
    free(str);

    str = lib_strdup("");
    printf(">> lib_strrev(\"\")\n");
    printf("<< %s\n", lib_strrev(str));
    free(str);

    printf(">> lib_strrev(NULL)\n");
    printf("<< %s\n", lib_strrev(NULL));

}

void test_strtok() {

    //// strtok()

    char* str;
    char* tok;

    str = lib_strdup("This is test");

    printf("\n");
    printf(">> lib_strtok(\"This is test\", \" \")\n");
    tok = lib_strtok(str, " ");
    while (tok) {
        printf("<< %s\n", tok);
        tok = lib_strtok(NULL, " ");
    }
    free(str);

    str = lib_strdup("Name,Unit;Value");
    printf(">> lib_strtok(\"Name,Unit;Value\", \",;\")\n");
    tok = lib_strtok(str, ",;");
    while (tok) {
        printf("<< %s\n", tok);
        tok = lib_strtok(NULL, ",;");
    }
    free(str);

}

void test_strsep() {

    //// strsep()

    char* str;
    char* tok;

    str = lib_strdup("This is test");

    printf("\n");
    printf(">> lib_strsep(\"This is test\", \" \")\n");
    while ((tok = lib_strsep(&str, " "))) {
        printf("<< %s\n", tok);
    } 
    free(str);

    str = lib_strdup("Name,Unit;Value");
    printf(">> lib_strsep(\"Name,Unit;Value\", \",;\")\n");
    while ((tok = lib_strsep(&str, ",;"))) {
        printf("<< %s\n", tok);
    } 
    free(str);

}

int test(const char* name) {
    if (!name) {
        fputs("Test name is empty\n", stderr);
        return 1;
    }

    if (strcmp(name, "strjoin") == 0) {
        test_strjoin();
        return 0;
    }
    if (strcmp(name, "strcatv") == 0) {
        test_strcatv();
        return 0;
    }
    if (strcmp(name, "strcpyv") == 0) {
        test_strcpyv();
        return 0;
    }
    if (strcmp(name, "strlwr") == 0) {
        test_strlwr();
        return 0;
    }
    if (strcmp(name, "strupr") == 0) {
        test_strupr();
        return 0;
    }

    fprintf(stderr, "Test not found: %s\n", name);
    return 1;

}

int main(int argc, char* argv[]) {

    if (argc > 1) {
        return test(argv[1]);
    }

    test_strjoin();
    test_strcjoin();

    test_strcatv();
    test_strcpyv();

    test_strcmp();
    test_strncmp();

    test_stricmp();
    test_strnicmp();

    test_strstr();
    test_strnstr();

    test_strlwr();
    test_strupr();
    test_strrev();

    test_strtok();
    test_strsep();

    return 0;
}