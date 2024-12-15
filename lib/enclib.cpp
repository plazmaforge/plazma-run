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

/**
 * Returns true if the Encoding ID supports conversion
 */
bool lib_enc_supports_conv(int id) {
    if (id == LIB_ENC_UTF8_ID) {
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

    // unimap -> UTF-8
    if (has_from && to_id == LIB_ENC_UTF8_ID) {

        #ifdef DEBUG
        fprintf(stderr, ">> conv_by_id: unimap -> UTF-8\n");
        #endif

        return lib_enc_conv_to_utf8_by_id(from_id, from_data, from_len, to_data, to_len);
    }

    // UTF-8 -> unimap
    if (from_id == LIB_ENC_UTF8_ID && has_to) {

        #ifdef DEBUG
        fprintf(stderr, ">> conv_by_id: UTF-8 -> unimap\n");
        #endif

        return lib_enc_conv_from_utf8_by_id(to_id, from_data, from_len, to_data, to_len);
    }

    // TODO: unimap -> UTF-16
    // TODO: unimap -> UTF-32

    // TODO: UTF-16 -> unimap
    // TODO: UTF-32 -> unimap

    if (!has_from && !has_to) {
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

/**
 * Converts data to UTF-8 by Encoding ID
 */
int lib_enc_conv_to_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    if (!from_data || from_len == 0 || !to_data || !to_len) {
        return -1;
    }

    if (from_len == 0) {
        return 0;
    }

    // Get 'from' map
    lib_unimap_t conv_map;
    lib_unimap_get_unimap_by_id(&conv_map, conv_id);

    return lib_enc_conv_to_utf8_by_map(&conv_map , from_data, from_len, to_data, to_len);
}

int lib_enc_conv_from_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf8_by_id: conv_id=%d\n", conv_id);
    #endif

    if (!from_data || from_len == 0 || !to_data || !to_len) {
        return -1;
    }

    if (from_len == 0) {
        return 0;
    }

    // Get 'conv' map
    lib_unimap_t conv_map;
    lib_unimap_get_unimap_by_id(&conv_map, conv_id);

    return lib_enc_conv_from_utf8_by_map(&conv_map , from_data, from_len, to_data, to_len);
}

static int _to_code(char chr) {
    unsigned char u = (unsigned char) chr;
    return u;
}

/**
 * Converts data to UTF-8 by Encoding Map
 */
int lib_enc_conv_to_utf8_by_map(lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_to_utf8_by_map: conv_id=%d, from_len=%lu\n", (conv_map ? conv_map->id : 0), from_len);
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
    size_t seq_len = 0;
    size_t total   = 0;

    char* data     = from_data;
    int i          = 0;
    int j          = 0;

    while (i < from_len) {

        icode = _to_code(*data);

        // Convert char to  UTF-8 codepoint
        ucode = lib_unimap_conv_icode(conv_map, icode);
        if (ucode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid UTF-8 codepoint: ucode=%d\n", ucode);
            #endif
            return -1;
        }

        // Calculate sequence lenght of codepoint
        seq_len = lib_utf8_get_codepoint_sequence_len(ucode);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v1=%lu\n", seq_len);
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

    new_data = _data_new(new_len);
    if (!new_data) {
        // error
        return -1;
    }

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    data       = from_data;
    i          = 0;
    j          = 0;
    total      = 0;

    while (i < from_len) {

        icode = _to_code(*data);

        // Convert char to UTF-8 codepoint
        ucode = lib_unimap_conv_icode(conv_map, icode);
        if (ucode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid UTF-8 codepoint: ucode=%d\n", ucode);
            #endif
            free(new_data);
            return -1;
        }

        // Convert UTF-8 codepoint to multi byte char
        seq_len = lib_utf8_to_char(buf, ucode);
        if (seq_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v2=%lu\n", seq_len);
            #endif
            free(new_data);
            return -1;
        }

        // Double check sequence lenght (?)
        if (seq_len != lib_utf8_get_codepoint_sequence_len(ucode)) {
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
    *to_len  = new_len;

    return 0;
}

/**
 * Converts data from UTF-8 by Encoding Map
 */
int lib_enc_conv_from_utf8_by_map(lib_unimap_t* conv_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> conv_from_utf8_by_map: conv_id=%d, from_len=%lu\n", (conv_map ? conv_map->id : 0), from_len);
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
    size_t seq_len = 0;
    size_t total   = 0;
    
    char c         = '\0';
    char* data     = from_data;
    int i          = 0;
    int j          = 0;

    while (i < from_len) {

        c = *data;

        // Calculate sequence lenght of UTF-8 char
        seq_len = lib_utf8_get_byte_sequence_len(c);
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

        c = *data;

        // Calculate sequence lenght of UTF-8 char
        seq_len = lib_utf8_get_byte_sequence_len(c);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: Invalid Sequence: seq_len_v2=%lu\n", seq_len);
            #endif
            free(new_data);
            return -1;
        }

        // Convert current UTF-8 char to codepoint
        int cp_len = lib_utf8_to_codepoint(data, &ucode);
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

        // Convert UTF-8 codepoint to code by conv_map
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
