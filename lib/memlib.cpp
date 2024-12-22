#include <stdlib.h>
#include <string.h>
#include "memlib.h"

void lib_memset0(void* data, size_t len) {
    if (/*!data || */len == 0) {
        return;
    }
    memset(data, 0, len);
}

void lib_free0(void* data, size_t len) {
    if (!data) {
        return;
    }
    lib_memset0(data, len);
    free(data);
}

void lib_memreset(void* data, size_t len) {
    lib_memset0(data, len);
}

void lib_freereset(void* data, size_t len) {
    lib_free0(data, len);
}



