#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base32.h"

static const char base32u[]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
static const char base32l[]   = "abcdefghijklmmopqrstuvwxyz234567";
static const char zbase32[]   = "ybndrfg8ejkmcpqxot1uwisza345h769";
static const char base32hex[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

static const char* base32_table_type(int type) {
    if (type == LIB_BASE32 || type == LIB_BASE32U) {
        return base32u;
    }
    if (type == LIB_BASE32L) {
        return base32l;
    }
    if (type == LIB_ZBASE32) {
        return zbase32;
    }
    if (type == LIB_BASE32HEX) {
        return base32hex;
    }
    return base32u;
}

static bool base32_non_print(char c) {
    return (c >= 0 && c <= 32);
}

static bool base32_is_spec(int type, char c) {
    // Z-Base32 doesn't support '='
    return ((type != LIB_ZBASE32 && c == '=') || base32_non_print(c));
}

static bool base32_find(const char* encode_table, char c) {
    char* s = (char*) encode_table;
    for (size_t i = 0; i < 32; i++) {
        if (c == *s) {
            return true;
        }
        s++;
    }
    return false;
}

static bool base32_is_valid_char(int type, const char* encode_table, char c) {
    return base32_is_spec(type, c) || base32_find(encode_table, c);
}

static bool base32_is_valid(int type, const char* encode_table, const char *src, size_t len) {
    char* s = (char*) src;
    for (size_t i = 0; i < len; i++) {
        if (!base32_is_valid_char(type, encode_table, *s)) {
            return false;
        }
        s++;
    }
    return true;
}

static size_t base32_encode_len_bin(size_t len) {
	return 8 * len;
}

static size_t base32_encode_len_type(int type, size_t len) {
	size_t bin_len = base32_encode_len_bin(len);
    size_t padding = bin_len % 5;
    if (padding != 0) {
        bin_len = bin_len + (5 - padding);
    }

    // calculate iterations with step 5
    size_t out_len = bin_len / 5;

    if (type == LIB_ZBASE32) {
        return out_len;
    }

    if (out_len % 8 != 0) {
        size_t count = out_len / 8;
        count++;
        out_len = count * 8;
    }

    return out_len;
}

static size_t base32_encode_len(size_t len) {
    return base32_encode_len_type(LIB_BASE32, len);
}

static char to_bin(char c, int i) {
    return ( (c & (1 << i)) ? '1' : '0' );
}

static void to_bin_count(char* buf, char c, int count) {
    int n = count - 1;
    for (int i = n; i >= 0; --i) {
        buf[n - i] = to_bin(c, i);
    }
    //fprintf(stderr, "buf = %s\n", buf);
}

static void to_bin8(char* buf, char c) {
    to_bin_count(buf, c, 8);
}

static void to_bin5(char* buf, char c) {
    to_bin_count(buf, c, 5);
}

ssize_t base32_encode_type(int type, char* out, const unsigned char* src, size_t len) {
    if (!out || !src) {
        return -1;
    }
    if (len == 0) {
        return 0;
    }

    const char* encode_table = base32_table_type(type);

    //fprintf(stderr, "src_len = %lu\n", len);
    size_t bin_len = 8 * len;
    size_t padding = bin_len % 5;
    size_t add_len = 0;
    if (padding != 0) {
        add_len = 5 - padding;
        bin_len = bin_len + add_len;
    }

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "add_len = %lu\n", add_len);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);

    char bin_str[bin_len + 1];
    bin_str[0] = '\0';

    for (size_t i = 0; i < len; i++) {
        char byte_str[9];
        //sprintf(byte_string, "%08b", src[i]);
        to_bin8(byte_str, src[i]);
        strcat(bin_str, byte_str);
    }

    if (padding != 0) {
        //strcat(bin_str, "00000" + padding);
        char* add_str = bin_str + (8 * len);
        memset(add_str, '0', add_len);
    }
    bin_str[bin_len] = '\0';

    //fprintf(stderr, "bin_str = %s\n", bin_str);
    //fprintf(stderr, "str_len = %lu\n", strlen(bin_str));

    ssize_t out_len = 0;
    for (size_t i = 0; i < bin_len; i += 5) {
    
        char segment[6];
        strncpy(segment, bin_str + i, 5);
        segment[5] = '\0';
        int index = (int) strtol(segment, NULL, 2);
        strncat(out, &encode_table[index], 1);
        out_len++;
    }

    //fprintf(stderr, "out_len = %lu\n", out_len);
    add_len = 0;
    if (type != LIB_ZBASE32 && (out_len % 8) != 0) {
        size_t count = out_len / 8;
        count++;
        size_t out_len2 = count * 8;
        add_len = out_len2 - out_len;
        memset(out + out_len, '=', add_len);
        out_len += add_len;
    }

    //fprintf(stderr, "add_len = %lu\n", add_len);
    //fprintf(stderr, "out_len = %lu\n", out_len);

    return out_len;
}

