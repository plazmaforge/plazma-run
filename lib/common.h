#ifndef PLAZMA_LIB_COMMON_H
#define PLAZMA_LIB_COMMON_H

#include <string.h>

#include "error.h"

void lib_platform_zeroize(void *buf, size_t len) {
    if (len > 0) {
        // TODO
        memset(buf, 0, len);
    }
}


#endif // PLAZMA_LIB_COMMON_H