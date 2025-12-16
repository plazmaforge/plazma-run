#ifndef PLAZMA_LIB_DCSLIB_H
#define PLAZMA_LIB_DCSLIB_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "intlib.h"
#include "unimap.h"
#include "encpre.h"

static int lib_dcs_char_seq(lib_unimap_t* unimap, int enc_id, const char* str);

static int lib_dcs_code_seq(lib_unimap_t* unimap, int enc_id, int cp);

static int lib_dcs_to_code(lib_unimap_t* unimap, int enc_id, const char* str, int* cp);

static int lib_dcs_to_char(lib_unimap_t* unimap, int enc_id, char* buf, int cp);

////

static int _enc_find_idx(int* map, size_t start, size_t len, int ucode) {
    if (!map || len == 0 || ucode < 0) {
        return -1;
    }
    size_t idx;
    for (size_t i = 0; i < len; i++) {
        idx = i + start;
        if (ucode == map[idx]) {
            return idx;
        }
    }
    return -1;
}

////

static int lib_dcs_char_seq(lib_unimap_t* unimap, int enc_id, const char* str) {
    int cp;
    return lib_dcs_to_code(unimap, enc_id, str, &cp);
}

static int lib_dcs_code_seq(lib_unimap_t* unimap, int enc_id, int cp) {
    // TODO    
    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    return lib_dcs_to_char(unimap, enc_id, buf, cp);
}

static int lib_dcs_to_code(lib_unimap_t* unimap, int enc_id, const char* str, int* cp) {

    #ifdef DEBUG_LL
    fprintf(stderr, ">> _dcs_to_code\n");
    #endif

    if (!str) {
        // error
        return -1;
    }

    // Get unsigned code
    int icode = _u8(*str);
    int ucode = 0;

    // 1. icode -> icode
    if (icode < unimap->start) {
        #ifdef DEBUG_LL
        fprintf(stderr, ">> icode : asc\n");
        #endif
        *cp = icode;
        return 1;
    }

    // 2. icode -> 'from_map'
    int idx = icode - unimap->start;
    #ifdef DEBUG_LL
    fprintf(stderr, ">> oidx  : %d\n", idx);
    #endif

    // Get Unicode from 'from_map"
    if (idx >= unimap->len) {
        // error: NO_CHR (?)
        // ACSII tab only or Leader chars
        // Leader chars in range [128 .. 255] (unimap->start .. unimap->len)
        #ifdef ERROR
        fprintf(stderr, ">> error: NO_CHR (-13)\n");
        #endif
        return -13;
    }

    icode = unimap->map[idx];
    if (icode == LD_CHR) {

        // Leader char
        // Check next char
        if (str[1] == '\0') {
            return -1;
        }

        // Get two chars
        uint8_t b1 = _u8(str[0]);
        uint8_t b2 = _u8(str[1]);

        // Convert two chars to DCS code
        icode = _u16(b1, b2);

        // Shift index
        idx = icode - unimap->ext_start + unimap->start;

        if (idx >= unimap->len + unimap->ext_len) {
            // error: NO_CHR (?)
            #ifdef ERROR
            fprintf(stderr, ">> error: NO_CHR (-14)\n");
            #endif
            return -14;
        }

        // Get Unicode by index in DCS table
        ucode = unimap->map[idx];
        *cp = ucode;
        return 2;
    }

    *cp = icode;
    return 1;

}

static int lib_dcs_to_char(lib_unimap_t* unimap, int enc_id, char* buf, int cp) {

    #ifdef DEBUG_LL
    fprintf(stderr, ">> dcs_to_char\n");
    #endif

    int ocode = 0;
    int ucode = cp;
    int seq   = 0;

    #ifdef DEBUG_L1
    fprintf(stderr, ">> ucode: 0x%04x \n", (unsigned int) ucode);
    #endif

    // Convert [EncodingID] codepoint to code by map
    if (cp < unimap->start) {
        ocode = cp;
        seq   = 1;
    } else {
        if (ucode == 0xFFFD) {
            #ifdef ERROR
            fprintf(stderr, ">> error : ucode is NO_CHR (U+FFFD)\n");
            #endif
            ocode = NO_DAT;
            seq   = 1;
        } else {
            int idx = _enc_find_idx(unimap->map, 0, unimap->len, ucode);
            if (idx < 0) {
                idx = _enc_find_idx(unimap->map, unimap->len, unimap->ext_len, ucode);
                if (idx < 0) {
                    #ifdef ERROR
                    fprintf(stderr, ">> error : ocode not found\n");
                    fprintf(stderr, ">> ext_start : %lu, ext_len: %lu\n", unimap->ext_start, unimap->ext_len);
                    #endif
                    ocode = NO_DAT;
                    seq   = 1;
                } else {
                    #ifdef DEBUG_LL
                    fprintf(stderr, ">> oidx  : %d\n", idx);
                    #endif
                    ocode = idx - unimap->len + unimap->ext_start;
                    seq   = 2;
                }
            } else {
                #ifdef DEBUG_LL
                fprintf(stderr, ">> oidx  : %d\n", idx);
                #endif
                ocode = idx + unimap->start;
                seq   = 1;
            }
        }

    }

    #ifdef DEBUG_L1
    fprintf(stderr, "<< ocode: 0x%04x \n", (unsigned int) ocode);
    #endif

    if (seq == 1) {
        *buf = (char) ocode;
    } else if (seq == 2) {
        buf[0] = _u81(ocode);
        buf[1] = _u82(ocode);
    }
    
    return seq;
}

#endif // PLAZMA_LIB_DCSLIB_H
