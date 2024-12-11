#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "enclib.h"
#include "utf8lib.h"

/**
 * Converts data by Encoding IDs 
 */
char* lib_enc_conv_by_id(int from_id, int to_id, char* data, size_t len, size_t& to_len) {
    if (!data || len == 0) {
        return NULL;
    }    

    bool has_from = lib_unimap_supports_map(from_id);
    bool has_to = lib_unimap_supports_map(to_id);

    if (has_from && has_to) {
        to_len = len;
        char* to_data = (char*) malloc(to_len);
        if (!to_data) {
            // error
            return NULL;
        }
        
        // Copy data
        memcpy(to_data, data, len);

        // Conversion b2b only
        lib_unimap_conv_by_id(from_id, to_id, to_data, to_len);
        return to_data;
    }

    if (has_from && to_id == LIB_ENC_UTF_ID) {
        return lib_enc_conv_to_utf8_by_id(from_id, data, len, to_len);
    }

    // TODO
    return NULL;
}

/**
 * Converts data to UTF-8 by Encoding ID
 */
char* lib_enc_conv_to_utf8_by_id(int from_id, char* data, size_t len, size_t& to_len) {
    if (!data || len == 0) {
        return NULL;
    }
        // Get 'from' map
    lib_unimap_t from_map;
    lib_unimap_get_unimap_by_id(&from_map, from_id);

    return lib_enc_conv_to_utf8_by_map(&from_map , data, len, to_len);
}


static int _to_code(char chr) {
    unsigned char u = (unsigned char) chr;
    return u;
}

/**
 * Converts data to UTF-8 by Encoding Map
 */
char* lib_enc_conv_to_utf8_by_map(lib_unimap_t* from_map , char* data, size_t len, size_t& to_len) {
    if (!from_map || !from_map->map || !data || len == 0) {
        return NULL;
    }

    unsigned char u;
    int icode;
    //int ocode;
    int ucode;
    size_t seq_len;
    size_t total = 0;

    for (int i = 0; i < len; i++) {

        u = _to_code(data[i]);
        icode = u;

        // Get UTF-8 code from conversion map
        ucode = lib_unimap_get_ucode(from_map, icode);
        if (ucode < 0) {
            // error
            return NULL;
        }

        // Calculate sequence lenght of codepoint
        seq_len = lib_utf8_get_codepoint_sequence_len(ucode);
        if (seq_len == 0) {
            // error
            return NULL;
        }

        total += seq_len;

    }

    to_len = total;

    if (to_len == 0) {
        return NULL;
    }

    char* to_data = (char*) malloc(to_len);
    if (!to_data) {
        // error
        return NULL;
    }

    char buf[] = "\0\0\0\0\0"; // buffer to exchange (max size = 4 + 1)
    int j = 0;
    total = 0;

    for (int i = 0; i < len; i++) {

        u = _to_code(data[i]);
        icode = u;

        // Get UTF-8 code from conversion map
        ucode = lib_unimap_get_ucode(from_map, icode);
        if (ucode < 0) {
            // error
            free(to_data);
            return NULL;
        }

        // Convert UTF-8 code to multi byte char
        seq_len = lib_utf8_to_char(buf, ucode);
        if (seq_len <= 0) {
            // error
            free(to_data);
            return NULL;
        }

        // Double check sequence lenght (?)
        if (seq_len != lib_utf8_get_codepoint_sequence_len(ucode)) {
            // error
            free(to_data);
            return NULL;
        }

        // Copy data from buffer to output
        for (int k = 0; k < seq_len; k++) {
            to_data[j + k] = buf[k];
        }

        j += seq_len;
        total += seq_len;
    }

    if (to_len != total) {
        // error
        free(to_data);
        return NULL;
    }
 
    return to_data;
}

