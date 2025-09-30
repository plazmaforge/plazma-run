#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "enclib.h"
#include "utf8lib.h"

// #define DEBUG    1
// #define DEBUG_LL 1
// #define DEBUG_L2 1
// #define ERROR    1

static char* _char_new(size_t size) {
  return (char*) calloc(size, sizeof(char));
}

static char* _data_new(size_t size) {
  char* data = _char_new(size + 1);
  if (!data) {
    return NULL;
  }
  data[size] = '\0';
  return data;
}

static char* _data_copy(char* src, size_t size) {
    if (!src) {
        return NULL;
    }
    char* data = _data_new(size);
    if (!data) {
        // error
        return NULL;
    }
        
    // Copy data
    memcpy(data, src, size);

    return data;
} 

static int _init_ctx(lib_enc_context_t* ctx) {
    if (!ctx) {
        return 1;
    }

    /* From */
    ctx->from_id     = 0;
    ctx->from_data   = NULL;
    ctx->from_len    = 0;
    ctx->from_map    = NULL;
    ctx->from_is_utf = false;

    /* To */
    ctx->to_id       = 0;
    ctx->to_data     = NULL;
    ctx->to_len      = 0;
    ctx->to_map      = NULL;
    ctx->to_is_utf   = false;

    return 0;
}

static uint8_t to_uint8(char value) {
    return (uint8_t) value;
}

////////////////////////////////////////////////////////////////////////////////////////

static size_t _enc_char_seq_len(bool is_utf, int id, const char* str);

static size_t _enc_code_seq_len(bool is_utf, int id, int cp);

static int _enc_to_code(lib_enc_context_t* ctx, int id, const char* str, int* cp);

static int _enc_to_char(lib_enc_context_t* ctx, int id, char* buf, int cp);

static int _enc_check_ctx(lib_enc_context_t* ctx);

static int _enc_conv_ctx(lib_enc_context_t* ctx);

static int _enc_conv_to_utf7_ctx(lib_enc_context_t* ctx);

static int _enc_conv_from_utf7_ctx(lib_enc_context_t* ctx);

////////////////////////////////////////////////////////////////////////////////////////

int lib_enc_bom_len(int id) {

    // UTF-8
    if (id == LIB_ENC_UTF8_ID) {
        return 0;
    }
    if (id == LIB_ENC_UTF8_BOM_ID) {
        return 3;
    }

    // UTF-16
    if (id == LIB_ENC_UTF16_ID) {
        return 2;
    }

    // UTF-16 (BE/LE)
    if (id == LIB_ENC_UTF16BE_ID
     || id == LIB_ENC_UTF16LE_ID) {
        return 0;
    }

    // UTF-16-BOM
    if (id == LIB_ENC_UTF16BE_BOM_ID
     || id == LIB_ENC_UTF16LE_BOM_ID) {
        return 2;
    }

    // UTF-32
    if (id == LIB_ENC_UTF32_ID) {
        return 4;
    }

    // UTF-32 (BE/LE)
    if (id == LIB_ENC_UTF32BE_ID
     || id == LIB_ENC_UTF32LE_ID) {
        return 0;
    }

    // UTF-32-BOM
    if (id == LIB_ENC_UTF32BE_BOM_ID
     || id == LIB_ENC_UTF32LE_BOM_ID) {
        return 4;
    }

    return 0;
}

/**
 * Set BOM and return count of BOM
 */
int lib_enc_set_bom(int id, char* str) {
    if (!str) {
        return 0;
    }

    // UTF-8
    if (id == LIB_ENC_UTF8_ID) {
        return 0;
    }
    if (id == LIB_ENC_UTF8_BOM_ID) {
        str[0] = 0xEF;
        str[1] = 0xBB;
        str[2] = 0xBF;
        return 3;
    }

    // UTF-16
    if (id == LIB_ENC_UTF16_ID 
     || id == LIB_ENC_UTF16BE_BOM_ID) {
        str[0] = 0xFE;
        str[1] = 0xFF;
        return 2;
    }
    if (id == LIB_ENC_UTF16LE_BOM_ID) {
        str[0] = 0xFF;
        str[1] = 0xFE;
        return 2;
    }

    // UTF-16 (BE/LE)
    if (id == LIB_ENC_UTF16BE_ID
     || id == LIB_ENC_UTF16LE_ID) {
        return 0;
    }

    // UTF-32
    if (id == LIB_ENC_UTF32_ID 
     || id == LIB_ENC_UTF32BE_BOM_ID) {
        str[0] = 0x00;
        str[1] = 0x00;
        str[2] = 0xFE;
        str[3] = 0xFF;
        return 4;
    }
    if (id == LIB_ENC_UTF32LE_BOM_ID) {
        str[0] = 0xFF;
        str[1] = 0xFE;
        str[2] = 0x00;
        str[3] = 0x00;
        return 4;
    }

    // UTF-32 (BE/LE)
    if (id == LIB_ENC_UTF32BE_ID
     || id == LIB_ENC_UTF32LE_ID) {
        return 0;
    }

    return 0;
}

