

/**
 * Converts data to UTF-8 by Encoding ID
 */
//int lib_enc_conv_to_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

//int lib_enc_conv_from_utf8_by_id(int conv_id, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-8 by Encoding Map
 */
//int lib_enc_conv_to_utf8_by_map(struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-8 by Encoding Map
 */
//int lib_enc_conv_from_utf8_by_map(struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

//// UTF

//int lib_enc_conv_to_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);


/**
 * Converts data from UTF-[ID] by Encoding Map
 */
//int lib_enc_conv_from_utf_by_map(int utf_id, struct lib_unimap_t* conv_map, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

/**
 * Converts data to UTF-[ID] by Encoding ID
 */
//int lib_enc_conv_to_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-[ID] by Encoding ID
 */
//int lib_enc_conv_from_utf_by_id(int utf_id, int conv_id, char* from_data, size_t from_len, 
//    char** to_data, size_t* to_len);

/**
 * Converts data from UTF-[ID] to UTF-[ID]
 */
//int lib_enc_conv_utf2utf(int from_id, int to_id, char* from_data, size_t from_len, char** to_data, size_t* to_len);

/**
 * Converts data to UTF-7
 */
//int lib_enc_conv_to_utf7(int from_id, char* from_data, size_t from_len, char** to_data, size_t* to_len);



    /*
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
    */


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
