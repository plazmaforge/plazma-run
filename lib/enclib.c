#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "enclib.h"
#include "utf8lib.h"

// #define DEBUG    1
// #define DEBUG_LL 1
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

    // unimap <-> unimap
    if (has_from && has_to) {

        // Copy data
        char* data = _data_copy(from_data, from_len);
        if (!data) {
            // error
            return -1;
        }
        
        // Conversion b2b only
        int retval = lib_unimap_conv_by_id(from_id, to_id, data, from_len);

        *to_data = data;
        *to_len  = from_len;

        return retval;
    }

    // unimap -> UTF-[ID]
    if (has_from && lib_enc_supports_utf_conv(to_id)) {

        #ifdef DEBUG
        fprintf(stderr, ">> conv_by_id: unimap -> UTF-[ID]\n");
        #endif

        return lib_enc_conv_to_utf_by_id(to_id, from_id, from_data, from_len, to_data, to_len);
    }

    // UTF-[ID] -> unimap
    if (has_to && lib_enc_supports_utf_conv(from_id)) {

        #ifdef DEBUG
        fprintf(stderr, ">> conv_by_id: UTF-[ID] -> unimap\n");
        #endif

        return lib_enc_conv_from_utf_by_id(from_id, to_id, from_data, from_len, to_data, to_len);
    }

    if (!has_from && !has_to) {

        // TODO: STUB
        if (lib_enc_supports_utf_conv(from_id) && to_id == LIB_ENC_UTF7_ID) {
            return lib_enc_conv_to_utf7(from_id, from_data, from_len, to_data, to_len);
        }

        // TODO: STUB
        if (from_id == LIB_ENC_UTF7_ID && lib_enc_supports_utf_conv(to_id)) {
            #ifdef ERROR
            fprintf(stderr, "Conversion from %d unsupported\n", from_id);
            #endif
            return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
        }

        if (lib_enc_supports_utf_conv(from_id) && lib_enc_supports_utf_conv(to_id)) {
            return lib_enc_conv_utf2utf(from_id, to_id, from_data, from_len, to_data, to_len);
        }

        #ifdef ERROR
        fprintf(stderr, "Conversion from/to map unsupported: %d, %d\n", from_id, to_id);
        #endif
        return LIB_ENC_ERR_CONV_USUPPORTED;
    }

    if (!has_from) {
        #ifdef ERROR
        fprintf(stderr, "Conversion from map unsupported   : %d\n", from_id);
        #endif
        return LIB_ENC_ERR_CONV_FROM_USUPPORTED;
    }

    if (!has_to) {
        #ifdef ERROR
        fprintf(stderr, "Conversion to map unsupported     : %d\n", to_id);
        #endif
        return LIB_ENC_ERR_CONV_TO_USUPPORTED;
    }

    return LIB_ENC_ERR_CONV_USUPPORTED;
}

static int _to_code(char chr) {
    unsigned char u = (unsigned char) chr;
    return u;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// UTF-8/UTF-8-BOM/UTF-16/UTF-16BE/UTF-16LE/UTF-32/UTF-32BE/UTF-32LE
/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Converts data to UTF-[ID] by Encoding ID
 */
int lib_enc_conv_to_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf_by_id: unimap -> UTF-[ID]\n");
    #endif

    if (!from_data || from_len == 0 || !to_data || !to_len) {
        return -1;
    }

    if (from_len == 0) {
        return 0;
    }

    // Get 'from' map
    struct lib_unimap_t conv_map;
    lib_unimap_get_unimap_by_id(&conv_map, conv_id);

    return lib_enc_conv_to_utf_by_map(utf_id, &conv_map, from_data, from_len, to_data, to_len);
}

/**
 * Converts data from UTF-[ID] by Encoding ID
 */
int lib_enc_conv_from_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf_by_id: conv_id=%d\n", conv_id);
    #endif

    if (!from_data || from_len == 0 || !to_data || !to_len) {
        return -1;
    }

    if (from_len == 0) {
        return 0;
    }

    // Get 'conv' map
    struct lib_unimap_t conv_map;
    lib_unimap_get_unimap_by_id(&conv_map, conv_id);

    return lib_enc_conv_from_utf_by_map(utf_id, &conv_map, from_data, from_len, to_data, to_len);
}

////

/**
 * Converts data to UTF-[ID] by Encoding Map
 */