bool lib_enc_supports_utf_conv(int id) {

    return (id == LIB_ENC_UTF7_ID

     || id == LIB_ENC_UTF8_ID
     || id == LIB_ENC_UTF8_BOM_ID

     || id == LIB_ENC_UTF16_ID
     || id == LIB_ENC_UTF16BE_ID
     || id == LIB_ENC_UTF16LE_ID
     || id == LIB_ENC_UTF16BE_BOM_ID
     || id == LIB_ENC_UTF16LE_BOM_ID

     || id == LIB_ENC_UTF32_ID
     || id == LIB_ENC_UTF32BE_ID
     || id == LIB_ENC_UTF32LE_ID
     || id == LIB_ENC_UTF32BE_BOM_ID
     || id == LIB_ENC_UTF32LE_BOM_ID);
} 

/**
 * Returns true if the Encoding ID supports conversion
 */
bool lib_enc_supports_conv(int id) {
    if (lib_enc_supports_utf_conv(id)) {
        return true;
    }

    return lib_unimap_supports_map(id);
} 

/**
 * Returns (conversion only) encoding id by encoding name 
 */
int lib_enc_get_conv_encoding_id(const char* name) {
    int id = lib_enc_get_encoding_id(name);
    if (id == 0) {
        return 0;
    }

    // Check support conversion
    if (!lib_enc_supports_conv(id)) {
        return 0;
    }
    return id;
}

/**
 * Converts data by Encoding IDs 
 */
int lib_enc_conv_by_id(int from_id, int to_id, char* from_data, size_t from_len,
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_by_id: from_id=%d, to_id=%d\n", from_id, to_id);
    #endif

    if (!from_data || !to_data || !to_len) {
        return -1;
    }    

    if (from_len == 0) {
        return 0;
    }

    bool sup_from = lib_enc_supports_conv(from_id);
    bool sup_to = lib_enc_supports_conv(to_id);

    if (!sup_from && !sup_to) {
        #ifdef ERROR
        fprintf(stderr, "Conversion from %d to %d unsupported\n", from_id, to_id);
        #endif
        return LIB_ENC_ERR_CONV_USUPPORTED;
    }
    if (!sup_from) {
        #ifdef ERROR
        fprintf(stderr, "Conversion from %d unsupported\n", from_id);
        #endif
        return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
    }
    if (!sup_to) {
        #ifdef ERROR
        fprintf(stderr, "Conversion to %d unsupported\n", from_id);
        #endif
        return LIB_ENC_ERR_CONV_TO_USUPPORTED;
    }

    if (from_id == to_id) {

        // Copy data
        char* data = _data_copy(from_data, from_len);
        if (!data) {
            // error
            return -1;
        }
        
        *to_data = data;
        *to_len  = from_len;

        return 0;
    }    

    *to_data = NULL;
    *to_len  = 0;

    bool has_from = lib_unimap_supports_map(from_id);
    bool has_to   = lib_unimap_supports_map(to_id);

    ////////////////////////////////////////////////////////////////////////////////
    // UTF7 Only
    ////////////////////////////////////////////////////////////////////////////////

    lib_enc_context_t ctx;
    _init_ctx(&ctx);

    ctx.from_id = from_id;
    ctx.from_data = from_data;
    ctx.from_len = from_len;

    ctx.to_id   = to_id;
    ctx.to_data = to_data;
    ctx.to_len = to_len;

    if (has_from) {
        lib_unimap_t from_map;
        lib_unimap_get_unimap_by_id(&from_map, from_id);
        ctx.from_map = &from_map;
        ctx.from_is_utf = false;
    } else {
        ctx.from_is_utf = true;
    }

    if (has_to) {
        lib_unimap_t to_map;
        lib_unimap_get_unimap_by_id(&to_map, to_id);
        ctx.to_map = &to_map;
        ctx.to_is_utf = false;
    } else {
        ctx.to_is_utf = true;
    }

    if (from_id == LIB_ENC_UTF7_ID) {
        if (lib_enc_supports_utf_conv(to_id)) {
            return _enc_conv_from_utf7_ctx(&ctx);
        } else {
            #ifdef ERROR
            fprintf(stderr, "Conversion from/to unsupported: %d, %d\n", from_id, to_id);
            #endif
            return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
        }
    }

    if (to_id == LIB_ENC_UTF7_ID) {
        if (lib_enc_supports_utf_conv(from_id)) {
            return _enc_conv_to_utf7_ctx(&ctx);
        } else {
            #ifdef ERROR
            fprintf(stderr, "Conversion from/to unsupported: %d, %d\n", from_id, to_id);
            #endif
            return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
        }
    }

    return _enc_conv_ctx(&ctx);

}

