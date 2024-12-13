#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "enclib.h"
#include "utf8lib.h"

//#define DEBUG    1
//#define DEBUG_LL 1
//#define ERROR    1

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
    fprintf(stderr, ">> lib_enc_conv_by_id: from_id=%d, to_id=%d\n", from_id, to_id);
    #endif

    if (!from_data || !to_data || !to_len) {
        return -1;
    }    

    if (from_len == 0) {
        return 0;
    }    

    *to_data = NULL;
    *to_len  = 0;

    bool has_from = lib_unimap_supports_map(from_id);
    bool has_to   = lib_unimap_supports_map(to_id);

    // unimap <-> unimap
    if (has_from && has_to) {
        size_t len = from_len;
        char* data = (char*) malloc(len);
        if (!data) {
            // error
            return -1;
        }
        
        // Copy data
        memcpy(data, from_data, len);

        // Conversion b2b only
        int ret = lib_unimap_conv_by_id(from_id, to_id, data, len);

        *to_data = data;
        *to_len  = len;

        return ret;
    }

    // unimap -> UTF-8
    if (has_from && to_id == LIB_ENC_UTF8_ID) {

        #ifdef DEBUG
        fprintf(stderr, ">> lib_enc_conv_by_id: unimap -> UTF-8\n");
        #endif

        return lib_enc_conv_to_utf8_by_id(from_id, from_data, from_len, to_data, to_len);
    }

    // UTF-8 -> unimap
    if (from_id == LIB_ENC_UTF8_ID && has_to) {

        #ifdef DEBUG
        fprintf(stderr, ">> lib_enc_conv_by_id: UTF-8 -> unimap\n");
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
int lib_enc_conv_to_utf8_by_id(int from_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    if (!from_data || from_len == 0 || !to_data || !to_len) {
        return -1;
    }

    if (from_len == 0) {
        return 0;
    }

    // Get 'from' map
    lib_unimap_t from_map;
    lib_unimap_get_unimap_by_id(&from_map, from_id);

    return lib_enc_conv_to_utf8_by_map(&from_map , from_data, from_len, to_data, to_len);
}

int lib_enc_conv_from_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    #ifdef DEBUG
    fprintf(stderr, ">> lib_enc_conv_from_utf8_by_id: conv_id=%d\n", conv_id);
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
int lib_enc_conv_to_utf8_by_map(lib_unimap_t* from_map, char* from_data, size_t from_len, 
    char** to_data, size_t* to_len) {

    if (!from_map || !from_map->map || !from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

    *to_data = NULL;
    *to_len = 0;

    int icode;
    int ucode;

    char* new_data = NULL;
    size_t new_len = 0;
    size_t seq_len = 0;
    size_t total   = 0;

    for (int i = 0; i < from_len; i++) {

        icode = _to_code(from_data[i]);

        // Get UTF-8 code from conversion map
        ucode = lib_unimap_get_ucode(from_map, icode);
        if (ucode < 0) {
            // error
            return -1;
        }

        // Calculate sequence lenght of codepoint
        seq_len = lib_utf8_get_codepoint_sequence_len(ucode);
        if (seq_len == 0) {
            // error
            return -1;
        }

        total += seq_len;
    }

    new_len = total;
    if (new_len == 0) {
        // error
        return -1;
    }

    new_data = (char*) malloc(new_len);
    if (!new_data) {
        // error
        return -1;
    }

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    int j = 0;
    total = 0;

    for (int i = 0; i < from_len; i++) {

        icode = _to_code(from_data[i]);

        // Get UTF-8 code from conversion map
        ucode = lib_unimap_get_ucode(from_map, icode);
        if (ucode < 0) {
            // error
            free(new_data);
            return -1;
        }

        // Convert UTF-8 code to multi byte char
        seq_len = lib_utf8_to_char(buf, ucode);
        if (seq_len <= 0) {
            // error
            free(new_data);
            return -1;
        }

        // Double check sequence lenght (?)
        if (seq_len != lib_utf8_get_codepoint_sequence_len(ucode)) {
            // error
            free(new_data);
            return -1;
        }

        // Copy data from buffer to output
        for (int k = 0; k < seq_len; k++) {
            new_data[j + k] = buf[k];
        }

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
    fprintf(stderr, ">> lib_enc_conv_from_utf8_by_map: conv_id=%d\n", conv_map ? conv_map->id : 0);
    fprintf(stderr, ">> lib_enc_conv_from_utf8_by_map: from_len=%lu\n", from_len);
    #endif

    if (!conv_map || !conv_map->map || !from_data || !to_data || !to_len) {
        return -1;
    }
    if (from_len == 0) {
        return 0;
    }

    *to_data = NULL;
    *to_len = 0;

    int ocode;
    int ucode;

    char* new_data = NULL;
    size_t new_len = 0;
    size_t seq_len = 0;
    size_t total   = 0;

    int i = 0;
    int k = 0;
    char c = '\0';

    for (i = 0; i < from_len; i += seq_len) {

        // Calculate sequence lenght of char
        c = from_data[i];
        seq_len = lib_utf8_get_byte_sequence_len(c);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: seq_len-1: %lu\n", seq_len);
            #endif
            return -1;
        }

        //i += seq_len;
        total++;
    }

    // if (k != from_len) {
    //     // error
    //     #ifdef ERROR
    //     fprintf(stderr, "ERROR: k != from_len\n");
    //     #endif
    //     return -1;
    // }

    new_len = total;
    if (new_len == 0) {
        // error
        #ifdef ERROR
        fprintf(stderr, "ERROR: new_len == 0\n");
        #endif
        return -1;
    }

    new_data = (char*) malloc(new_len);
    if (!new_data) {
        // error
        return -1;
    }

    int j   = 0;
    seq_len = 0;
    total   = 0;

    i = 0;
    k = 0;

    char* data = from_data;

    for (i = 0; i < from_len; i += seq_len) {

        c = *data;
        seq_len = lib_utf8_get_byte_sequence_len(c);
        if (seq_len == 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: seq_len-2: %lu\n", seq_len);
            #endif
            free(new_data);
            return -1;
        }

        int cp_len = lib_utf8_to_codepoint(data, &ucode);
        if (cp_len <= 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: cp_len-2: %d\n", cp_len);
            #endif
            free(new_data);
            return -1;
        }

        // Double check sequence lenght (?)
        if (seq_len != cp_len) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: seq_len != cp_len\n");
            #endif
            free(new_data);
            return -1;
        }

        // Convert UTF-8 code to code by conv_map
        if (ucode < conv_map->start) {
            ocode = ucode; 
        } else {
            ocode = lib_unimap_conv_ucode(conv_map, ucode);
        }
        
        if (ocode < 0) {
            // error
            #ifdef ERROR
            fprintf(stderr, "ERROR: ocode=%d\n", ocode);
            #endif
            free(new_data);
            return -1;
        }

        // Copy data from buffer to output
        new_data[j] = ocode;

        data += seq_len;
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
