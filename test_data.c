#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memlib.h"
#include "array.h"
#include "list.h"

void print_test(const char* str) {
    fprintf(stdout, "============================\n");
    fprintf(stdout, "TEST %s\n", str);
    fprintf(stdout, "============================\n");
}

void print_header(const char* str) {
    fprintf(stdout, ">> %s\n", str);
    fprintf(stdout, "============================\n");
}

//// ARRAY ////

void test_array_ptr() {

    print_test("array_ptr");

    lib_array_t array;
    lib_array_init_ptr(&array, 10);
    fprintf(stdout, "array_ptr: inited\n\n");

    lib_array_set(&array, 0, (void*) 100);
    lib_array_set(&array, 1, (void*) 200);
    lib_array_set(&array, 2, "300-A");
    //lib_array_set(&array, 3, "400-A");

    print_header("Set values");
    fprintf(stdout, "array[0]: %d\n", (int) lib_array_get(&array, 0));
    fprintf(stdout, "array[1]: %d\n", (int) lib_array_get(&array, 1));
    fprintf(stdout, "array[2]: %s\n", lib_array_get(&array, 2));
    fprintf(stdout, "array[3]: %s\n", lib_array_get(&array, 3));
    fprintf(stdout, "\n");

    // FREE
    lib_array_free(&array);
}

void test_array_val() {

    print_test("array_val");

    lib_array_t array;
    lib_array_init_val(&array, 10, sizeof(int));
    fprintf(stdout, "array_val: inited\n\n");

    int x0 = 100;
    int x1 = 200;
    int x2 = 300;
    int x3 = 400;

    lib_array_set(&array, 0, &x0);
    lib_array_set(&array, 1, &x1);
    lib_array_set(&array, 2, &x2);
    //lib_array_set(&array, 3, &x3);

    print_header("Set values");
    fprintf(stdout, "array[0]: %d\n", *(int*) lib_array_get(&array, 0));
    fprintf(stdout, "array[1]: %d\n", *(int*) lib_array_get(&array, 1));
    fprintf(stdout, "array[2]: %d\n", *(int*) lib_array_get(&array, 2));
    fprintf(stdout, "array[3]: %d\n", *(int*) lib_array_get(&array, 3));

    int* px100 = (int*) lib_array_get(&array, 100);
    if (px100) {
        fprintf(stdout, "array[100]: %d\n", *px100);
    } else {
        fprintf(stdout, "array[100]: (null)\n");
    }    
    fprintf(stdout, "\n");

    lib_array_free(&array);
}

//LIB_ARRAY_CLASS_STRUCT(test_array_t, int)
//LIB_ARRAY_CLASS_INIT(test_array, test_array_t, int)
//LIB_ARRAY_CLASS_GET(test_array, test_array_t, int)
//LIB_ARRAY_CLASS_SET(test_array, test_array_t, int)

//LIB_ARRAY_CLASS(test_array2, test_array2_t, int)

LIB_ARRAY_TYPE(int)
LIB_ARRAY_TYPE_PTR(int)

void test_array_int() {
    lib_int_array_t array;
    lib_int_array_init(&array, 10);

    lib_int_array_set(&array, 0, 100);
    lib_int_array_set(&array, 1, 200);
    lib_int_array_set(&array, 2, 300);
    lib_int_array_set(&array, 3, 400);
    //
    lib_int_array_set(&array, 6, 500);

    print_header("int array");
    for (size_t i = 0 ; i < array.size; i++) {
        fprintf(stdout, "array[%lu]: %d\n", i, lib_int_array_get(&array, i));
    }

    lib_int_array_free(&array);
}

LIB_ARRAY_TYPE(float)

void test_array_float() {
    lib_float_array_t array;
    lib_float_array_init(&array, 10);

    lib_float_array_set(&array, 0, 100.10);
    lib_float_array_set(&array, 1, 200.20);
    lib_float_array_set(&array, 2, 300.30);
    lib_float_array_set(&array, 3, 400.40);
    //
    lib_float_array_set(&array, 6, 500.50);

    print_header("float array");
    for (size_t i = 0 ; i < array.size; i++) {
        fprintf(stdout, "array[%lu]: %f\n", i, lib_float_array_get(&array, i));
    }

    lib_float_array_free(&array);
}

void test_array() {
    test_array_ptr();
    test_array_val();

    test_array_int();
    test_array_float();
}

//// LIST ////

void print_list(lib_list_t* list) {
    if (!list || !list->data || list->size == 0) {
        return;
    }
    size_t size = list->size;
    for (size_t i = 0; i < size; i++) {
        fprintf(stdout, "list[%02lu]: %s\n", i, lib_list_get(list, i));
    }
    fprintf(stdout, "\n");
}

void test_list_ptr() {

    print_test("list_ptr");

    lib_list_t list;
    lib_list_init_ptr(&list, 10);
    fprintf(stdout, "list_ptr: inited\n\n");

    lib_list_add(&list, (void*) 100);
    lib_list_add(&list, (void*) 200);
    lib_list_add(&list, "300-A");
    lib_list_add(&list, "400-A");
    lib_list_add(&list, NULL);
    lib_list_add(&list, "500-A");

    print_header("Add values");
    fprintf(stdout, "list[0]: %d\n", (int) lib_list_get(&list, 0));
    fprintf(stdout, "list[1]: %d\n", (int) lib_list_get(&list, 1));
    fprintf(stdout, "list[2]: %s\n", lib_list_get(&list, 2));
    fprintf(stdout, "list[3]: %s\n", lib_list_get(&list, 3));
    fprintf(stdout, "list[4]: %s\n", lib_list_get(&list, 4));
    fprintf(stdout, "list[5]: %s\n", lib_list_get(&list, 5));
    fprintf(stdout, "list[6]: %s\n", lib_list_get(&list, 6));
    fprintf(stdout, "\n");

    lib_list_free(&list);

    // REINIT
    lib_list_init_ptr(&list, 10);
    char buf[6];
    int count = 15;

    // ADD STRING VALUES
    for (int i = 0; i < count; i++) {
        if (i == 13) {
            lib_list_add(&list, NULL);
        } else {
            sprintf(buf, "%02d-X", i + 1);
            lib_list_add(&list, strdup(buf));
        }
    }

    print_header("Add string values");
    print_list(&list);

    // INSERT
    lib_list_insert(&list, 5, strdup("X--X"));
    print_header("Insert");
    print_list(&list);

    // REMOVE BY INDEX
    lib_list_remove_index(&list, 14);
    print_header("Remove by index");
    print_list(&list);

    // ADD PTR VALUE
    char* str1 = strdup("str-1");
    lib_list_add(&list, str1);
    lib_list_add(&list, strdup("str-2"));
    lib_list_add(&list, strdup("str-3"));

    print_header("Add ptr values");
    print_list(&list);

    // REMOVE BY PTR VALUE
    lib_list_remove(&list, str1);
    //lib_list_remove(&list, strdup("str-1"));
    print_header("Remove by ptr value");
    print_list(&list);

    // ITERATOR
    lib_iterator_t iterator;
    lib_list_iterator_init(&list, &iterator);

    print_header("Iterator");
    while(lib_list_iterator_has_next(&list, &iterator)) {
        fprintf(stdout, ">> %s\n", lib_list_iterator_next(&list, &iterator));
    }

    // FREE
    lib_list_free_all(&list);
}

void test_list() {
    test_list_ptr();
    //test_list_val();
}

void test_data() {
    test_array();
    test_list();
}

int main(int argc, char* argv[]) {
    test_data();
    return 0;
}