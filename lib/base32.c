#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base32.h"


static const char encode_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

static size_t base32_encode_len_bin(size_t len) {
	return 8 * len;
}

static size_t base32_encode_len(size_t len) {
	size_t bin_len = base32_encode_len_bin(len);
    size_t padding = bin_len % 5;
    if (padding != 0) {
        bin_len = bin_len + (5 - padding);
    }

    // calculate iterations with step 5
    size_t out_len = bin_len / 5;

    // padding = out_len % 8;
    // if (padding != 0) {
    //     out_len = out_len / 8;
    //     out_len++;
    //     out_len = out_len * 8;
    // }

    if (out_len % 8 != 0) {
        size_t count = out_len / 8;
        count++;
        out_len = count * 8;
    }

    return out_len;
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

static void base32_encode(char* out, const unsigned char* src, size_t len) {

    //fprintf(stderr, "src_len = %lu\n", len);
    size_t bin_len = 8 * len;
    size_t padding = bin_len % 5;
    size_t alg_len = 0;
    if (padding != 0) {
        alg_len = 5 - padding;
        bin_len = bin_len + alg_len;
    }

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "alg_len = %lu\n", alg_len);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);

    //char binary_string[8 * len + 1];
    char binary_string[bin_len + 1];
    binary_string[0] = '\0';

    for (size_t i = 0; i < len; i++) {
        char byte_string[9];
        //sprintf(byte_string, "%08b", src[i]);
        to_bin8(byte_string, src[i]);
        strcat(binary_string, byte_string);
    }

    if (padding != 0) {
        //strcat(binary_string, "00000" + padding);
        char* alg_str = binary_string + (8 * len);
        memset(alg_str, '0', alg_len);
        //alg_str += alg_len;
        //alg_str[0] = '\0';
    }
    binary_string[bin_len] = '\0';

    //fprintf(stderr, "bin_str = %s\n", binary_string);
    //fprintf(stderr, "str_len = %lu\n", strlen(binary_string));

    size_t out_len = 0;
    for (size_t i = 0; i < bin_len; i += 5) {
    
        char segment[6];
        strncpy(segment, binary_string + i, 5);
        segment[5] = '\0';
        int index = (int) strtol(segment, NULL, 2);
        strncat(out, &encode_table[index], 1);
        out_len++;
    }

    //fprintf(stderr, "out_len = %lu\n", out_len);
    alg_len = 0;
    //while (strlen(out) % 8 != 0) {
    //    strcat(out, "=");
    //    out_len++;
    //    alg_len++;
    //}

    if (out_len % 8 != 0) {
        size_t count = out_len / 8;
        count++;
        size_t out_len2 = count * 8;
        alg_len = out_len2 - out_len;
        memset(out + out_len, '=', alg_len);
    }

    fprintf(stderr, "alg_len = %lu\n", alg_len);
    fprintf(stderr, "out_len = %lu\n", out_len);
}

static size_t base32_decode_len_bin(size_t len) {
	return 5 * len;
}

static size_t base32_decode_len(size_t len) {
	size_t out_len = base32_decode_len_bin(len);

    // calculate iterations with step 8
    return out_len / 8;
}

void base32_decode(unsigned char* out, const char *src, size_t len) {
    //fprintf(stderr, "src_len = %lu\n", len);
    size_t bin_len = 5 * len;
    size_t padding = bin_len % 8;

    //if (padding != 0) {
        //bin_len = bin_len - padding
    //}

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);

    char binary_string[bin_len + 1];
    binary_string[0] = '\0';

    if (padding != 0) {
        bin_len = bin_len - padding;
        //fprintf(stderr, "bin_len = %lu\n", bin_len);
    }

    for (size_t i = 0; i < len; i++) {
        if (src[i] != '=') {
            char index_string[6];
            //sprintf(index_string, "%05b", (int) (strchr(encode_table, src[i]) - encode_table));
            to_bin5(index_string, (int) (strchr(encode_table, src[i]) - encode_table));
            strcat(binary_string, index_string);
        }
    }

    //padding = strlen(binary_string) % 8;
    //if (padding != 0) {
        //bin_len = strlen(binary_string) - padding;
        //binary_string[strlen(binary_string) - padding] = '\0';
        //binary_string[bin_len] = '\0';
    //}

    binary_string[bin_len] = '\0';

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "bin_len = %lu\n", bin_len);
    //fprintf(stderr, "str_len = %lu\n", strlen(binary_string));

    //for (size_t i = 0; i < strlen(binary_string); i += 8) {
    size_t out_len = 0;
    for (size_t i = 0; i < bin_len; i += 8) {
        char byte_string[9];
        strncpy(byte_string, binary_string + i, 8);
        byte_string[8] = '\0';
        out[i / 8] = (unsigned char) strtol(byte_string, NULL, 2);
        out_len++;
    }
    //fprintf(stderr, "out_len = %lu\n", out_len);
}

////

char* lib_base32_encode(const char *src, size_t len, size_t* out_len) {
	size_t olen = base32_encode_len(len);
    //fprintf(stderr, "%lu\n", olen);
    char* out = (char*) malloc(olen + 1);
	base32_encode(out, (unsigned char*) src, len);
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

char* lib_base32_decode(const char *src, size_t len, size_t* out_len) {
    size_t olen = base32_decode_len(len);
    //fprintf(stderr, "%lu\n", olen);
    char* out = (char*) malloc(olen + 1);
	base32_decode((unsigned char*) out, src, len);
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

// https://medium.com/@at.kishor.k/demystifying-base32-an-in-depth-guide-to-this-encoding-standard-697b9426fc25
//01001000011001010110110001101100011011110011000100110010001100110