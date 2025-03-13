#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "array.h"
#include "list.h"


//// ARRAY ////

void test_array_ptr() {

    fprintf(stderr, "Test array_ptr...\n");

    lib_array_t array;
    lib_array_init_ptr(&array, 10);
    fprintf(stderr, "array_ptr: inited\n");

    lib_array_set(&array, 0, (void*) 100);
    lib_array_set(&array, 1, (void*) 200);
    lib_array_set(&array, 2, "300-A");
    //lib_array_set(&array, 3, "400-A");

    fprintf(stderr, "array[0]: %d\n", (int) lib_array_get(&array, 0));
    fprintf(stderr, "array[1]: %d\n", (int) lib_array_get(&array, 1));
    fprintf(stderr, "array[2]: %s\n", lib_array_get(&array, 2));
    fprintf(stderr, "array[3]: %s\n", lib_array_get(&array, 3));
    fprintf(stderr, "\n");

    lib_array_free(&array);

}

void test_array_val() {

    fprintf(stderr, "Test array_val...\n");

    lib_array_t array;
    lib_array_init_val(&array, 10, sizeof(int));
    fprintf(stderr, "array_val: inited\n");

    int x0 = 100;
    int x1 = 200;
    int x2 = 300;
    int x3 = 400;

    lib_array_set(&array, 0, &x0);
    lib_array_set(&array, 1, &x1);
    lib_array_set(&array, 2, &x2);
    //lib_array_set(&array, 3, &x3);

    fprintf(stderr, "array[0]: %d\n", *(int*) lib_array_get(&array, 0));
    fprintf(stderr, "array[1]: %d\n", *(int*) lib_array_get(&array, 1));
    fprintf(stderr, "array[2]: %d\n", *(int*) lib_array_get(&array, 2));
    fprintf(stderr, "array[3]: %d\n", *(int*) lib_array_get(&array, 3));

    int* px100 = (int*) lib_array_get(&array, 100);
    if (px100) {
        fprintf(stderr, "array[100]: %d\n", *px100);
    } else {
        fprintf(stderr, "array[100]: (null)\n");
    }    
    fprintf(stderr, "\n");

    lib_array_free(&array);
}

void test_array() {
    test_array_ptr();
    test_array_val();
}

//// LIST ////

void test_list_ptr() {

    fprintf(stderr, "Test list_ptr...\n");

    lib_list_t list;
    lib_list_init_ptr(&list, 10);
    fprintf(stderr, "list_ptr: inited\n");

    lib_list_add(&list, (void*) 100);
    lib_list_add(&list, (void*) 200);
    lib_list_add(&list, "300-A");
    lib_list_add(&list, "400-A");
    lib_list_add(&list, NULL);
    lib_list_add(&list, "500-A");

    fprintf(stderr, "list[0]: %d\n", (int) lib_list_get(&list, 0));
    fprintf(stderr, "list[1]: %d\n", (int) lib_list_get(&list, 1));
    fprintf(stderr, "list[2]: %s\n", lib_list_get(&list, 2));
    fprintf(stderr, "list[3]: %s\n", lib_list_get(&list, 3));
    fprintf(stderr, "list[4]: %s\n", lib_list_get(&list, 4));
    fprintf(stderr, "list[5]: %s\n", lib_list_get(&list, 5));
    fprintf(stderr, "list[6]: %s\n", lib_list_get(&list, 6));
    fprintf(stderr, "\n");

    lib_list_free(&list);

    ////

    lib_list_init_ptr(&list, 10);
    char buf[6];

    for (int i = 0; i < 15; i++) {
        if (i == 13) {
            lib_list_add(&list, NULL);
        } else {
            sprintf(buf, "%02d-X", i + 1);
            lib_list_add(&list, strdup(buf));
        }
        fprintf(stderr, "list[%02d]: %s\n", i, lib_list_get(&list, i));
    }

    lib_list_free_all(&list);

}

void test_list() {
    test_list_ptr();
    //test_list_val();
}

void test_clt() {
    test_array();
    test_list();
}

int main(int argc, char* argv[]) {
    test_clt();
    return 0;
}