static ssize_t base32_encode(char* out, const unsigned char* src, size_t len) {
    return base32_encode_type(LIB_BASE32, out, src, len);
}

static size_t base32_decode_len_bin(size_t len) {
	return 5 * len;
}

static size_t base32_decode_len(size_t len) {
	size_t out_len = base32_decode_len_bin(len);

    // calculate iterations with step 8
    return out_len / 8;
}

ssize_t base32_decode_type(int type, unsigned char* out, const char *src, size_t len) {
    if (!out || !src) {
        return -1;
    }
    if (len == 0) {
        return 0;
    }

    const char* encode_table = base32_table_type(type);

    //fprintf(stderr, "src_len = %lu\n", len);
    size_t bin_len = 5 * len;
    size_t padding = bin_len % 8;

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);

    char bin_str[bin_len + 1];
    bin_str[0] = '\0';

    if (padding != 0) {
        bin_len = bin_len - padding;
        //fprintf(stderr, "bin_len = %lu\n", bin_len);
    }

    for (size_t i = 0; i < len; i++) {
        if (src[i] != '=') {
            char ind_str[6];
            //sprintf(index_string, "%05b", (int) (strchr(encode_table, src[i]) - encode_table));
            to_bin5(ind_str, (int) (strchr(encode_table, src[i]) - encode_table));
            strcat(bin_str, ind_str);
        }
    }

    bin_str[bin_len] = '\0';

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);
    //fprintf(stderr, "str_len = %lu\n", strlen(bin_str));

    size_t out_len = 0;
    for (size_t i = 0; i < bin_len; i += 8) {
        char byte_str[9];
        strncpy(byte_str, bin_str + i, 8);
        byte_str[8] = '\0';
        out[i / 8] = (unsigned char) strtol(byte_str, NULL, 2);
        out_len++;
    }
    //fprintf(stderr, "out_len = %lu\n", out_len);    
    return out_len;
}

ssize_t base32_decode(unsigned char* out, const char *src, size_t len) {
    return base32_decode_type(LIB_BASE32, out, src, len);
}

////

char* lib_base32_encode_type(int type, const char *src, size_t len, size_t* out_len) {
    if (!src || !out_len || len == 0) {
        return NULL;
    }
	size_t olen = base32_encode_len_type(type, len);
    //fprintf(stderr, "olen  : %lu\n", olen);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base32_encode_type(type, out, (unsigned char*) src, len);
    //fprintf(stderr, "retval: %ld\n", retval);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

char* lib_base32_encode(const char *src, size_t len, size_t* out_len) {
    return lib_base32_encode_type(LIB_BASE32, src, len, out_len);
}

char* lib_base32_decode_type(int type, const char *src, size_t len, size_t* out_len) {
    if (!src || !out_len || len == 0) {
        return NULL;
    }

    // Validate chars
    if (!base32_is_valid(type, base32_table_type(type), src, len)) {
        return NULL;
    }
    
    size_t olen = base32_decode_len(len);
    //fprintf(stderr, "olen  : %lu\n", olen);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base32_decode_type(type, (unsigned char*) out, src, len);
    //fprintf(stderr, "retval: %ld\n", retval);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

char* lib_base32_decode(const char *src, size_t len, size_t* out_len) {
    return lib_base32_decode_type(LIB_BASE32, src, len, out_len);
}

// https://medium.com/@at.kishor.k/demystifying-base32-an-in-depth-guide-to-this-encoding-standard-697b9426fc25

// Base32, Z-Base32, Base32Hex, Crockford’s Base32
// https://base32encode.com/blog/base32-alternatives/

// https://www.npmjs.com/package/crockford-base32
// https://github.com/dflydev/dflydev-base32-crockford/blob/master/src/Dflydev/Base32/Crockford/Crockford.php