static int _to_code(char chr) {
    unsigned char u = (unsigned char) chr;
    return u;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

// Calculate lenght of output base64 array by input lenght
size_t to_b64_len(size_t len) {
    return (len * 4 + 2) / 3;
}

size_t to_u16_len(size_t len) {
    return (len * 3) / 4;
}

// flush utf7 block  to prepared (non utf7) byte array (bdata)
// count is unicode point count (!)
// bdata is shared for all data buffer by max count * 2 (hi/lo)

int to_u16_block(int from_id, char* idata, char* bdata, size_t bsize, size_t count) {

    int ucode  = 0;
    uint8_t hi = 0;
    uint8_t lo = 0;
    size_t i   = 0;
    size_t j   = 0;

    size_t from_seq_len = 0;

    char* data = idata;
    while (i < count) {

        // Calculate input sequence lenght of UTF-[ID] char
        from_seq_len = lib_utf_char_seq_len(from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            return -1;
        }

        // Convert input current UTF-[ID] char to codepoint
        int from_cp_len = lib_utf_to_code(from_id, data, &ucode);
        if (from_cp_len < 0) {
        //if (from_cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_cp_len_v2=%d\n", from_cp_len);
            #endif
            return -1;
        }

        // unicode point to hi and lo
        hi = ucode >> 8;
        lo = ucode & 0xFF;
        

        //#ifdef DEBUG
        //fprintf(stderr, "DEBUG: >> ucode=%d, hi=%d, lo=%d\n", ucode, hi, lo);
        //#endif

        // next codepoint
        i++; 
        data += from_seq_len;

        bdata[j] = hi;
        j++;

        bdata[j] = lo;
        j++;
    }
    return 0;
}

int to_char_block(int to_id, char* idata, char* odata, size_t osize, size_t count) {

    int ucode  = 0;
    uint16_t icode = 0;
    uint8_t hi = 0;
    uint8_t lo = 0;    
    size_t i   = 0;
    size_t j   = 0;

    size_t to_seq_len = 0;
    char* data = idata;
    char* buf = odata;
    while (i < count) {

        if (i + 1 >= count) {
            return -1;
        }

        hi = to_uint8(*data);
        data++;
        i++;
        lo = to_uint8(*data);

        // next (hi, lo) for x2 only
        data++;
        i++;

        // TODO: STUB
        //ucode = 65;

        icode = (icode << 8) | hi;
        icode = (icode << 8) | lo;
        ucode = icode;
  
        to_seq_len = lib_utf_to_char(to_id, buf, ucode);
        if (to_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq_len=%lu\n", to_seq_len);
            #endif
            return -1;
        }

        #ifdef DEBUG
        fprintf(stderr, "DEBUG: >> ucode=%d, hi=%d, lo=%d\n", ucode, hi, lo);
        #endif

        j++;
        buf += to_seq_len;
    }
    return 0;
}

// uint8_t to_base64
static const int map1[64] = {
    
    /* A-Z */
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90,

    /* a-z */
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,

    /* 0-9 */
    48, 49, 50, 51, 52, 53, 54, 55, 56, 57,

    /* +/ */
    43, 47
};

// int8_t from_base64
static const int map2[128] = {

    /* C0 controls, -1 for legal ones (CR LF TAB), -3 for illegal ones */
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -1, -1, -3, -3, -1, -3, -3,
    -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3, -3,

    /* general punctuation with + and / and a special value (-2) for - */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -2, -1, 63,

    /* digits */
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,

    /* A-Z */
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -3, -1, -1, -1,

    /* a-z */
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -3, -3
};

int base64_encode(char* idata, size_t isize, char* odata, size_t osize) {

    size_t dsize = (isize * 4 + 2) / 3;
    // osize = ((isize + 2) / 3) * 4;  // with ==
    // osize = dsize;

    size_t i = 0;
    size_t j = 0;

    uint8_t i0, i1, i2 = 0;
    uint8_t j0, j1, j2, j3 = 0;

    while (i < isize) {
        i0 = to_uint8(idata[i]);
        i++;

        if (i < isize) {
            i1 = to_uint8(idata[i]);
            i++;
        } else {
            i1 = 0;
        }

        if (i < isize) {
            i2 = to_uint8(idata[i]);
            i++;
        } else {
            i2 = 0;
        }

        ////

        //fprintf(stderr, ">> i-0: %d\n", i0);
        //fprintf(stderr, ">> i-1: %d\n", i1);
        //fprintf(stderr, ">> i-2: %d\n", i2);

        j0 = i0 / 4;
        j1 = ((i0 & 3) * 0x10) | (i1 / 0x10);
        j2 = ((i1 & 0x0F) * 4) | (i2 / 0x40);
        j3 = i2 & 0x3F;

        odata[j] = map1[j0];
        //fprintf(stderr, ">> odata-0: %d\n", odata[j]);
        j++;

        odata[j] = map1[j1];
        //fprintf(stderr, ">> odata-1: %d\n", odata[j]);
        j++;

        if (j < dsize) {
            odata[j] = map1[j2];
            //fprintf(stderr, ">> odata-2: %d\n", odata[j]);
        } else {
            // odata[j] = '=';
        }
        j++;

        if (j < dsize) {
            odata[j] = map1[j3];
            //fprintf(stderr, ">> odata-3: %d\n", odata[j]);
        } else {
            // odata[j] = '=';
        }
        j++;

    }
    return 0;
}

////

int base64_decode(char* idata, size_t isize, char* odata, size_t osize) {

    // osize = (isize * 3) / 4;

    size_t i = 0;
    size_t j = 0;

    char i0, i1, i2, i3 = 0;
    char b0, b1, b2, b3 = 0;
    char j0, j1, j2 = 0;

    while (i < isize) {
        i0 = idata[i];
        i++;
        i1 = idata[i];
        i++;

        if (i < isize) {
            i2 = idata[i];
            i++;
        } else {
            i2 = 'A';
        }

        if (i < isize) {
            i3 = idata[i];
            i++;
        } else {
            i3 = 'A';
        }

        // check i0-i3 > 127: error

        b0 = map2[i0];
        b1 = map2[i1];
        b2 = map2[i2];
        b3 = map2[i3];

        // check b0-b3 > 63: error

        j0 = (b0 * 4) | b1 / 0x10;
        j1 = ((b1 & 0x0F) * 0x10) | (b2 / 4);
        j2 = ((b2 & 3) * 0x40) | b3;

        odata[j] = j0;
        j++;
        if (j < osize) {
            odata[j] = j1;
            j++;
        }
        if (j < osize) {
            odata[j] = j2;
            j++;
        }
    }
    return 0;
}

/* Set directly */
bool IN_SET_D(int c) {
    return
       (c >= 65 && c <= 90)  /* A-Z */
    || (c >= 97 && c <= 122) /* a-z */
    || (c >= 48 && c <= 57)  /* 0-9 */
    || c == 39 
    || c == 40               /* ( */ 
    || c == 41               /* ) */ 
    || (c >= 44 && c <= 47)
    || c == 58               /* : */ 
    || c == 63;              /* ? */ 
}

/* Set optional */
bool IN_SET_O(int c) {
    return
       (c >= 33 && c <= 38)
    || c == 42               /* * */
    || (c >= 59 && c <= 64)
    || c == 91
    || (c >= 93 && c <= 96)
    || (c >= 123 && c <= 125);
}

/* Set base64 */
bool IN_SET_B(int c) {
    return
       (c >= 65 && c <= 90)  /* A-Z */
    || (c >= 97 && c <= 122) /* a-z */
    || (c >= 48 && c <= 57)  /* 0-9 */
    || c == 43 /* + */
    || c == 47; /* / */
}

/**
 * Converts data to UTF-7
 */
static int _enc_conv_to_utf7_ctx(lib_enc_context_t* ctx) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7_ctx\n");
    #endif

    if (ctx->to_data) {
        *(ctx->to_data) = NULL;
    }
    if (ctx->to_len) {
        *(ctx->to_len) = 0;
    }

    int err = _enc_check_ctx(ctx);
    if (err != 0) {
        return err;
    }

    if (ctx->from_len == 0) {
        #ifdef DEBUG
        fprintf(stderr, "DEBUG: 'from_len is 0: No conversation\n");
        #endif
        return 0;
    }

    int from_id      = ctx->from_id;
    char* from_data  = ctx->from_data;
    size_t from_len  = ctx->from_len;
    bool from_is_utf = ctx->from_is_utf;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_utf   = ctx->to_is_utf;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: from_id=%d, len=%lu\n", from_id, from_len);
    #endif


    //int to_id = LIB_ENC_UTF7_ID;

    // if (to_data) {
    //     *to_data = NULL;
    // }
    // if (to_len) {
    //     *to_len = 0;
    // }

    // if (!from_data || !to_data || !to_len) {
    //     return -1;
    // }
    // if (from_len == 0) {
    //     return 0;
    // }

    int ocode;
    int ucode;

    char* new_data      = NULL;
    size_t new_len      = 0;
    size_t from_seq_len = 0;
    //size_t to_seq_len   = 0;
    size_t total        = 0;

    size_t from_bom_len = lib_enc_bom_len(from_id);
    size_t to_bom_len   = lib_enc_bom_len(to_id);

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data     = from_data;
    int i          = from_bom_len;
    //int j          = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: starting...\n");
    #endif

    #ifdef DEBUGs
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

    bool start_block     = false; // Start UTF7 block flag
    size_t block_count   = 0;     // Count of Unicode points in UTF7 block
    size_t block_u16_len = 0;     // Size of binary block of Unicode point pairs (hi/lo): block_count * 2
    size_t block_b64_len = 0;     // Size of base64 block
    size_t u16_len       = 0;     // Max size of binary block
    char* u16_data       = NULL;  // Binary block data
    char* out_data       = NULL;
    char* cur_data       = NULL;
    bool use_set_o       = true;
    bool is_directly     = false;

    while (i < from_len) {

        // Calculate input sequence lenght of UTF-[ID] char
        from_seq_len = lib_utf_char_seq_len(from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            return -1;
        }

        // Convert input current UTF-[ID] char to codepoint
        int from_cp_len = lib_utf_to_code(from_id, data, &ucode);
        if (from_cp_len < 0) {
        //if (cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_cp_len_v2=%d\n", from_cp_len);
            #endif
            return -1;
        }

        /////////////////////////////////////////////////////////////

        is_directly = false;

        if (ucode <= 127) {
            
            if (IN_SET_D(ucode)) {
                /* Set directly */
                is_directly = true;
            } else if (IN_SET_O(ucode)) {
                /* Set optional */
                is_directly = !use_set_o;
            } else {
                /* Set other */
                is_directly = true;
            }

        } else if (ucode > 127) {
            is_directly = false;
        }

        if (is_directly) {

            if (start_block) {
                block_u16_len = block_count * 2; // hi, lo (Temp sulution for UTF16 x2 only)
                if (block_u16_len > u16_len) {
                    u16_len = block_u16_len;
                }

                total++;     // +
                block_b64_len = to_b64_len(block_u16_len);

                // Calc UTF7 block
                total +=  block_b64_len;

                if (i + from_seq_len < from_len) {
                    total++; // -
                }

                // End UF7 block
                start_block = false;
                //block_count = 0;
            }

            total++;     // Calc ASCII
            if (ucode == '+') {
                total++; // Calc "+-"
            }

        } else {

            if (!start_block) {

                // Start UTF7 block
                start_block = true;
                block_count = 0;
            }
            block_count++;
        }

        /////////////////////////////////////////////////////////////

        data += from_seq_len;
        i += from_seq_len;
    }

    if (i != from_len) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: i != from_len\n");
        #endif
        return -1;
    }

    // Last block
    if (start_block) {
        block_u16_len = block_count * 2; // hi, lo (Temp sulution for UTF16 x2 only)
        if (block_u16_len > u16_len) {
            u16_len = block_u16_len;
        }

        total++;     // '+'
        block_b64_len = to_b64_len(block_u16_len);

        // Calc UTF7 block
        total +=  block_b64_len;
                
        if (i + from_seq_len < from_len) {
            total++; // '-'
        }

        // End UF7 block
        start_block = false;
        block_count = 0;
    }

    new_len = total;
    if (new_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: new_len == 0\n");
        #endif
        return -1;
    }

    if (u16_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: u16_len == 0\n");
        #endif
        return -1;
    }

    new_data = _data_new(new_len + to_bom_len);
    if (!new_data) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        #endif
        return -1;
    }

    u16_data = _data_new(u16_len);
    if (!u16_data) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        #endif
        free(new_data);
        return -1;
    }

    lib_enc_set_bom(to_id, new_data);

    data         = from_data;
    i            = from_bom_len;
    from_seq_len = 0;
    total        = 0;
    out_data     = new_data + to_bom_len;
    start_block  = false;
    block_count  = 0;
        
    while (i < from_len) {

        // Calculate input sequence lenght of UTF-[ID] char
        from_seq_len = lib_utf_char_seq_len(from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            free(new_data);
            free(u16_data);
            return -1;
        }

        // Convert input current UTF-[ID] char to codepoint
        int from_cp_len = lib_utf_to_code(from_id, data, &ucode);
        if (from_cp_len < 0) {
        //if (from_cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_cp_len_v2=%d\n", from_cp_len);
            #endif
            free(new_data);
            free(u16_data);
            return -1;
        }

        /////////////////////////////////////////////////////////////

        is_directly = false;

        if (ucode <= 127) {

            if (IN_SET_D(ucode)) {
                /* Set directly */
                is_directly = true;
            } else if (IN_SET_O(ucode)) {
                /* Set optional */
                is_directly = !use_set_o;
            } else {
                /* Set other */
                is_directly = true;
            }

        } else if (ucode > 127) {

            is_directly = false;

        }

        if (is_directly) {

            if (start_block) {
                block_u16_len = block_count * 2; // hi, lo (Temp sulution for UTF16 x2 only)
                block_b64_len = to_b64_len(block_u16_len);

                // output to u16_data (hi, lo)
                to_u16_block(from_id, cur_data, u16_data, block_u16_len, block_count);

                *out_data = '+';
                out_data++;
                total++;     // +

                // flush UTF7 block
                // UTF16 -> base64
                base64_encode(u16_data, block_u16_len, out_data, block_b64_len);

                // Calc UTF7 block
                out_data += block_b64_len;
                total += block_b64_len;

                if (i + from_seq_len < from_len) {
                    *out_data = '-';
                    out_data++;
                    total++; // -
                }

                // End UF7 block
                start_block = false;
                cur_data    = NULL;
                //block_count = 0;
            }

            *out_data = (char) ucode;
            out_data++;
            total++;     // Calc ASCII

            if (ucode == '+') {

                *out_data = '-';
                out_data++;
                total++; // Calc "+-"
            }

        } else {

            if (!start_block) {

                // Start UTF7 block
                start_block = true;
                block_count = 0;
                cur_data = data;
            }
            block_count++;

        }

        /////////////////////////////////////////////////////////////

        data += from_seq_len;
        i += from_seq_len;

    }

    if (start_block) {
        block_u16_len = block_count * 2; // hi, lo (Temp sulution for UTF16 x2 only)
        block_b64_len = to_b64_len(block_u16_len);

        // output to u16_data (hi, lo)
        to_u16_block(from_id, cur_data, u16_data, block_u16_len, block_count);

        *out_data = '+';
        out_data++;
        total++;     // +

        // flush UTF7 block
        // UTF16 -> base64
        base64_encode(u16_data, block_u16_len, out_data, block_b64_len);
        out_data += block_b64_len;

        // Calc UTF7 block
        total += block_b64_len;

        if (i + from_seq_len < from_len) {
            *out_data = '-';
            out_data++;
            total++; // -
        }

        // End UF7 block
        start_block = false;
        cur_data    = NULL;
        //block_count = 0;
    }

    if (new_len != total) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Incorrect len: new_len=%lu, total=%lu\n", new_len, total);
        #endif
        free(new_data);
        free(u16_data);
        return -1;
    }

    free(u16_data);

    *to_data = new_data;
    *to_len  = new_len + to_bom_len;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_utf7: SUCCESS\n");
    #endif

    return 0;
}

