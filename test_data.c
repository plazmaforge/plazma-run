#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memlib.h"
#include "array.h"
#include "list.h"
#include "map.h"

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

lib_array_t empty_array() {
    lib_array_t a;
    return a;
}

void test_array_ptr() {

    print_test("array_ptr");
    lib_array_t x = empty_array();

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
    fprintf(stdout, "\n");

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
    fprintf(stdout, "\n");

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
    fprintf(stdout, "\n");

    // FREE
    lib_list_free_all(&list);
}

LIB_LIST_TYPE(int)
LIB_LIST_TYPE_PTR(int)

LIB_LIST_STATIC_TYPE(double)

void test_list_int() {
    lib_int_list_t list;
    lib_int_list_init(&list, 10);

    lib_int_list_add(&list, 100);
    lib_int_list_add(&list, 200);
    lib_int_list_add(&list, 300);
    lib_int_list_add(&list, 400);
    //
    lib_int_list_add(&list, 500);
    //
    lib_int_list_insert(&list, 2, 222);
    //
    lib_int_list_remove(&list, 400);

    print_header("int list");
    for (size_t i = 0 ; i < list.size; i++) {
        fprintf(stdout, "list[%lu]: %d\n", i, lib_int_list_get(&list, i));
    }
    fprintf(stdout, "\n");

    lib_int_list_free(&list);
}

////

void test_map_def() {

    print_test("map_def");

    lib_map_t map;
    lib_map_init(&map, 10);
    fprintf(stdout, "map_def: inited\n\n");

    lib_map_add(&map, (void*) 100, "V-100");
    lib_map_add(&map, (void*) 200, "V-200");
    lib_map_add(&map, "300", "V-300");
    lib_map_add(&map, "400", "V-400");
    lib_map_add(&map, NULL, "V-NULL");
    lib_map_add(&map, "500", "V-500");

    print_header("Add entries");
    fprintf(stdout, "map[100]   : %s\n", lib_map_get(&map, (void*) 100));
    fprintf(stdout, "map[200]   : %s\n", lib_map_get(&map, (void*) 200));
    fprintf(stdout, "map[\"300\"] : %s\n", lib_map_get(&map, "300"));
    fprintf(stdout, "map[\"400\"] : %s\n", lib_map_get(&map, "400"));
    fprintf(stdout, "map[NULL]  : %s\n", lib_map_get(&map, NULL));
    fprintf(stdout, "map[\"500\"] : %s\n", lib_map_get(&map, "500"));
    fprintf(stdout, "map[\"777\"] : %s\n", lib_map_get(&map, "777"));
    fprintf(stdout, "\n");

    print_header("Set new values");
    lib_map_add(&map, (void*) 200, "N-200");
    lib_map_add(&map, "300", "N-300");

    fprintf(stdout, "map[100]   : %s\n", lib_map_get(&map, (void*) 100));
    fprintf(stdout, "map[200]   : %s\n", lib_map_get(&map, (void*) 200));
    fprintf(stdout, "map[\"300\"] : %s\n", lib_map_get(&map, "300"));
    fprintf(stdout, "map[\"400\"] : %s\n", lib_map_get(&map, "400"));
    fprintf(stdout, "map[NULL]  : %s\n", lib_map_get(&map, NULL));
    fprintf(stdout, "map[\"500\"] : %s\n", lib_map_get(&map, "500"));
    fprintf(stdout, "map[\"777\"] : %s\n", lib_map_get(&map, "777"));
    fprintf(stdout, "\n");

    print_header("Remove entities: [200, \"500\"]");
    lib_map_remove(&map, (void*) 200);
    lib_map_remove(&map, "500");

    fprintf(stdout, "map[100]   : %s\n", lib_map_get(&map, (void*) 100));
    fprintf(stdout, "map[200]   : %s\n", lib_map_get(&map, (void*) 200));
    fprintf(stdout, "map[\"300\"] : %s\n", lib_map_get(&map, "300"));
    fprintf(stdout, "map[\"400\"] : %s\n", lib_map_get(&map, "400"));
    fprintf(stdout, "map[NULL]  : %s\n", lib_map_get(&map, NULL));
    fprintf(stdout, "map[\"500\"] : %s\n", lib_map_get(&map, "500"));
    fprintf(stdout, "map[\"777\"] : %s\n", lib_map_get(&map, "777"));
    fprintf(stdout, "\n");

    fprintf(stdout, "has[100]   : %d\n", lib_map_contains(&map, (void*) 100));
    fprintf(stdout, "has[200]   : %d\n", lib_map_contains(&map, (void*) 200));
    fprintf(stdout, "has[\"300\"] : %d\n", lib_map_contains(&map, "300"));
    fprintf(stdout, "has[\"400\"] : %d\n", lib_map_contains(&map, "400"));
    fprintf(stdout, "has[NULL]  : %d\n", lib_map_contains(&map, NULL));
    fprintf(stdout, "has[\"500\"] : %d\n", lib_map_contains(&map, "500"));
    fprintf(stdout, "has[\"777\"] : %d\n", lib_map_contains(&map, "777"));
    fprintf(stdout, "\n");

    lib_map_free(&map);

}

