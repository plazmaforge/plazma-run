#include<stdio.h>

int main() {
        
    printf("char        : %2lu bytes\n", sizeof(char));

    printf("short       : %2lu bytes\n", sizeof(short));
    printf("int         : %2lu bytes\n", sizeof(int));
    printf("long        : %2lu bytes\n", sizeof(long));
    printf("long long   : %2lu bytes\n", sizeof(long long));

    printf("float       : %2lu bytes\n", sizeof(float));
    printf("double      : %2lu bytes\n", sizeof(double));
    printf("long double : %2lu bytes\n", sizeof(long double));
    
    return 0;
}
