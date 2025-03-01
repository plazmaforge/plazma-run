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

static bool base64_is_valid(const char* data, size_t size) {
    char* s = (char*) data;
    for (size_t i = 0; i < size; i++) {
        if (!base64_is_valid_char(*s)) {
			//fprintf(stderr, "#%lu '%c' [%d]\n", (i + 1), *s, (int) *s);
			// error: invalid char
            return false;
        }
        s++;
    }
    return true;
}

static size_t base64_encode_size(size_t isize) {
	return ((isize + 2) / 3 * 4);
}

static int base64_encode(const char* idata, size_t isize, char* odata, size_t* osize) {
	int i;
	char *p;

	p = odata;
	for (i = 0; i < isize - 2; i += 3) {
		*p++ = encode_table[(idata[i] >> 2) & 0x3F];
		*p++ = encode_table[((idata[i] & 0x3) << 4) |
			((int)(idata[i + 1] & 0xF0) >> 4)];
		*p++ = encode_table[((idata[i + 1] & 0xF) << 2) |
			((int)(idata[i + 2] & 0xC0) >> 6)];
		*p++ = encode_table[idata[i + 2] & 0x3F];
	}
	if (i < isize) {
		*p++ = encode_table[(idata[i] >> 2) & 0x3F];
		if (i == (isize - 1)) {
			*p++ = encode_table[((idata[i] & 0x3) << 4)];
			*p++ = '=';
		}
		else {
			*p++ = encode_table[((idata[i] & 0x3) << 4) |
				((int)(idata[i + 1] & 0xF0) >> 4)];
			*p++ = encode_table[((idata[i + 1] & 0xF) << 2)];
		}
		*p++ = '=';
	}

	*osize = p - odata;
	return 0;
}

////

static size_t base64_decode_size(const char* idata, size_t isize, size_t* nprbytes) {
	const unsigned char* ibuf = (const unsigned char *) idata;
	size_t count = 0;
	while (count <= isize && decode_table[*(ibuf++)] <= 63) {
		count++;
	}
	
	//fprintf(stderr, "count-1   : %lu\n", count);

	*nprbytes = (ibuf - (const unsigned char *) idata) - 1;
	size_t size = ((*nprbytes + 3) / 4) * 3;

	//fprintf(stderr, "isize-1   : %lu\n", isize);
	//fprintf(stderr, "bsize-1   : %lu\n", size);
	//fprintf(stderr, "nprbytes-1: %lu\n", *nprbytes);

	return size;
}

static int base64_decode(const char* idata, size_t isize, char* odata, size_t* osize, size_t nprbytes) {

	// We doesn't use isize

	size_t size = *osize;
	const unsigned char* ibuf;
	unsigned char* obuf;
	
	//int nprbytes;
	//ibuf = (const unsigned char *) idata;
	//while (decode_table[*(ibuf++)] <= 63);
	//nprbytes = (ibuf - (const unsigned char *) idata) - 1;
	//size = ((nprbytes + 3) / 4) * 3;

	//fprintf(stderr, "bsize-2   : %lu\n", size);
	//fprintf(stderr, "nprbytes-2: %d\n", nprbytes);

	ibuf = (const unsigned char *) idata;
	obuf = (unsigned char *) odata;

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

	size -= (4 - nprbytes) & 3;
	*osize = size; 

	return 0;
}

////

int lib_base64_encode(const char* idata, size_t isize, char** odata, size_t* osize) {
	if (osize) {
		*osize = 0;
	}

    if (!idata || !odata || !osize) {
		// error: invalid arguments
        return -1;
    }

	if (isize == 0) {
		// error: empty data
        return -1;
    }

	size_t size = base64_encode_size(isize);
	if (size == 0) {
		// error: encode size
		return -1;
	}

    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }
	int retval = base64_encode(idata, isize, buf, &size);
    if (retval != 0) {
        free(buf);
        return retval;
    }
    buf[size] = '\0';

	*odata = buf;
    *osize = size;

	return 0;
}

int lib_base64_decode(const char* idata, size_t isize,  char** odata, size_t* osize) {
	if (osize) {
		*osize = 0;
	}
	
	if (!idata || !odata || !osize) {
		// error: invalid arguments
        return -1;
    }

	if (isize == 0) {
		// error: empty data
        return -1;
    }

	//fprintf(stderr, "isize: %lu\n", isize);
	while (isize > 0 && base64_is_spec(idata[isize - 1])) {
		isize--;
	}
	//fprintf(stderr, "isize: %lu\n", isize);

	if (isize == 0) {
		// error: empty trim data
		return -1;
	}

    // Validate chars
    if (!base64_is_valid(idata, isize)) {
		// error: validation
        return -1;
    }

	size_t nprbytes = 0;
    size_t size = base64_decode_size(idata, isize, &nprbytes);
	if (size == 0) {
		// error: decode size
		return -1;
	}
    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }
	int retval = base64_decode(idata, isize, buf, &size, nprbytes);
    if (retval != 0) {
        free(buf);
        return -1;
    }
    buf[size] = '\0';

	*odata = buf;
	*osize = size;

	return 0;
}

// https://stackoverflow.com/questions/342409/how-do-i-base64-encode-decode-in-c
// https://github.com/gaspardpetit/base64/
// https://web.mit.edu/freebsd/head/contrib/wpa/src/utils/
// https://github.com/gaspardpetit/base64/blob/main/src/apache/apache.h