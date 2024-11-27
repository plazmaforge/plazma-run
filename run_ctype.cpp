#include<stdio.h>

int main() {
        
    printf("char        : %2zu bytes\n", sizeof(char));

    printf("short       : %2zu bytes\n", sizeof(short));
    printf("int         : %2zu bytes\n", sizeof(int));
    printf("long        : %2zu bytes\n", sizeof(long));
    printf("long long   : %2zu bytes\n", sizeof(long long));

    printf("float       : %2zu bytes\n", sizeof(float));
    printf("double      : %2zu bytes\n", sizeof(double));
    printf("long double : %2zu bytes\n", sizeof(long double));
    
    return 0;
}