int lib_enc_conv_to_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf_by_map: conv_id=%d, from_len=%lu\n", (conv_map ? conv_map->id : 0), from_len);
    #endif

    if (to_data) {
        *to_data = NULL;
    }
    if (to_len) {
        *to_len = 0;
    }

    if (!conv_map || !conv_map->map || !from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

    int icode;
    int ucode;

    char* new_data = NULL;
    size_t new_len = 0;
    size_t bom_len = 0;
    size_t seq_len = 0;
    size_t total   = 0;

    char* data     = from_data;
    int i          = 0;
    int j          = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf_by_map: start\n");
    #endif

    bom_len = lib_enc_bom_len(utf_id);

    while (i < from_len) {

        icode = _to_code(*data);

        // Convert char to  Unicode codepoint
        ucode = lib_unimap_conv_icode(conv_map, icode);
        if (ucode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid UTF-[ID] codepoint: ucode=%d\n", ucode);
            #endif
            return -1;
        }

        // Calculate sequence lenght of codepoint
        seq_len = lib_utf_code_seq_len(utf_id, ucode);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR-888: Invalid Sequence: seq_len_v1=%lu\n", seq_len);
            #endif
            return -1;
        }
        data++;
        i++;
        total += seq_len;
    }

    new_len = total;
    if (new_len == 0) {
        // error
        return -1;
    }

    new_data = _data_new(new_len + bom_len);
    if (!new_data) {
        // error
        return -1;
    }

    lib_enc_set_bom(utf_id, new_data);

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    data       = from_data;
    i          = 0;
    j          = bom_len;
    total      = 0;

    while (i < from_len) {

        icode = _to_code(*data);

        // Convert char to Unicode codepoint
        ucode = lib_unimap_conv_icode(conv_map, icode);
        if (ucode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid UTF-[ID] codepoint: ucode=%d\n", ucode);
            #endif
            free(new_data);
            return -1;
        }

        // Convert Unicode codepoint to multi byte char
        seq_len = lib_utf_to_char(utf_id, buf, ucode);
        if (seq_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v2=%lu\n", seq_len);
            #endif
            free(new_data);
            return -1;
        }

        // Double check sequence lenght (?)
        if (seq_len != lib_utf_code_seq_len(utf_id, ucode)) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len != cp_len\n");
            #endif
            free(new_data);
            return -1;
        }

        // Copy data from buffer to output
        for (int k = 0; k < seq_len; k++) {
            new_data[j + k] = buf[k];
        }

        data++;
        i++;
        j += seq_len;
        total += seq_len;
    }

    if (new_len != total) {
        // error
        free(new_data);
        return -1;
    }

    *to_data = new_data;
    *to_len  = new_len + bom_len;

    return 0;
}

/**
 * Converts data from UTF-[ID] by Encoding Map
 */
int lib_enc_conv_from_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf_by_map: conv_id=%d, from_len=%lu\n", (conv_map ? conv_map->id : 0), from_len);
    #endif

    if (to_data) {
        *to_data = NULL;
    }
    if (to_len) {
        *to_len = 0;
    }

    if (!conv_map || !conv_map->map || !from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

    int ocode;
    int ucode;

    char* new_data = NULL;
    size_t new_len = 0;
    size_t bom_len = lib_enc_bom_len(utf_id);
    size_t seq_len = 0;
    size_t total   = 0;
    
    char* data     = from_data;
    int i          = bom_len;
    int j          = 0;

    while (i < from_len) {

        // Calculate sequence lenght of UTF-[ID] char
        seq_len = lib_utf_char_seq_len(utf_id, data);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v1=%lu\n", seq_len);
            #endif
            return -1;
        }

        data += seq_len;
        i += seq_len;
        total++;
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

    new_data = _data_new(new_len);
    if (!new_data) {
        // error
        return -1;
    }


    data    = from_data;
    i       = 0;
    j       = 0;
    seq_len = 0;
    total   = 0;
        
    while (i < from_len) {

        // Calculate sequence lenght of UTF-[ID] char
        seq_len = lib_utf_char_seq_len(utf_id, data);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v2=%lu\n", seq_len);
            #endif
            free(new_data);
            return -1;
        }

        // Convert current UTF-[ID] char to codepoint
        int cp_len = lib_utf_to_code(utf_id, data, &ucode);
        if (cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: cp_len_v2=%d\n", cp_len);
            #endif
            free(new_data);
            return -1;
        }

        // Double check sequence lenght (?)
        if (seq_len != cp_len) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len != cp_len\n");
            #endif
            free(new_data);
            return -1;
        }

        // Convert UTF-[ID] codepoint to code by conv_map
        if (ucode < conv_map->start) {
            ocode = ucode; 
        } else {
            ocode = lib_unimap_conv_ucode(conv_map, ucode);
        }
        
        if (ocode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid char: ocode=%d\n", ocode);
            #endif
            free(new_data);
            return -1;
        }

        // Set output char
        new_data[j] = ocode;
        //*new_data = ocode;

        data += seq_len;
        i += seq_len;
        //new_data++;
        j++;
        total++;
    }

    if (new_len != total) {
        // error
        free(new_data);
        return -1;
    }

    *to_data = new_data;
    *to_len  = new_len;

    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Converts data from UTF-[ID] to UTF-[ID]
 */
