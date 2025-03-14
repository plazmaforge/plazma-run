#include "memlib.h"
#include "data.h"
#include "stack.h"

static int _stack_init(lib_stack_t* stack, size_t capacity, size_t value_size);

int lib_stack_init(lib_stack_t* stack, size_t capacity, size_t value_size) {
    return _stack_init(stack, capacity, value_size);
}

size_t lib_stack_size(lib_stack_t* stack) {
    return !stack ? 0 : stack->size;
}

size_t lib_stack_capacity(lib_stack_t* stack) {
    return !stack ? 0 : stack->capacity;
}

size_t lib_stack_value_size(lib_stack_t* stack) {
    return !stack ? 0 : stack->value_size;
}

static int _stack_init(lib_stack_t* stack, size_t capacity, size_t value_size) {
    if (!stack || value_size == 0) {
        return -1;
    }
    stack->size = 0;
    stack->capacity = LIB_MAX(LIB_DATA_CAPACITY_MIN, capacity);
    stack->value_size = value_size;
    stack->data = lib_mallocz(capacity * value_size);
    if (!stack->data) {
        return -1;
    }
    return 0;
}