/**
 * Converts data from UTF-7
 */
static int _enc_conv_from_utf7_ctx(lib_enc_context_t* ctx) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7_ctx\n");
    #endif

    if (ctx->to_data) {
        *(ctx->to_data) = NULL;
    }
    if (ctx->to_len) {
        *(ctx->to_len) = 0;
    }

    int err = _enc_check_ctx(ctx);
    if (err != 0) {
        return err;
    }

    if (ctx->from_len == 0) {
        #ifdef DEBUG
        fprintf(stderr, "DEBUG: 'from_len is 0: No conversation\n");
        #endif
        return 0;
    }

    int from_id      = ctx->from_id;
    char* from_data  = ctx->from_data;
    size_t from_len  = ctx->from_len;
    bool from_is_utf = ctx->from_is_utf;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_utf   = ctx->to_is_utf;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7: from_id=%d, to_id=%d, len=%lu\n", from_id, to_id, from_len);
    #endif

    
    // if (to_data) {
    //     *to_data = NULL;
    // }
    // if (to_len) {
    //     *to_len = 0;
    // }

    // if (!from_data || !to_data || !to_len) {
    //     return -1;
    // }
    // if (from_len == 0) {
    //     return 0;
    // }

    int icode;
    int ucode;

    char* new_data      = NULL;
    size_t new_len      = 0;
    size_t from_seq_len = 0;
    size_t total        = 0;

    size_t from_bom_len = lib_enc_bom_len(from_id);
    size_t to_bom_len   = lib_enc_bom_len(to_id);


    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data     = from_data;
    int i          = from_bom_len;
    int j          = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7: starting...\n");
    #endif

    #ifdef DEBUG
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

    bool start_block     = false; // Start UTF7 block flag
    size_t block_count   = 0;     // Count of Unicode points in UTF7 block
    size_t block_u16_len = 0;     // Size of binary block of Unicode point pairs (hi/lo): block_count * 2
    size_t block_b64_len = 0;     // Size of base64 block
    size_t block_out_len = 0;     // Size of output block
    size_t u16_len       = 0;     // Max size of binary block
    char* u16_data       = NULL;  // Binary block data
    char* out_data       = NULL;
    char* cur_data       = NULL;
    bool use_set_o       = true;
    bool is_directly     = false;

    while (i < from_len) {

        icode = _to_code(*data);

        #ifdef DEBUG_L2
        fprintf(stderr, "DEBUG: [1] icode=%d\n", icode);
        #endif

        from_seq_len = 1; // by default
        if (start_block) {

            // Check end block
            if (icode == '-') {
                block_b64_len = block_count;
                block_u16_len = to_u16_len(block_b64_len);
                if (block_u16_len > u16_len) {
                    u16_len = block_u16_len;
                }
                block_out_len = block_u16_len; // block_u16_len / 2; // TODO: STUB
                total += block_out_len; // Temp solution for UTF7 x2 only
                // skip total for '-'

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [1] flush-block: total=%lu\n", total);
                #endif

                start_block = false;
                //block_count = 0;
            } else {
                block_count++;
                // skip total for char - processing in end block
            }            

        } else {

            // Check start block
            if (icode == '+') {
                if (i + 1 < from_len && _to_code(data[i + 1]) == '-') {
                    from_seq_len = 2;
                    total++; // '-'

                    #ifdef DEBUG_LL
                    fprintf(stderr, "DEBUG: [1] flush '-': total=%lu\n", total);
                    #endif
                } else {
                    start_block = true;
                    block_count = 0;
                    // skip total for '+'
                }
            } else {
               total++;

               #ifdef DEBUG_LL
               fprintf(stderr, "DEBUG: [1] flush char: total=%lu\n", total);
               #endif
            }
        }

        data += from_seq_len;
        i += from_seq_len;

    }

    if (start_block) {
        block_b64_len = block_count;
        block_u16_len = to_u16_len(block_b64_len);
        if (block_u16_len > u16_len) {
            u16_len = block_u16_len;
        }
        block_out_len = block_u16_len; // block_u16_len / 2; // TODO: STUB
        total += block_out_len; // Temp solution for UTF7 x2 only

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [1] flush last: total=%lu\n", total);
        #endif

        start_block = false;
        //block_count = 0;
    }

    if (i != from_len) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: i != from_len\n");
        #endif
        return -1;
    }

    new_len = total;
    if (new_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: new_len == 0\n");
        #endif
        return -1;
    }

    new_data = _data_new(new_len + to_bom_len);
    if (!new_data) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Cannot allocate memory: new_data\n");
        #endif
        return -1;
    }

    u16_data = _data_new(u16_len);
    if (!u16_data) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Cannot allocate memory: u16_data\n");
        #endif
        free(new_data);
        return -1;
    }

    lib_enc_set_bom(to_id, new_data);

    data         = from_data;
    i            = from_bom_len;
    j            = to_bom_len;
    from_seq_len = 0;
    total        = 0;
    out_data     = new_data + to_bom_len;
    start_block  = false;
    block_count  = 0;
        
    while (i < from_len) {

        icode = _to_code(*data);

        #ifdef DEBUG_L2
        fprintf(stderr, "DEBUG: [2] icode=%d\n", icode);
        #endif

        from_seq_len = 1; // by default
        if (start_block) {

            // Check end block
            if (icode == '-') {
                block_b64_len = block_count;
                block_u16_len = to_u16_len(block_b64_len);

                block_out_len = block_u16_len; // block_u16_len / 2; // TODO: STUB

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [2] flush block: start: cur_data=%s, block_b64_len=%lu, block_u16_len=%lu\n", (cur_data ? "[OK]" : "[NULL]"), block_b64_len, block_u16_len);
                #endif

                // base64 -> UTF16 
                base64_decode(cur_data, block_b64_len, u16_data, block_u16_len);

                // output u16_data (hi, lo) to char data
                to_char_block(to_id, u16_data, out_data, block_out_len, block_u16_len);

                out_data += block_out_len;
                total += block_out_len; // Temp solution for UTF7 x2 only

                // skip total for '-'

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [2] flush block: success: total=%lu\n", total);
                #endif

                start_block = false;
                //block_count = 0;
                cur_data    = NULL;
            } else {
                block_count++;
                // skip total for char - processing in end block
            }            

        } else {

            // Check start block
            if (icode == '+') {
                if (i + 1 < from_len && _to_code(data[i + 1]) == '-') {
                    from_seq_len = 2;
                    total++; // '-'

                    #ifdef DEBUG_LL
                    fprintf(stderr, "DEBUG: [2] flush '-': total=%lu\n", total);
                    #endif
                } else {
                    start_block = true;
                    block_count = 0;
                    cur_data    = (i + 1 < from_len) ? (data + 1) : NULL;

                    #ifdef DEBUG_LL
                    fprintf(stderr, "DEBUG: [2] start block: cur_data=%s\n", (cur_data ? "[OK]" : "[NULL]"));
                    #endif

                    // skip total for '-'
                }
            } else {
                total++;

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [2] flush char: total=%lu\n", total);
                #endif
            }

        }

        data += from_seq_len;
        i += from_seq_len;

    }

    if (start_block) {
        block_b64_len = block_count;
        block_u16_len = to_u16_len(block_b64_len);

        block_out_len = block_u16_len; // block_u16_len / 2; // TODO: STUB

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [2] flush last: START: cur_data=%s, block_b64_len=%lu, block_u16_len=%lu\n", (cur_data ? "[OK]" : "[NULL]"), block_b64_len, block_u16_len);
        #endif

        // base64 -> UTF16 
        base64_decode(cur_data, block_b64_len, u16_data, block_u16_len);

        // output u16_data (hi, lo) to char data
        to_char_block(to_id, u16_data, out_data, block_out_len, block_u16_len);

        out_data += block_out_len;
        total += block_out_len; // Temp solution for UTF7 x2 only

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [2] flush block: SUCCESS: total=%lu\n", total);
        #endif

        start_block = false;
        //block_count = 0;
    }

    if (new_len != total) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Incorrect len: new_len=%lu, total=%lu\n", new_len, total);
        #endif
        free(new_data);
        free(u16_data);
        return -1;
    }

    free(u16_data);

    *to_data = new_data;
    *to_len  = new_len + to_bom_len;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7: SUCCESS\n");
    #endif

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

