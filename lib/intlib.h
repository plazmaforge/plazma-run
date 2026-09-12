#ifndef PLAZMA_LIB_INTLIB_H
#define PLAZMA_LIB_INTLIB_H

#include <stdint.h>

/**
 * Convert char value to uint8
 */
static uint8_t _u8(char value) {
    return (uint8_t) value;
}

/**
 * Set 'hi' and 'lo' uint8 values to uint16
 */
static uint16_t _u16(uint8_t b1, uint8_t b2) {
    return (b1 << 8) | b2;
}

//  buf[0] = (ocode >> 8) & 0xFF;
//  buf[1] = ocode & 0xFF;

/**
 * Get 'hi' uint8 value from uint16
 */
static uint8_t _u81(uint16_t value) {    
    return (value >> 8) & 0xFF;
}

/**
 * Get 'lo' uint8 value from uint16
 */
static uint8_t _u82(uint16_t value) { 
    return value & 0xFF;
}

#endif // PLAZMA_LIB_INTLIB_H
