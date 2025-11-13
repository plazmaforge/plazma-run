#ifndef PLAZMA_LIB_INTLIB_H
#define PLAZMA_LIB_INTLIB_H

#include <stdint.h>

static uint8_t _u8(char value) {
    return (uint8_t) value;
}

static uint16_t _u16(uint8_t b1, uint8_t b2) {
    return (b1 << 8) | b2;
}

#endif // PLAZMA_LIB_INTLIB_H