static size_t _enc_char_seq_len(bool is_utf, int id, const char* str) {
    if (!str) {
        return 0;
    }
    if (!is_utf) {
        return 1;
    }

    // UTF
    return lib_utf_char_seq_len(id, str);
}

static size_t _enc_code_seq_len(bool is_utf, int id, int cp) {
    if (!is_utf) {
        return 1;  // Lenght is always one
    }

    // UTF
    return lib_utf_code_seq_len(id, cp);
}

static int _enc_to_code(lib_enc_context_t* ctx, int id, const char* str, int* cp) {
    if (!ctx->from_is_utf) {

        // Get unsigned code
        int icode = _to_code(*str);

        // Convert char to  Unicode codepoint
        *cp = lib_unimap_conv_icode(ctx->from_map, icode);

        return 1; // Lenght is always one
    }

    // UTF
    return lib_utf_to_code(id, str, cp);
}

static int _enc_to_char(lib_enc_context_t* ctx, int id, char* buf, int cp) {

    if (!ctx->to_is_utf) {
        int ocode = 0;

        // Convert [EncodingID] codepoint to code by map
        if (cp < ctx->to_map->start) {
            ocode = cp; 
        } else {
            ocode = lib_unimap_conv_ucode(ctx->to_map, cp);
        }
        *buf = (char) ocode;
        return 1;
    }

    return lib_utf_to_char(id, buf, cp);
}

