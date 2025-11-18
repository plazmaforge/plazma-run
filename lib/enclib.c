#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "intlib.h"
#include "utf7lib.h"
#include "utflib.h"
#include "ucslib.h"
#include "encbom.h"
#include "enclib.h"

// #define DEBUG    1
// #define DEBUG_LL 1
// #define DEBUG_L1 1
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
    ctx->from_is_mbc = false;

    /* To */
    ctx->to_id       = 0;
    ctx->to_data     = NULL;
    ctx->to_len      = 0;
    ctx->to_map      = NULL;
    ctx->to_is_mbc   = false;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////

static int _enc_char_seq(bool is_mbc, int enc_id, const char* str);

static int _enc_code_seq(bool is_mbc, int enc_id, int cp);

static int _enc_to_code(lib_enc_context_t* ctx, int enc_id, const char* str, int* cp);

static int _enc_to_char(lib_enc_context_t* ctx, int enc_id, char* buf, int cp);

static int _enc_check_ctx(lib_enc_context_t* ctx);

static int _enc_conv_ctx(lib_enc_context_t* ctx);

static int _enc_conv_to_utf7_ctx(lib_enc_context_t* ctx);

static int _enc_conv_from_utf7_ctx(lib_enc_context_t* ctx);

////////////////////////////////////////////////////////////////////////////////////////

/**
 * Return true if the Encoding ID supports conversion
 */
bool lib_enc_supports_conv(int enc_id) {

    if (lib_enc_is_utf7(enc_id)) {
        return true;
    }
    
    if (lib_enc_is_utf(enc_id)) {
        return true;
    }

    if (lib_enc_is_ucs(enc_id)) {
        return true;
    }

    return lib_unimap_supports_map(enc_id);
} 

/**
 * Return (conversion only) encoding id by encoding name 
 */
int lib_enc_get_conv_encoding_id(const char* name) {
    int enc_id = lib_enc_get_id(name);
    if (enc_id == 0) {
        return 0;
    }

    // Check support conversion
    if (!lib_enc_supports_conv(enc_id)) {
        return 0;
    }
    return enc_id;
}

/**
 * Convert data by Encoding IDs 
 */
