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

static bool base16_is_valid(const char* data, size_t size) {
    char* s = (char*) data;
    for (size_t i = 0; i < size; i++) {
        if (!base16_is_valid_char(*s)) {
			//fprintf(stderr, "Invalid char: '%c' [%d]\n", *s, (int) (*s));
            return false;
        }
        s++;
    }
    return true;
}

////

static size_t base16_encode_size(size_t size) {
	return size * 2;
}

static size_t base16_decode_size(size_t size) {
	return size / 2;
}

int hex2bin(char* out, const char hex) {
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

static ssize_t base16_encode(char* odata, const char* idata, size_t isize) {
    const unsigned char* buf = (const unsigned char*) idata;
    for (size_t i = 0; i < isize; i++) {
        odata[i * 2]     = encode_table[buf[i] >> 4];
        odata[i * 2 + 1] = encode_table[buf[i] & 0x0F];
    }
    return isize * 2;
}

static ssize_t base16_decode(char* odata, const char* idata, size_t isize) {
	if (isize % 2 != 0) {
		return -1;
	}
		
	// isize /= 2;
	size_t osize = isize / 2;
	char b1;
	char b2;
	for (size_t i = 0; i < osize; i++) {
		if (!hex2bin(&b1, idata[i * 2]) || !hex2bin(&b2, idata[i * 2 + 1])) {
			//fprintf(stderr, "ERROR: non hex: '%c'-'%c'\n", idata[i * 2], idata[i * 2 + 1]);
			return -1;
		}
		odata[i] = (b1 << 4) | b2;
	}
	return osize;
}

////

int lib_base16_encode(const char* idata, size_t isize, char** odata, size_t* osize) {
    if (!idata || !osize || isize == 0) {
        return -1;
    }

    size_t size = base16_encode_size(isize);
    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }
    
    ssize_t retval = base16_encode(buf, idata, isize);
    if (retval < 0) {
        free(buf);
        return -1;
    }

    buf[size] = '\0';

    *odata = buf;
    *osize = size;

    return 0;
}

int lib_base16_decode(const char* idata, size_t isize, char** odata, size_t* osize) {
    if (!idata || !osize || isize == 0) {
        return -1;
    }

	size_t i = isize - 1;
	while (i >= 0 && base16_is_spec(idata[i])) {
		if (i == 0) {
			return -1;
		}
		i--;
		isize--;
	}

    if (isize % 2 != 0) {
        //fprintf(stderr, "ERROR: size\n");
        return -1;
    }

    // Validate chars
    if (!base16_is_valid(idata, isize)) {
        //fprintf(stderr, "ERROR: invalid\n");
        return -1;
    }

    size_t size = base16_decode_size(isize);
    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }

    ssize_t retval = base16_decode(buf, idata, isize);
    if (retval < 0) {
        free(buf);
        return -1;
    }
    buf[size] = '\0';

    *odata = buf;
    *osize = size;

    return 0;
}

// https://nachtimwald.com/2017/09/24/hex-encode-and-decode-in-c/ 

// https://helloacm.com/c-program-to-convert-characters-to-hex-c-source-text/