static int _enc_check_ctx(lib_enc_context_t* ctx) {
    if (!ctx) {
        #ifdef ERROR
        fprintf(stderr, "ERROR: 'context' is empty\n");
        #endif
        return -1;
    }
    int err = 0;
    if (!ctx->from_data) {
        #ifdef ERROR
        fprintf(stderr, "ERROR: 'from_data' is empty\n");
        #endif
        err = -1;
    }
    if (!ctx->to_data) {
        #ifdef ERROR
        fprintf(stderr, "ERROR: 'to_data' is empty\n");
        #endif
        err = -1;
    }
    if (!ctx->to_len) {
        #ifdef ERROR
        fprintf(stderr, "ERROR: 'to_len' is empty\n");
        #endif
        err = -1;
    }
    return err;
}

/**
 * Converts data from [EncodingID] to [EncodingID]
 */
static int _enc_conv_ctx(lib_enc_context_t* ctx) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx\n");
    #endif

    if (ctx->to_data) {
        *(ctx->to_data) = NULL;
    }
    if (ctx->to_len) {
        *(ctx->to_len) = 0;
    }

    int err = _enc_check_ctx(ctx);
    if (err != 0) {
        return err;
    }

    if (ctx->from_len == 0) {
        #ifdef DEBUG
        fprintf(stderr, "DEBUG: 'from_len is 0: No conversation\n");
        #endif
        return 0;
    }

    int from_id      = ctx->from_id;
    char* from_data  = ctx->from_data;
    size_t from_len  = ctx->from_len;
    bool from_is_utf = ctx->from_is_utf;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_utf   = ctx->to_is_utf;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx: from_id=%d, to_id=%d, len=%lu\n", from_id, to_id, from_len);
    #endif

    int ocode;
    int ucode;

    char* new_data      = NULL;
    size_t new_len      = 0;
    size_t from_seq_len = 0;
    size_t to_seq_len   = 0;
    size_t total        = 0;

    size_t from_bom_len = lib_enc_bom_len(from_id);
    size_t to_bom_len   = lib_enc_bom_len(to_id);


    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data     = from_data;
    int i          = from_bom_len;
    int j          = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx: starting...\n");
    #endif

    #ifdef DEBUG
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

    while (i < from_len) {

        // Calculate input sequence lenght of [EncodingID] char
        from_seq_len = _enc_char_seq_len(from_is_utf, from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            return -1;
        }

        // Convert input current [EncodingID] char to codepoint
        int from_cp_len = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_cp_len < 0) {
        //if (cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_cp_len_v2=%d\n", from_cp_len);
            #endif
            free(new_data);
            return -1;
        }

        to_seq_len = _enc_code_seq_len(to_is_utf, to_id, ucode);
        if (to_seq_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq_len=%lu\n", to_seq_len);
            #endif
            free(new_data);
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 1: to_seq_len=%lu\n", to_seq_len);
        #endif

        data += from_seq_len;
        i += from_seq_len;
        total += to_seq_len;
    }

    if (i != from_len) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: i != from_len\n");
        #endif
        return -1;
    }

    new_len = total;
    if (new_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: new_len == 0\n");
        #endif
        return -1;
    }

    new_data = _data_new(new_len + to_bom_len);
    if (!new_data) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Cannot allocate memory\n");
        #endif
        return -1;
    }

    lib_enc_set_bom(to_id, new_data);

    data         = from_data;
    i            = from_bom_len;
    j            = to_bom_len;
    from_seq_len = 0;
    to_seq_len   = 0;
    total        = 0;
        
    while (i < from_len) {

        // Calculate input sequence lenght of [EncodingID] char
        from_seq_len = _enc_char_seq_len(from_is_utf, from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            return -1;
        }

        // Convert input current [EncodingID] char to codepoint
        int from_cp_len = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_cp_len < 0) {
        //if (from_cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_cp_len_v2=%d\n", from_cp_len);
            #endif
            free(new_data);
            return -1;
        }

        // Convert Unicode codepoint to multi byte char
        to_seq_len = _enc_to_char(ctx, to_id, buf, ucode);
        if (to_seq_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq_len_v2=%lu\n", to_seq_len);
            #endif
            free(new_data);
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 2: to_seq_len=%lu\n", to_seq_len);
        #endif

        // Copy data from buffer to output
        for (int k = 0; k < to_seq_len; k++) {
            new_data[j + k] = buf[k];
        }

        data += from_seq_len;
        i += from_seq_len;

        j += to_seq_len;
        total += to_seq_len;

    }

    if (new_len != total) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Incorrect len: new_len=%lu, total=%lu\n", new_len, total);
        #endif
        free(new_data);
        return -1;
    }

    *to_data = new_data;
    *to_len  = new_len + to_bom_len;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx: SUCCESS\n");
    #endif

    return 0;
}

