#ifndef PLAZMA_LIB_INTLIB_H
#define PLAZMA_LIB_INTLIB_H

#include <stdint.h>

static uint8_t _u8(char value) {
    return (uint8_t) value;
}

static uint16_t _u16(uint8_t b1, uint8_t b2) {
    return (b1 << 8) | b2;
}

//  buf[0] = (ocode >> 8) & 0xFF;
//  buf[1] = ocode & 0xFF;

static uint8_t _u81(uint16_t value) {    
    return (value >> 8) & 0xFF;
}

static uint8_t _u82(uint16_t value) { 
    return value & 0xFF;
}

#endif // PLAZMA_LIB_INTLIB_H