LIB_MAP_ENTRY_STRUCT(test_entry_t, int, char*)
LIB_MAP_STRUCT(test_map_t, test_entry_t)

LIB_MAP_CLASS_STRUCT(my_map_t, my_entry_t, int, char*)
LIB_MAP_CLASS_PRIVATE(lib_my_map, my_map_t, my_entry_t, int, char*)
LIB_MAP_CLASS_INIT(lib_my_map, my_map_t, my_entry_t)
LIB_MAP_CLASS_FREE(lib_my_map, my_map_t, my_entry_t)

LIB_MAP_CLASS(ipc_map, ipc_map_t, ipc_entry_t, int, char*)

LIB_MAP_TYPE(int, float)

void test_map_type() {

    print_test("map_type");

    // test map
    test_entry_t test_entry;
    test_entry.key = 100;
    test_entry.value = "V-100";

    test_map_t test_map;
    test_map.size = 10;
    test_map.capacity = 100;

    // my map
    my_entry_t my_entry;
    my_entry.key = 100;
    my_entry.value = "V-100";

    my_map_t my_map;
    lib_my_map_init(&my_map, 20);

    fprintf(stdout, "my_map.size    : %lu\n", my_map.size);
    fprintf(stdout, "my_map.capacity: %lu\n", my_map.capacity);
    fprintf(stdout, "my_map.entries : %p\n", my_map.entries);
    fprintf(stdout, "\n");

    lib_my_map_free(&my_map);

    print_header("my_map_free");

    fprintf(stdout, "my_map.entries : %p\n", my_map.entries);
    fprintf(stdout, "\n");

    ipc_map_t map;
    ipc_map_init(&map, 10);
    ipc_map_add(&map, 100, "VAL-100");
    ipc_map_add(&map, 200, "VAL-200");
    ipc_map_add(&map, 300, "VAL-300");
    ipc_map_add(&map, 400, "VAL-400");
    ipc_map_add(&map, 500, "VAL-500");

    print_header("Add entities");
    fprintf(stdout, "map[100]: %s\n", ipc_map_get(&map, 100));
    fprintf(stdout, "map[200]: %s\n", ipc_map_get(&map, 200));
    fprintf(stdout, "map[300]: %s\n", ipc_map_get(&map, 300));
    fprintf(stdout, "map[400]: %s\n", ipc_map_get(&map, 400));
    fprintf(stdout, "map[500]: %s\n", ipc_map_get(&map, 500));
    fprintf(stdout, "map[777]: %s\n", ipc_map_get(&map, 777));
    fprintf(stdout, "map.size: %lu\n", map.size);
    fprintf(stdout, "\n");

    ipc_map_remove(&map, 300);
    ipc_map_remove(&map, 500);

    print_header("Remove entities");

    fprintf(stdout, "map[100]: %s\n", ipc_map_get(&map, 100));
    fprintf(stdout, "map[200]: %s\n", ipc_map_get(&map, 200));
    fprintf(stdout, "map[300]: %s\n", ipc_map_get(&map, 300));
    fprintf(stdout, "map[400]: %s\n", ipc_map_get(&map, 400));
    fprintf(stdout, "map[500]: %s\n", ipc_map_get(&map, 500));
    fprintf(stdout, "map[777]: %s\n", ipc_map_get(&map, 777));
    fprintf(stdout, "map.size: %lu\n", map.size);
    fprintf(stdout, "\n");

}

void test_map_int_float() {

    print_test("map_int_float");

    lib_int_float_map_t map;

    lib_int_float_map_init(&map, 10);

    size_t count = 20;
    for (size_t i = 0; i < count; i++) {
        size_t key = (i + 1);
        float value = key * 100;
        lib_int_float_map_add(&map, key, value);
    }

    for (size_t i = 0; i < count; i++) {
        size_t key = (i + 1);
        float value = key * 100;
        fprintf(stdout, "map[%lu]: %f\n", key, lib_int_float_map_get(&map, key));
    }
    fprintf(stdout, "\n");

    fprintf(stdout, "map[%d]: %f\n", -100, lib_int_float_map_get(&map, -100));
    fprintf(stdout, "map[%d]: %f\n", -200, lib_int_float_map_get(&map, -200));
    fprintf(stdout, "map[%d]: %f\n", 7777, lib_int_float_map_get(&map, 7777));
    fprintf(stdout, "\n");

}

////

void test_list() {
    test_list_ptr();
    //test_list_val();

    test_list_int();
}

void test_map() {
    test_map_def();
    test_map_type();
    test_map_int_float();
}

void test_data() {
    test_array();
    test_list();
    test_map();
}

//#define assert_array(a) \
//     (sizeof(char[1 - 2 * __builtin_types_compatible_p(typeof(a), typeof(&(a)[0]))]) - 1)

int main(int argc, char* argv[]) {
    test_data();    
    return 0;
}