int lib_enc_conv_by_id(int from_id, int to_id, char* from_data, size_t from_len, char** to_data, size_t* to_len) {

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
    bool sup_to   = lib_enc_supports_conv(to_id);

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

    lib_enc_context_t ctx;
    _init_ctx(&ctx);

    ctx.from_id   = from_id;
    ctx.from_data = from_data;
    ctx.from_len  = from_len;

    ctx.to_id     = to_id;
    ctx.to_data   = to_data;
    ctx.to_len    = to_len;

    if (has_from) {
        lib_unimap_t from_map;
        lib_unimap_get_unimap_by_id(&from_map, from_id);
        ctx.from_map = &from_map;
        ctx.from_is_mbc = false;
    } else {
        ctx.from_is_mbc = true;
    }

    if (has_to) {
        lib_unimap_t to_map;
        lib_unimap_get_unimap_by_id(&to_map, to_id);
        ctx.to_map = &to_map;
        ctx.to_is_mbc = false;
    } else {
        ctx.to_is_mbc = true;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // UTF7 encoding only
    ////////////////////////////////////////////////////////////////////////////////

    if (lib_enc_is_utf7(from_id)) {
        if (lib_enc_supports_conv(to_id)) {
            return _enc_conv_from_utf7_ctx(&ctx);
        } else {
            #ifdef ERROR
            fprintf(stderr, "Conversion from/to unsupported: %d, %d\n", from_id, to_id);
            #endif
            return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
        }
    }

    if (lib_enc_is_utf7(to_id)) {
        if (lib_enc_supports_conv(from_id)) {
            return _enc_conv_to_utf7_ctx(&ctx);
        } else {
            #ifdef ERROR
            fprintf(stderr, "Conversion from/to unsupported: %d, %d\n", from_id, to_id);
            #endif
            return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    // Other encodings
    ////////////////////////////////////////////////////////////////////////////////

    return _enc_conv_ctx(&ctx);

}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
// UTF7: BEGIN
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Convert char data to UTF16 block
 */
static int _u16_block(lib_enc_context_t* ctx, char* idata, char* odata, size_t osize, size_t count) {

    int from_id  = ctx->from_id;
    int ucode    = 0;
    size_t i     = 0;
    int from_seq = 0;
    int u16_seq  = 0;
    char* data   = idata;
    char* buf    = odata;

    while (i < count) {

        // char [EncodingID] -> codepoint
        from_seq = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq=%d\n", from_seq);
            #endif
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> ucode=%d\n", ucode);
        #endif

        // codepoint -> utf16
        u16_seq = lib_utf16_to_char(buf, ucode);
        buf += u16_seq;

        // next codepoint
        i++; 
        data += from_seq;

    }
    return 0;
}

/**
 * Convert UTF16 block to char data
 */
static int _char_block(lib_enc_context_t* ctx, char* idata, char* odata, size_t osize, size_t count) {

    int to_id     = ctx->to_id;
    int ucode     = 0;
    size_t i      = 0;
    int to_seq    = 0;
    int u16_seq   = 0;
    char* data    = idata;
    char* buf     = odata;

    while (i < count) {

        if (i + 1 >= count) {
            return -1;
        }

        // utf16 -> codepoint
        u16_seq = lib_utf16_to_code(data, &ucode);
        data += u16_seq;
        i += u16_seq;

        // codepoint -> char [EncodingID]
        to_seq = _enc_to_char(ctx, to_id, buf, ucode);
        if (to_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq=%d\n", to_seq);
            #endif
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> ucode=%d\n", ucode);
        #endif

        buf += to_seq;

    }
    return 0;
}

///////////////////////////////////////////////////////////////////////

static int _enc_utf16_to_code(const char* str) {
    int cp = 0;
    int err = lib_utf16_to_code(str, &cp);
    return cp;
}

int base64_decode_count(lib_enc_context_t* ctx, char* idata, size_t isize, size_t osize, size_t* count) {

    // u16_len
    // osize = (isize * 3) / 4;
    
    if (count) {
        *count = 0;
    }
    int to_id   = ctx->to_id;
    bool is_mbc = ctx->to_is_mbc;

    size_t i = 0;
    size_t j = 0;

    char i0, i1, i2, i3 = 0;
    char b0, b1, b2, b3 = 0;
    char j0, j1, j2 = 0;

    size_t b = 0;
    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    int label = 0;

    size_t seq_len = 0;
    size_t u16_len = 0;
    size_t out_len = 0;
    size_t out_seq = 0;
    int ucode      = 0;
    
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

        #ifdef DEBUG_L2
        fprintf(stderr, "DEBUG: base64_decode  : label_0:\n");
        #endif

        //odata[j] = j0;
        buf[b]   = j0;
        b++;
        j++;

        label = 1;
        goto check;

        label_1:
        label = 0;
        if (j < osize) {

            #ifdef DEBUG_L2
            fprintf(stderr, "DEBUG: base64_decode  : label_1:\n");
            #endif

            //odata[j] = j1;
            buf[b]   = j1;
            b++;
            j++;
        }

        label = 2;
        goto check;

        label_2:
        label = 0;
        if (j < osize) {

            #ifdef DEBUG_L2
            fprintf(stderr, "DEBUG: base64_decode  : label_2:\n");
            #endif

            //odata[j] = j2;
            buf[b]   = j2;
            b++;
            j++;
        }

        check:

        #ifdef DEBUG_L1
        fprintf(stderr, "DEBUG: base64_decode  : check=%lu\n", b);
        #endif

        if (b == 4) {
            #ifdef DEBUG_L1
            fprintf(stderr, "DEBUG: base64_decode  : check=4: [OK]\n");
            #endif

            seq_len = lib_utf16_char_seq(buf);
            if (seq_len == 4) {

                ucode = _enc_utf16_to_code(buf);
                out_seq = _enc_code_seq(is_mbc, to_id, ucode);
                out_len += out_seq;

                #ifdef DEBUG_L2
                fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
                #endif

                u16_len += seq_len;
                b = 0;
            } else if (seq_len == 2) {
                    
                ucode = _enc_utf16_to_code(buf);
                out_seq = _enc_code_seq(is_mbc, to_id, ucode);
                out_len += out_seq;

                #ifdef DEBUG_L2
                fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
                #endif

                u16_len += seq_len;

                seq_len = lib_utf16_char_seq(buf + 2);
                if (seq_len == 2) {

                    ucode = _enc_utf16_to_code(buf + 2);
                    out_seq = _enc_code_seq(is_mbc, to_id, ucode);
                    out_len += out_seq;

                    #ifdef DEBUG_L2
                    fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
                    #endif

                    u16_len += seq_len;
                    b = 0;
                    
                } else if (seq_len == 4) {

                    // shift 2 bytes
                    buf[0] = buf[2];
                    buf[1] = buf[3];
                    buf[2] = '\0';
                    buf[3] = '\0';

                    // next time
                    b = 2;
                } else {
                    // error
                }
            }

        } else if (b == 2) {
            
            #ifdef DEBUG_L1
            fprintf(stderr, "DEBUG: base64_decode  : check=2: [OK]\n");
            #endif

            buf[2] = '\0';
            buf[3] = '\0';

            seq_len = lib_utf16_char_seq(buf);
            if (seq_len == 2) {

                ucode = _enc_utf16_to_code(buf);
                out_seq = _enc_code_seq(is_mbc, to_id, ucode);
                out_len += out_seq;

                #ifdef DEBUG_L2
                fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
                #endif

                u16_len += seq_len;
                b = 0;
            } else {
                // error
            }
        }

        if (label == 1) {
            label = 0;
            goto label_1;
        } else if (label == 2) {
            label = 0;
            goto label_2;
        }
        label = 0;

    }


    if (b == 4) {
        #ifdef DEBUG_L1
        fprintf(stderr, "DEBUG: base64_decode  : check=4: [OK]\n");
        #endif

        seq_len = lib_utf16_char_seq(buf);
        if (seq_len == 4) {

            ucode = _enc_utf16_to_code(buf);
            out_seq = _enc_code_seq(is_mbc, to_id, ucode);
            out_len += out_seq;

            #ifdef DEBUG_L2
            fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
            #endif

            u16_len += seq_len;
            b = 0;
        } else if (seq_len == 2) {
                    
            ucode = _enc_utf16_to_code(buf);
            out_seq = _enc_code_seq(is_mbc, to_id, ucode);
            out_len += out_seq;

            #ifdef DEBUG_L2
            fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
            #endif

            u16_len += seq_len;

            seq_len = lib_utf16_char_seq(buf + 2);
            if (seq_len == 2) {

                ucode = _enc_utf16_to_code(buf + 2);
                out_seq = _enc_code_seq(is_mbc, to_id, ucode);
                out_len += out_seq;

                #ifdef DEBUG_L2
                fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
                #endif

                u16_len += seq_len;
                b = 0;            
            } else if (seq_len == 4) {

                // shift 2 bytes
                buf[0] = buf[2];
                buf[1] = buf[3];
                buf[2] = '\0';
                buf[3] = '\0';

                // next time
                b = 2;
            } else {
                   // error
            }
        }

    } else if (b == 2) {
            
        #ifdef DEBUG_L1
        fprintf(stderr, "DEBUG: base64_decode  : check=2: [OK]\n");
        #endif

        buf[2] = '\0';
        buf[3] = '\0';

        seq_len = lib_utf16_char_seq(buf);
        if (seq_len == 2) {

            ucode = _enc_utf16_to_code(buf);
            out_seq = _enc_code_seq(is_mbc, to_id, ucode);
            out_len += out_seq;

            #ifdef DEBUG_L2
            fprintf(stderr, "DEBUG: base64_decode  : ucode=%d\n", ucode);
            #endif

            u16_len += seq_len;
            b = 0;
        } else {
            // error
        }
    }

    #ifdef DEBUG_LL
    fprintf(stderr, "DEBUG: base64_decode  : out_len=%lu\n", out_len);
    #endif

    #ifdef DEBUG_LL
    fprintf(stderr, "DEBUG: base64_decode  : u16_len=%lu\n", u16_len);
    #endif

    if (count) {
        *count = out_len;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////

/**
 * Convert data to UTF-7
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
    bool from_is_mbc = ctx->from_is_mbc;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_mbc   = ctx->to_is_mbc;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: from_id=%d, len=%lu\n", from_id, from_len);
    #endif

    int ocode;
    int ucode;

    char* new_data   = NULL;
    size_t new_len   = 0;
    size_t total     = 0;
    int from_seq     = 0;

    int from_bom_len = lib_enc_bom_len(from_id);
    int to_bom_len   = lib_enc_bom_len(to_id);

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data       = from_data + from_bom_len;
    int i            = from_bom_len;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: starting...\n");
    #endif

    #ifdef DEBUG
    fprintf(stderr, "DEBUG: from_bom_len=%d, to_bom_len=%d\n", from_bom_len, to_bom_len);
    #endif

    bool start_block     = false; // Start UTF7 block flag
    size_t block_count   = 0;     // Count of Unicode codepoints in UTF7 block (!)
    size_t block_u16_len = 0;     // Lenght of UTF16 block
    size_t block_b64_len = 0;     // Lenght of base64 block
    size_t u16_len       = 0;     // Max lenght of UTF16 block
    size_t u16_count     = 0;
    int u16_seq          = 0;

    char* u16_data       = NULL;  // UTF16 block data
    char* out_data       = NULL;
    char* cur_data       = NULL;
    bool use_set_o       = true;
    bool is_directly     = false;

    while (i < from_len) {

        // char [EncodingID] -> codepoint
        from_seq = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq=%d\n", from_seq);
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
                block_u16_len = u16_count;
                if (block_u16_len > u16_len) {
                    u16_len = block_u16_len;
                }

                total++;     // +
                block_b64_len = _b64_len(block_u16_len);

                // Calc UTF7 block
                total +=  block_b64_len;

                if (i + from_seq < from_len) {
                    total++; // -
                }

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: >> 1: block_count=%lu\n", block_count);
                #endif

                // End UF7 block
                start_block = false;
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
                u16_count   = 0;
            }

            block_count++;
            
            //if (from_id == LIB_ENC_UTF16_ID) {
            if (lib_enc_is_utf16(from_id)) {
                u16_seq = from_seq;
            } else {
                u16_seq = lib_utf16_code_seq(ucode);
            }
            u16_count += u16_seq;

        }

        /////////////////////////////////////////////////////////////

        data += from_seq;
        i += from_seq;
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
        block_u16_len = u16_count;
        if (block_u16_len > u16_len) {
            u16_len = block_u16_len;
        }

        total++;     // '+'
        block_b64_len = _b64_len(block_u16_len);

        // Calc UTF7 block
        total +=  block_b64_len;
                
        if (i + from_seq < from_len) {
            total++; // '-'
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 1: block_count=%lu\n", block_count);
        #endif

        // End UF7 block
        start_block = false;
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

    data         = from_data + from_bom_len;
    i            = from_bom_len;
    from_seq     = 0;
    total        = 0;
    out_data     = new_data + to_bom_len;
    start_block  = false;
    block_count  = 0;
        
    while (i < from_len) {

        // char [EncodingID] -> codepoint
        from_seq = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq=%d\n", from_seq);
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
                block_u16_len = u16_count;
                block_b64_len = _b64_len(block_u16_len);

                // char -> UTF16
                _u16_block(ctx, cur_data, u16_data, block_u16_len, block_count);

                *out_data = '+';
                out_data++;
                total++;     // +

                // flush UTF7 block
                // UTF16 -> base64
                base64_encode(u16_data, block_u16_len, out_data, block_b64_len);

                // Calc UTF7 block
                out_data += block_b64_len;
                total += block_b64_len;

                if (i + from_seq < from_len) {
                    *out_data = '-';
                    out_data++;
                    total++; // -
                }

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: >> 2: block_count=%lu\n", block_count);
                #endif

                // End UF7 block
                start_block = false;
                cur_data    = NULL;
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
                u16_count   = 0;
                cur_data    = data;
            }

            block_count++;
            //if (from_id == LIB_ENC_UTF16_ID) {
            if (lib_enc_is_utf16(from_id)) {
                u16_seq = from_seq;
            } else {
                u16_seq = lib_utf16_code_seq(ucode);
            }
            u16_count += u16_seq;

        }

        /////////////////////////////////////////////////////////////

        data += from_seq;
        i += from_seq;

    }

    if (start_block) {
        block_u16_len = u16_count;
        block_b64_len = _b64_len(block_u16_len);

        // char -> UTF16
        _u16_block(ctx, cur_data, u16_data, block_u16_len, block_count);

        *out_data = '+';
        out_data++;
        total++;     // +

        // flush UTF7 block
        // UTF16 -> base64
        base64_encode(u16_data, block_u16_len, out_data, block_b64_len);
        out_data += block_b64_len;

        // Calc UTF7 block
        total += block_b64_len;

        if (i + from_seq < from_len) {
            *out_data = '-';
            out_data++;
            total++; // -
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 2: block_count=%lu\n", block_count);
        #endif

        // End UF7 block
        start_block = false;
        cur_data    = NULL;
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
    fprintf(stderr, ">> conv_utf7: SUCCESS: total=%lu\n", total);  
    #endif

    return 0;
}

/**
 * Convert data from UTF-7
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
    bool from_is_mbc = ctx->from_is_mbc;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_mbc   = ctx->to_is_mbc;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7: from_id=%d, to_id=%d, len=%lu\n", from_id, to_id, from_len);
    #endif

    int icode;
    int ucode;

    char* new_data   = NULL;
    size_t new_len   = 0;
    size_t total     = 0;
    int from_seq     = 0;

    int from_bom_len = lib_enc_bom_len(from_id);
    int to_bom_len   = lib_enc_bom_len(to_id);

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data       = from_data + from_bom_len;
    int i            = from_bom_len;
    int j            = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf7: starting...\n");
    #endif

    #ifdef DEBUG
    fprintf(stderr, "DEBUG: from_bom_len=%d, to_bom_len=%d\n", from_bom_len, to_bom_len);
    #endif

    bool start_block     = false; // Start UTF7 block flag
    size_t block_count   = 0;     // Count of Unicode points in UTF7 block
    size_t decode_count  = 0;
    size_t block_u16_len = 0;     // Lenght of UTF16 block
    size_t block_b64_len = 0;     // Lenght of base64 block
    size_t block_out_len = 0;     // Lenght of output block
    size_t u16_len       = 0;     // Max lenght of UTF16 block
    char* u16_data       = NULL;  // UTF16 block data
    char* out_data       = NULL;
    char* cur_data       = NULL;
    bool use_set_o       = true;
    bool is_directly     = false;

    while (i < from_len) {

        icode = _u8(*data);

        #ifdef DEBUG_L2
        fprintf(stderr, "DEBUG: [1] icode=%d\n", icode);
        #endif

        from_seq = 1; // by default
        if (start_block) {

            // Check end block
            if (icode == '-') {
                block_b64_len = block_count;
                block_u16_len = _u16_len(block_b64_len);
                if (block_u16_len > u16_len) {
                    u16_len = block_u16_len;
                }

                // base64 -> UTF16
                base64_decode_count(ctx, cur_data, block_b64_len, block_u16_len, &decode_count);

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [1] start block: u16_len=%lu\n", u16_len);
                #endif

                block_out_len = decode_count;
                total += block_out_len;
                // skip total for '-'

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [1] flush block: total=%lu\n", total);
                #endif

                start_block = false;
                cur_data    = NULL;
            } else {
                block_count++;
                // skip total for char - processing in end block
            }            

        } else {

            // Check start block
            if (icode == '+') {
                if (i + 1 < from_len && _u8(data[i + 1]) == '-') {
                    from_seq = 2;
                    total++; // '-'

                    #ifdef DEBUG_LL
                    fprintf(stderr, "DEBUG: [1] flush '-': total=%lu\n", total);
                    #endif
                } else {
                    start_block = true;
                    block_count = 0;
                    cur_data    = (i + 1 < from_len) ? (data + 1) : NULL;
                    // skip total for '+'
                }
            } else {
               total++;

               #ifdef DEBUG_LL
               fprintf(stderr, "DEBUG: [1] flush char: total=%lu\n", total);
               #endif
            }
        }

        data += from_seq;
        i += from_seq;

    }

    if (start_block) {
        block_b64_len = block_count;
        block_u16_len = _u16_len(block_b64_len);
        if (block_u16_len > u16_len) {
            u16_len = block_u16_len;
        }

        // base64 -> UTF16
        base64_decode_count(ctx, cur_data, block_b64_len, block_u16_len, &decode_count);

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [1] last block : u16_len=%lu\n", u16_len);
        #endif

        block_out_len = decode_count;
        total += block_out_len;

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [1] flush last : total=%lu\n", total);
        #endif

        start_block = false;
        cur_data    = NULL;
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

    data         = from_data + from_bom_len;
    i            = from_bom_len;
    j            = to_bom_len;
    from_seq     = 0;
    total        = 0;
    out_data     = new_data + to_bom_len;
    start_block  = false;
    block_count  = 0;
        
    while (i < from_len) {

        icode = _u8(*data);

        #ifdef DEBUG_L2
        fprintf(stderr, "DEBUG: [2] icode=%d\n", icode);
        #endif

        from_seq = 1; // by default
        if (start_block) {

            // Check end block
            if (icode == '-') {
                block_b64_len = block_count;
                block_u16_len = _u16_len(block_b64_len);

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [2] flush block: start: cur_data=%s, block_b64_len=%lu, block_u16_len=%lu\n", (cur_data ? "[OK]" : "[NULL]"), block_b64_len, block_u16_len);
                #endif

                // TODO: Temp solution
                base64_decode_count(ctx, cur_data, block_b64_len, block_u16_len, &decode_count);

                block_out_len = decode_count;

                // base64 -> UTF16 
                base64_decode(cur_data, block_b64_len, u16_data, block_u16_len);

                // UTF16 -> char
                _char_block(ctx, u16_data, out_data, block_out_len, block_u16_len);

                out_data += block_out_len;
                total += block_out_len;

                // skip total for '-'

                #ifdef DEBUG_LL
                fprintf(stderr, "DEBUG: [2] flush block: success: total=%lu\n", total);
                #endif

                start_block = false;
                cur_data    = NULL;
            } else {
                block_count++;
                // skip total for char - processing in end block
            }            

        } else {

            // Check start block
            if (icode == '+') {
                if (i + 1 < from_len && _u8(data[i + 1]) == '-') {
                    from_seq = 2;
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

        data += from_seq;
        i += from_seq;

    }

    if (start_block) {
        block_b64_len = block_count;
        block_u16_len = _u16_len(block_b64_len);

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [2] flush last: START: cur_data=%s, block_b64_len=%lu, block_u16_len=%lu\n", (cur_data ? "[OK]" : "[NULL]"), block_b64_len, block_u16_len);
        #endif

        // TODO: Temp solution
        base64_decode_count(ctx, cur_data, block_b64_len, block_u16_len, &decode_count);

        block_out_len = decode_count;

        // base64 -> UTF16
        base64_decode(cur_data, block_b64_len, u16_data, block_u16_len);

        // UTF16 -> char
        _char_block(ctx, u16_data, out_data, block_out_len, block_u16_len);

        out_data += block_out_len;
        total += block_out_len;

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: [2] flush block: SUCCESS: total=%lu\n", total);
        #endif

        start_block = false;
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
//
// UTF7: END
//
/////////////////////////////////////////////////////////////////////////////////////////////////

static int _enc_char_seq(bool is_mbc, int enc_id, const char* str) {
    #ifdef DEBUG_L2
    fprintf(stderr, ">> enc_char_seq\n");
    #endif

    if (!str) {

        #ifdef DEBUG_L2
        fprintf(stderr, ">> enc_char_seq: return 0\n");
        #endif

        return 0;
    }

    // 1-BYTE
    if (!is_mbc) {
        #ifdef DEBUG_L2
        fprintf(stderr, ">> enc_char_seq: return 1\n");
        #endif
        return 1;
    }

    // UCS
    if (lib_enc_is_ucs(enc_id)) {
        return lib_ucs_char_seq(enc_id, str);
    }

    // UTF
    if (lib_enc_is_utf(enc_id)) {
        return lib_utf_char_seq(enc_id, str);
    }

    // OTHER
    return -1;
}

static int _enc_code_seq(bool is_mbc, int enc_id, int cp) {

    // 1-BYTE
    if (!is_mbc) {
        return 1;  // Lenght is always one
    }

    // UCS
    if (lib_enc_is_ucs(enc_id)) {
        return lib_ucs_code_seq(enc_id, cp);
    }

    // UTF
    if (lib_enc_is_utf(enc_id)) {
        return lib_utf_code_seq(enc_id, cp);
    }

    // OTHER
    return -1;
}

static int _enc_to_code(lib_enc_context_t* ctx, int enc_id, const char* str, int* cp) {

    // 1-BYTE
    if (!ctx->from_is_mbc) {

        // Get unsigned code
        int icode = _u8(*str);

        // Convert char to  Unicode codepoint
        *cp = lib_unimap_conv_icode(ctx->from_map, icode);

        return 1; // Lenght is always one
    }

    // UCS
    if (lib_enc_is_ucs(enc_id)) {
        return lib_ucs_to_code(enc_id, str, cp);
    }

    // UTF
    if (lib_enc_is_utf(enc_id)) {
        return lib_utf_to_code(enc_id, str, cp);
    }

    // OTHER
    return -1;
}

static int _enc_to_char(lib_enc_context_t* ctx, int enc_id, char* buf, int cp) {

    // 1-BYTE
    if (!ctx->to_is_mbc) {
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

    // UCS
    if (lib_enc_is_ucs(enc_id)) {
        return lib_ucs_to_char(enc_id, buf, cp);
    }

    // UTF
    if (lib_enc_is_utf(enc_id)) {
        return lib_utf_to_char(enc_id, buf, cp);
    }

    // OTHER
    return -1;
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
 * Convert data from [EncodingID] to [EncodingID]
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
    bool from_is_mbc = ctx->from_is_mbc;

    int to_id        = ctx->to_id;
    char** to_data   = ctx->to_data;
    size_t* to_len   = ctx->to_len;
    bool to_is_mbc   = ctx->to_is_mbc;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx: from_id=%d, to_id=%d, len=%lu\n", from_id, to_id, from_len);
    #endif

    int ocode;
    int ucode;

    char* new_data      = NULL;
    size_t new_len      = 0;
    size_t total        = 0;
    int from_seq        = 0;
    int to_seq          = 0;

    size_t from_bom_len = lib_enc_bom_len(from_id);
    size_t to_bom_len   = lib_enc_bom_len(to_id);

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)

    char* data          = from_data + from_bom_len;
    int i               = from_bom_len;
    int j               = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_ctx: starting...\n");
    #endif

    #ifdef DEBUG
    fprintf(stderr, "DEBUG: from_is_mbc=%d, to_is_mbc=%d\n", from_is_mbc, to_is_mbc);
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

    while (i < from_len) {

        // char [EncodingID] -> codepoint
        from_seq = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq=%d\n", from_seq);
            #endif
            free(new_data);
            return -1;
        }

        // codepoint -> char [EncodingID]
        to_seq = _enc_code_seq(to_is_mbc, to_id, ucode);
        if (to_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq=%d\n", to_seq);
            #endif
            free(new_data);
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 1: from_seq=%d, to_seq=%d\n", from_seq, to_seq);
        #endif

        data += from_seq;
        i += from_seq;
        total += to_seq;
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

    data         = from_data + from_bom_len;
    i            = from_bom_len;
    j            = to_bom_len;
    from_seq     = 0;
    to_seq       = 0;
    total        = 0;
        
    while (i < from_len) {

        // char [EncodingID] -> codepoint
        from_seq = _enc_to_code(ctx, from_id, data, &ucode);
        if (from_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq=%d\n", from_seq);
            #endif
            free(new_data);
            return -1;
        }

        // codepoint -> char [EncodingID]          
        to_seq = _enc_to_char(ctx, to_id, buf, ucode);
        if (to_seq <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: to_seq=%d\n", to_seq);
            #endif
            free(new_data);
            return -1;
        }

        #ifdef DEBUG_LL
        fprintf(stderr, "DEBUG: >> 2: from_seq=%d, to_seq=%d\n", from_seq, to_seq);
        #endif

        // Copy data from buffer to output
        for (int k = 0; k < to_seq; k++) {
            new_data[j + k] = buf[k];
        }

        data += from_seq;
        i += from_seq;

        j += to_seq;
        total += to_seq;

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
    fprintf(stderr, ">> conv_ctx: SUCCESS: total=%lu\n", total);
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
    lib_utf32_to_code(buf, &cp);
    printf("UNI-CP: %08X\n",  (unsigned int) cp);

    lib_utf16_to_char(buf, cp);
    printf("UTF-16: ");
    _print_char(buf);

    cp = 0;
    lib_utf16_to_code(buf, &cp);
    printf("UNI-CP: %08X\n",  (unsigned int) cp);

    return 0;
}

/*

iconv
https://github.com/pillarjs/iconv-lite/tree/master/encodings/tables

Encoding Converter (ENCA)
https://github.com/nijel/enca
https://github.com/nijel/enca/blob/master/lib/unicodemap.c
https://github.com/nijel/enca/blob/master/src/locale_detect.c
https://github.com/nijel/enca/blob/master/src/filebuf.c
https://github.com/nijel/enca/blob/master/src/getopt_long.c
https://github.com/nijel/enca/blob/master/src/options.c

UTF8
https://habr.com/ru/articles/751616/

UTF7 RFC
https://datatracker.ietf.org/doc/html/rfc2152

UTF7 Wiki
https://ru.m.wikipedia.org/wiki/UTF-7

UTF7 VBA
https://www.source-code.biz/snippets/vbasic/Base64Coder.bas.txt

UTF7 ICU
https://github.com/godotengine/godot/blob/master/thirdparty/icu4c/common/ucnv_u7.cpp
https://cocalc.com/github/godotengine/godot/blob/master/thirdparty/icu4c/common/ucnv_u7.cpp
https://unicode-org.github.io/icu/userguide/conversion/converters.html#1-single-string

UTF-7 JavaScript
https://github.com/kkaefer/utf7/blob/master/utf7.js

UTF-7 Mapping Table
https://www.fileformat.info/info/charset/UTF-7/list.htm

https://www.google.com/amp/s/habr.com/ru/amp/publications/835206/

BOCU
https://en.wikipedia.org/wiki/Binary_Ordered_Compression_for_Unicode

*/