int lib_enc_conv_utf2utf(int from_id, int to_id, char* from_data, size_t from_len, char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_utf2utf: from_id=%d, to_id=%d, len=%lu\n", from_id, to_id, from_len);
    #endif

    if (to_data) {
        *to_data = NULL;
    }
    if (to_len) {
        *to_len = 0;
    }

    if (!from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

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
    fprintf(stderr, ">> conv_utf2utf: Starting...\n");
    #endif

    #ifdef ERROR
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

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
            free(new_data);
            return -1;
        }

        to_seq_len = lib_utf_code_seq_len(to_id, ucode);
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
            free(new_data);
            return -1;
        }

        // Convert Unicode codepoint to multi byte char
        to_seq_len = lib_utf_to_char(to_id, buf, ucode);
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
    fprintf(stderr, ">> conv_utf2utf: SUCCESS\n");
    #endif

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

// Calculate size of output base64 array by input size
size_t to_base64_size(size_t size) {
    return (size * 4 + 2) / 3;
}

// flush utf7 block  to prepared (non utf7) byte array (dtata)
// count is unicode point count (!)
// bdata is shared for all data buffer by max count * 2 (hi/lo)

int to_utf7_block(int from_id, char* idata, char* bdata, size_t bsize, size_t count) {

    int ucode  = 0;
    uint8_t hi = 0;
    uint8_t lo = 0;
    size_t i   = 0;
    size_t j   = 0;

    size_t from_seq_len = 0;

    // char* data = idata + start; // why i can shift idata before call
    char* data = idata;
    while (i < count) {

        //ucode = data[i]; // stub
        // seq_len = ...

        // Calculate input sequence lenght of UTF-[ID] char
        from_seq_len = lib_utf_char_seq_len(from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            //free(new_data);
            //free(bin_data);
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
            //free(new_data);
            //free(bin_data);
            return -1;
        }

        // unicode point to hi and lo
        hi = ucode >> 8;
        lo = ucode & 0xFF;

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

// TODO: stub

//static const int map1[] = {};

//static const int map2[] = {};

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

    char i0, i1, i2 = 0;
    char j0, j1, j2, j3 = 0;

    while (i < isize) {
        i0 = idata[i];
        i++;

        if (i < isize) {
            i1 = idata[i];
            i++;
        } else {
            i1 = 0;
        }

        if (i < isize) {
            i2 = idata[i];
            i++;
        } else {
            i2 = 0;
        }

        ////

        j0 = i0 / 4;
        j1 = ((i0 & 3) * 0x10) | (i1 / 0x10);
        j2 = ((i1 & 0x0F) * 4) | (i2 / 0x40);
        j3 = i2 & 0x3F;

        odata[j] = map1[j0];
        j++;
        odata[j] = map1[j1];
        j++;
        if (j < dsize) {
            odata[j] = map1[j2];
        } else {
            // odata[j] = '=';
        }
        j++;
        if (j < dsize) {
            odata[j] = map1[j3];
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

/**
 * Converts data to UTF-7
 */
int lib_enc_conv_to_utf7(int from_id, char* from_data, size_t from_len, char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: from_id=%d, len=%lu\n", from_id, from_len);
    #endif

    int to_id = LIB_ENC_UTF7_ID;

    if (to_data) {
        *to_data = NULL;
    }
    if (to_len) {
        *to_len = 0;
    }

    if (!from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

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
    int j          = 0;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf7: Starting...\n");
    #endif

    #ifdef ERROR
    fprintf(stderr, "DEBUG: from_bom_len=%lu, to_bom_len=%lu\n", from_bom_len, to_bom_len);
    #endif

    bool start_block     = false; // Start UTF7 block flag
    size_t start_index   = 0;     // Start index in UTF7 block
    size_t block_count   = 0;     // Count of Unicode points in UTF7 block
    size_t block_bin_len = 0;     // Size of binary block of Unicode point pairs (hi/lo): block_count * 2
    size_t block_b64_len = 0;     // Size of base64 block
    size_t bin_len       = 0;     // Max size of binary block
    char* bin_data       = NULL;  // Binary block data
    char* out_data       = NULL;
    char* cur_data       = NULL;

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

        // to_seq_len = lib_utf_code_seq_len(to_id, ucode);
        // if (to_seq_len <= 0) {
        //     // error
        //     #ifdef ERROR
        //     fprintf(stderr, "ERROR: Invalid Sequence: to_seq_len=%lu\n", to_seq_len);
        //     #endif
        //     return -1;
        // }

        // #ifdef DEBUG_LL
        // fprintf(stderr, "DEBUG: >> 1: to_seq_len=%lu\n", to_seq_len);
        // #endif

        /////////////////////////////////////////////////////////////

        if (ucode <= 127) {

            if (start_block) {
                block_bin_len = block_count * 2; // hi, lo
                if (block_bin_len > bin_len) {
                    bin_len = block_bin_len;
                }

                total++;     // +
                block_b64_len = to_base64_size(block_bin_len);

                // Calc UTF7 block
                total +=  block_b64_len;

                if (i + from_seq_len < from_len) {
                    total++; // -
                }

                // End UF7 block
                start_block = false;
                block_count = 0;
            }

            total++;     // Calc ASCII
            if (ucode == '+') {
                total++; // Calc "+-"
            }

        } else if (ucode > 127) {
            if (!start_block) {

                // Start UTF7 block
                start_block = true;
                start_index = i;
            }
            block_count++;
        }

        /////////////////////////////////////////////////////////////

        data += from_seq_len;
        i += from_seq_len;
        //total += to_seq_len;
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
        block_bin_len = block_count * 2; // hi, lo
        if (block_bin_len > bin_len) {
            bin_len = block_bin_len;
        }

        total++;     // '+'
        block_b64_len = to_base64_size(block_bin_len);

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

    if (bin_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: bin_len == 0\n");
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

    bin_data = _data_new(bin_len);
    if (!bin_data) {
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
    j            = to_bom_len;
    from_seq_len = 0;
    //to_seq_len   = 0;
    total        = 0;
    out_data     = new_data;
        
    while (i < from_len) {

        // Calculate input sequence lenght of UTF-[ID] char
        from_seq_len = lib_utf_char_seq_len(from_id, data);
        if (from_seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: from_seq_len_v1=%lu\n", from_seq_len);
            #endif
            free(new_data);
            free(bin_data);
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
            free(bin_data);
            return -1;
        }

        // Convert Unicode codepoint to multi byte char
        // to_seq_len = lib_utf_to_char(to_id, buf, ucode);
        // if (to_seq_len <= 0) {
        //     // error
        //     #ifdef ERROR
        //     fprintf(stderr, "ERROR: Invalid Sequence: to_seq_len_v2=%lu\n", to_seq_len);
        //     #endif
        //     free(new_data);
        //     free(bin_data);
        //     return -1;
        // }

        // #ifdef DEBUG_LL
        // fprintf(stderr, "DEBUG: >> 2: to_seq_len=%lu\n", to_seq_len);
        // #endif

        // Copy data from buffer to output
        // for (int k = 0; k < to_seq_len; k++) {
        //     new_data[j + k] = buf[k];
        // }

        /////////////////////////////////////////////////////////////

        if (ucode <= 127) {

            if (start_block) {
                block_bin_len = block_count * 2; // hi, lo
                block_b64_len = to_base64_size(block_bin_len);

                // output to bin_data (hi, lo)
                to_utf7_block(from_id, cur_data, bin_data, block_bin_len, block_count);

                *out_data = '+';
                out_data++;
                total++;     // +

                // flush UTF7 block
                base64_encode(bin_data, block_bin_len, out_data, block_b64_len);
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
                block_count = 0;
            }

            *out_data = (char) ucode;
            out_data++;
            total++;     // Calc ASCII

            if (ucode == '+') {

                *out_data = '-';
                out_data++;
                total++; // Calc "+-"
            }

        } else if (ucode > 127) {
            if (!start_block) {

                // Start UTF7 block
                start_block = true;
                cur_data = data;
            }
            block_count++;
        }

        /////////////////////////////////////////////////////////////

        data += from_seq_len;
        i += from_seq_len;

        // j += to_seq_len;
        // total += to_seq_len;

    }

    if (start_block) {
        block_bin_len = block_count * 2; // hi, lo
        block_b64_len = to_base64_size(block_bin_len);

        // output to bin_data (hi, lo)
        to_utf7_block(from_id, cur_data, bin_data, block_bin_len, block_count);

        *out_data = '+';
        out_data++;
        total++;     // +

        // flush UTF7 block
        base64_encode(bin_data, block_bin_len, out_data, block_b64_len);
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
        block_count = 0;
    }

    if (new_len != total) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: Incorrect len: new_len=%lu, total=%lu\n", new_len, total);
        #endif
        free(new_data);
        free(bin_data);
        return -1;
    }

    *to_data = new_data;
    *to_len  = new_len + to_bom_len;

    #ifdef DEBUG
    fprintf(stderr, ">> conv_utf2utf: SUCCESS\n");
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
