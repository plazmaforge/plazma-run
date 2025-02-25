#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base16.h"

static const char encode_table[] = "0123456789ABCDEF";

////

static bool base16_non_print(char c) {
    return (c >= 0 && c <= 32);
}

static bool base16_is_spec(char c) {
    return base16_non_print(c);
}

static bool base16_is_table(char c) {
	return ((c >= '0' && c <= '9')
		|| (c >= 'A' && c <= 'F'));
}

static bool base16_is_valid_char(char c) {
    return base16_is_spec(c) || base16_is_table(c);
}

static bool base16_is_valid(const char *src, size_t len) {
    char* s = (char*) src;
    for (size_t i = 0; i < len; i++) {
        if (!base16_is_valid_char(*s)) {
			//fprintf(stderr, "Invalid char: '%c' [%d]\n", *s, (int) (*s));
            return false;
        }
        s++;
    }
    return true;
}

////

static size_t base16_encode_len(size_t len) {
	return len * 2;
}

static size_t base16_decode_len(size_t len) {
	return len / 2;
}

int hex2bin(char *out, const char hex) {
	if (!out)
		return 0;

	if (hex >= '0' && hex <= '9') {
		*out = hex - '0';
	} else if (hex >= 'A' && hex <= 'F') {
		*out = hex - 'A' + 10;
	} else if (hex >= 'a' && hex <= 'f') {
		*out = hex - 'a' + 10;
	} else {
		return 0;
	}

	return 1;
}

static ssize_t base16_encode(char* out, const char* src, size_t len) {
	const unsigned char* buf = (const unsigned char*) src;
	for (size_t i = 0; i < len; i++) {
		out[i * 2]     = encode_table[buf[i] >> 4];
		out[i * 2 + 1] = encode_table[buf[i] & 0x0F];
	}
	return len * 2;
}

static ssize_t base16_decode(char* out, const char* src, size_t len) {
	if (len % 2 != 0) {
		return -1;
	}
		
	len /= 2; // out_len
	char   b1;
	char   b2;
	for (size_t i = 0; i < len; i++) {
		if (!hex2bin(&b1, src[i * 2]) || !hex2bin(&b2, src[i * 2 + 1])) {
			//fprintf(stderr, "ERROR: non hex: '%c'-'%c'\n", src[i * 2], src[i * 2 + 1]);
			return -1;
		}
		out[i] = (b1 << 4) | b2;
	}
	return len; // out_len
}

////

char* lib_base16_encode(const char *src, size_t len, size_t* out_len) {
    if (!src || !out_len || len == 0) {
        return NULL;
    }

	size_t olen = base16_encode_len(len);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base16_encode(out, src, len);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

char* lib_base16_decode(const char *src, size_t len, size_t* out_len) {
	if (!src || !out_len || len == 0) {
        return NULL;
    }

	size_t i = len - 1;
	while (i >= 0 && base16_is_spec(src[i])) {
		if (i == 0) {
			return NULL;
		}
		i--;
		len--;
	}

	if (len % 2 != 0) {
		//fprintf(stderr, "ERROR: size\n");
		return NULL;
	}

    // Validate chars
    if (!base16_is_valid(src, len)) {
		//fprintf(stderr, "ERROR: invalid\n");
        return NULL;
    }

    size_t olen = base16_decode_len(len);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base16_decode(out, src, len);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';

	*out_len = olen;

	return out;
}

// https://nachtimwald.com/2017/09/24/hex-encode-and-decode-in-c/ 

// https://helloacm.com/c-program-to-convert-characters-to-hex-c-source-text/