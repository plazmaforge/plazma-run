#include "memlib.h"
#include "clt.h"
#include "stack.h"

static int _stack_init(lib_stack_t* stack, size_t capacity, size_t element_size);

int lib_stack_init(lib_stack_t* stack, size_t capacity, size_t element_size) {
    return _stack_init(stack, capacity, element_size);
}

size_t lib_stack_size(lib_stack_t* stack) {
    return !stack ? 0 : stack->size;
}

size_t lib_stack_capacity(lib_stack_t* stack) {
    return !stack ? 0 : stack->capacity;
}

size_t lib_stack_element_size(lib_stack_t* stack) {
    return !stack ? 0 : stack->element_size;
}

static int _stack_init(lib_stack_t* stack, size_t capacity, size_t element_size) {
    if (!stack || element_size == 0) {
        return -1;
    }
    stack->size = 0;
    stack->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    stack->element_size = element_size;
    stack->data = lib_mallocz(capacity * element_size);
    if (!stack->data) {
        return -1;
    }
    return 0;
}
