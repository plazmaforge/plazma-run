#include <stdlib.h>
#include <stdbool.h>
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

static bool base64_non_print(char c) {
    return (c >= 0 && c <= 32);
}

static bool base64_is_spec(char c) {
    return (c == '=' || base64_non_print(c));
}

static bool base64_is_table(char c) {
	return ((c >= 'A' && c <= 'Z')
	    || (c >= 'a' && c <= 'z')
		|| (c >= '0' && c <= '9')
		|| c == '+' 
		|| c == '/');
}

static bool base64_is_valid_char(char c) {
    return base64_is_spec(c) || base64_is_table(c);
}

static bool base64_is_valid(const char *src, size_t len) {
    char* s = (char*) src;
    for (size_t i = 0; i < len; i++) {
        if (!base64_is_valid_char(*s)) {
			// error: invalid char
            return false;
        }
        s++;
    }
    return true;
}

static size_t base64_encode_len(size_t len) {
	return ((len + 2) / 3 * 4);
}

static int base64_encode(const char* data, size_t len, char* odata, size_t* olen) {
	int i;
	char *p;

	p = odata;
	for (i = 0; i < len - 2; i += 3) {
		*p++ = encode_table[(data[i] >> 2) & 0x3F];
		*p++ = encode_table[((data[i] & 0x3) << 4) |
			((int)(data[i + 1] & 0xF0) >> 4)];
		*p++ = encode_table[((data[i + 1] & 0xF) << 2) |
			((int)(data[i + 2] & 0xC0) >> 6)];
		*p++ = encode_table[data[i + 2] & 0x3F];
	}
	if (i < len) {
		*p++ = encode_table[(data[i] >> 2) & 0x3F];
		if (i == (len - 1)) {
			*p++ = encode_table[((data[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = encode_table[((data[i] & 0x3) << 4) |
				((int)(data[i + 1] & 0xF0) >> 4)];
			*p++ = encode_table[((data[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	*olen = p - odata;
	return 0;
}

////

static size_t base64_decode_len(const char* data, size_t len, size_t* nprbytes) {
	size_t blen;
	const unsigned char* ibuf;
	size_t _nprbytes;

	ibuf = (const unsigned char *) data;
	size_t count = 0;
	while (count <= len && decode_table[*(ibuf++)] <= 63) {
		count++;
	}
	//fprintf(stderr, "count-1   : %lu\n", count);

	_nprbytes = (ibuf - (const unsigned char *) data) - 1;
	blen = ((_nprbytes + 3) / 4) * 3;

	*nprbytes = _nprbytes;

	//fprintf(stderr, "ilen-1    : %lu\n", len);
	//fprintf(stderr, "blen-1    : %lu\n", blen);
	//fprintf(stderr, "nprbytes-1: %lu\n", *nprbytes);

	return blen;
}

static int base64_decode(const char* data, size_t len, char* odata, size_t* olen, size_t nprbytes) {

	size_t blen = *olen;
	const unsigned char* ibuf;
	unsigned char* obuf;
	
	//int nprbytes;
	//ibuf = (const unsigned char *) data;
	//while (decode_table[*(ibuf++)] <= 63);
	//nprbytes = (ibuf - (const unsigned char *) data) - 1;
	//blen = ((nprbytes + 3) / 4) * 3;

	//fprintf(stderr, "blen-2    : %lu\n", blen);
	//fprintf(stderr, "nprbytes-2: %d\n", nprbytes);

	obuf = (unsigned char *) odata;
	ibuf = (const unsigned char *) data;

	while (nprbytes > 4) {
		*(obuf++) =
			(unsigned char)(decode_table[*ibuf] << 2 | decode_table[ibuf[1]] >> 4);
		*(obuf++) =
			(unsigned char)(decode_table[ibuf[1]] << 4 | decode_table[ibuf[2]] >> 2);
		*(obuf++) =
			(unsigned char)(decode_table[ibuf[2]] << 6 | decode_table[ibuf[3]]);
		ibuf += 4;
		nprbytes -= 4;
	}

	/* Note: (nprbytes == 1) would be an error, so just ingore that case */
	if (nprbytes > 1) {
		*(obuf++) =
			(unsigned char)(decode_table[*ibuf] << 2 | decode_table[ibuf[1]] >> 4);
	}
	if (nprbytes > 2) {
		*(obuf++) =
			(unsigned char)(decode_table[ibuf[1]] << 4 | decode_table[ibuf[2]] >> 2);
	}
	if (nprbytes > 3) {
		*(obuf++) =
			(unsigned char)(decode_table[ibuf[2]] << 6 | decode_table[ibuf[3]]);
	}

	blen -= (4 - nprbytes) & 3;
	*olen = blen; 

	return 0;
}

////

char* lib_base64_encode(const char* data, size_t len, size_t* olen) {
	if (olen) {
		*olen = 0;
	}

    if (!data || !olen) {
		// error: invalid arguments
        return NULL;
    }

	if (len == 0) {
		// error: empty data
        return NULL;
    }

	size_t blen = base64_encode_len(len);
	if (blen == 0) {
		// error: encode len
		return NULL;
	}

    char* buf = (char*) malloc(blen + 1);
    if (!buf) {
        return NULL;
    }
	int retval = base64_encode(data, len, buf, &blen);
    if (retval != 0) {
        free(buf);
        return NULL;
    }
    buf[blen] = '\0';
    *olen = blen;
	return buf;
}

char* lib_base64_decode(const char* data, size_t len, size_t* olen) {
	if (olen) {
		*olen = 0;
	}
	
	if (!data || !olen) {
		// error: invalid arguments
        return NULL;
    }

	if (len == 0) {
		// error: empty data
        return NULL;
    }

	while (len > 0 && base64_is_spec(data[len - 1])) {
		len--;
	}

	if (len == 0) {
		// error: empty trim data
		return NULL;
	}

    // Validate chars
    if (!base64_is_valid(data, len)) {
		// error: validation
        return NULL;
    }

	size_t nprbytes = 0;
    size_t blen = base64_decode_len(data, len, &nprbytes);
	if (blen == 0) {
		// error: decode len
		return NULL;
	}
    char* buf = (char*) malloc(blen + 1);
    if (!buf) {
        return NULL;
    }
	int retval = base64_decode(data, len, buf, &blen, nprbytes);
    if (retval != 0) {
        free(buf);
        return NULL;
    }
    buf[blen] = '\0';
	*olen = blen;
	return buf;
}

// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
// https://github.com/gaspardpetit/base64/
// https://web.mit.edu/freebsd/head/contrib/wpa/src/utils/
// https://github.com/gaspardpetit/base64/blob/main/src/apache/apache.h