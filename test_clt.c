#include <stdlib.h>
#include <stdio.h>

#include "array.h"


void test_array_cast() {

    fprintf(stderr, "Test array_cast...\n");

    lib_array_t array;
    lib_array_init_cast(&array, 10);
    //lib_array_set_mem_type(&array, LIB_CLT_MEM_TYPE_CAST);
    fprintf(stderr, "Inited array_cast\n");

    lib_array_set(&array, 0, (void*) 100);
    lib_array_set(&array, 1, (void*) 200);
    lib_array_set(&array, 2, "300-A");
    lib_array_set(&array, 3, "400-A");

    fprintf(stderr, "array[0]: %d\n", (int) lib_array_get(&array, 0));
    fprintf(stderr, "array[1]: %d\n", (int) lib_array_get(&array, 1));
    fprintf(stderr, "array[2]: %s\n", lib_array_get(&array, 2));
    fprintf(stderr, "array[3]: %s\n", lib_array_get(&array, 3));

}

void test_array_copy() {

    fprintf(stderr, "Test array_copy...\n");

    lib_array_t array;
    lib_array_init(&array, 10, sizeof(int));
    fprintf(stderr, "Inited array_copy\n");

    int x0 = 100;
    int x1 = 200;
    int x2 = 300;
    int x3 = 400;

    lib_array_set(&array, 0, &x0);
    lib_array_set(&array, 1, &x1);
    lib_array_set(&array, 2, &x2);
    lib_array_set(&array, 3, &x3);

    fprintf(stderr, "array[0]: %d\n", *(int*) lib_array_get(&array, 0));
    fprintf(stderr, "array[1]: %d\n", *(int*) lib_array_get(&array, 1));
    fprintf(stderr, "array[2]: %d\n", *(int*) lib_array_get(&array, 2));
    fprintf(stderr, "array[3]: %d\n", *(int*) lib_array_get(&array, 3));

}

void test_clt() {
    test_array_cast();
    test_array_copy();

}

int main(int argc, char* argv[]) {
    test_clt();
    return 0;
}