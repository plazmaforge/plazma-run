#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "base64.h"

static const char encode_table[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char decode_table[256] = {
	/* ASCII table */
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

////

static size_t base64_encode_len(size_t len) {
	return ((len + 2) / 3 * 4);
}

static ssize_t base64_encode(char* out, const char* src, size_t len) {
	int i;
	char *p;

	p = out;
	for (i = 0; i < len - 2; i += 3) {
		*p++ = encode_table[(src[i] >> 2) & 0x3F];
		*p++ = encode_table[((src[i] & 0x3) << 4) |
			((int)(src[i + 1] & 0xF0) >> 4)];
		*p++ = encode_table[((src[i + 1] & 0xF) << 2) |
			((int)(src[i + 2] & 0xC0) >> 6)];
		*p++ = encode_table[src[i + 2] & 0x3F];
	}
	if (i < len) {
		*p++ = encode_table[(src[i] >> 2) & 0x3F];
		if (i == (len - 1)) {
			*p++ = encode_table[((src[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = encode_table[((src[i] & 0x3) << 4) |
				((int)(src[i + 1] & 0xF0) >> 4)];
			*p++ = encode_table[((src[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	return p - out;
}

////

static int base64_decode_len(const char* src) {
	int out_len;
	const unsigned char *bufin;
	int nprbytes;

	bufin = (const unsigned char *) src;
	while (decode_table[*(bufin++)] <= 63);

	nprbytes = (bufin - (const unsigned char *) src) - 1;
	out_len = ((nprbytes + 3) / 4) * 3;

	return out_len;
}

static ssize_t base64_decode(char* out, const char* src) {
	ssize_t out_len = 0;
	const unsigned char *bufin;
	unsigned char *bufout;
	int nprbytes;

	bufin = (const unsigned char *) src;
	while (decode_table[*(bufin++)] <= 63);
	nprbytes = (bufin - (const unsigned char *) src) - 1;
	out_len = ((nprbytes + 3) / 4) * 3;

	bufout = (unsigned char *) out;
	bufin = (const unsigned char *) src;

	while (nprbytes > 4) {
		*(bufout++) =
			(unsigned char)(decode_table[*bufin] << 2 | decode_table[bufin[1]] >> 4);
		*(bufout++) =
			(unsigned char)(decode_table[bufin[1]] << 4 | decode_table[bufin[2]] >> 2);
		*(bufout++) =
			(unsigned char)(decode_table[bufin[2]] << 6 | decode_table[bufin[3]]);
		bufin += 4;
		nprbytes -= 4;
	}

	/* Note: (nprbytes == 1) would be an error, so just ingore that case */
	if (nprbytes > 1) {
		*(bufout++) =
			(unsigned char)(decode_table[*bufin] << 2 | decode_table[bufin[1]] >> 4);
	}
	if (nprbytes > 2) {
		*(bufout++) =
			(unsigned char)(decode_table[bufin[1]] << 4 | decode_table[bufin[2]] >> 2);
	}
	if (nprbytes > 3) {
		*(bufout++) =
			(unsigned char)(decode_table[bufin[2]] << 6 | decode_table[bufin[3]]);
	}

	out_len -= (4 - nprbytes) & 3;
	return out_len;
}

////

char* lib_base64_encode(const char *src, size_t len, size_t* out_len) {
	size_t olen = base64_encode_len(len);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base64_encode(out, src, len);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    *out_len = olen;
	return out;
}

char* lib_base64_decode(const char *src, size_t len, size_t* out_len) {
    size_t olen = base64_decode_len(src);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
	ssize_t retval = base64_decode(out, src);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
	return out;
}

// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
// https://github.com/gaspardpetit/base64/
// https://web.mit.edu/freebsd/head/contrib/wpa/src/utils/
// https://github.com/gaspardpetit/base64/blob/main/src/apache/apache.h