void _print_char(char* buf) {
    printf("%02X ",  (unsigned char) buf[0]);
    printf("%02X ",  (unsigned char) buf[1]);
    printf("%02X ",  (unsigned char) buf[2]);
    printf("%02X\n", (unsigned char) buf[3]);
}

int lib_enc_test2() {
    int i;
    int j;
    for (i = 0; i < 64; i++) {
        j = map1[i];
        //map2[j] = i: Next
        printf(">> MAP: i=%d, j=%d,\n",  i, j);
    }
    return 0;
}

int lib_enc_test() {

    if (true) {
        lib_enc_test2();
        return 0;
    }
    
    // 128640
    // 1F680
    // U+0001F680
    // -----------
    // 00 01 F6 80

    //int cp = 128640;
    int cp = 0x10437;
    char buf[5];

    printf("UNI-CP: %08X\n",  (unsigned int) cp);

    lib_utf32_to_char(buf, cp);
    printf("UTF-32: ");
    _print_char(buf);

    cp = 0;
    cp = lib_utf32_to_code(buf);
    printf("UNI-CP: %08X\n",  (unsigned int) cp);

    lib_utf16_to_char(buf, cp);
    printf("UTF-16: ");
    _print_char(buf);

    cp = 0;
    cp = lib_utf16_to_code(buf);
    printf("UNI-CP: %08X\n",  (unsigned int) cp);

    return 0;
}
