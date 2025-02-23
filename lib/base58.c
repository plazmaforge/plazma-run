#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base58.h"

//static const char* const ASCII_CHARS  = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";

static const char encode_table[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

static const int8_t decode_table[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
};

static bool base58_non_print(char c) {
    return (c >= 0 && c <= 32);
}

static bool base58_is_spec(char c) {
    return base58_non_print(c);
}

typedef uint64_t b58_maxint_t;
typedef uint32_t b58_almostmaxint_t;
#define b58_almostmaxint_bits (sizeof(b58_almostmaxint_t) * 8)
static const b58_almostmaxint_t b58_almostmaxint_mask = ((((b58_maxint_t)1) << b58_almostmaxint_bits) - 1);

static bool _base58_encode_(char* out, size_t* out_len, const char* src, size_t len) {
	const uint8_t* bin = (uint8_t*) src;
	int carry;
	size_t i, j, high, zcount = 0;
	size_t size;
	
	while (zcount < len && !bin[zcount])
		++zcount;
	
	size = (len - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
	
	for (i = zcount, high = size - 1; i < len; ++i, high = j)	{
		for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
			if (!j) {
				// Otherwise j wraps to maxint which is > high
				break;
			}
		}
	}
	
	for (j = 0; j < size && !buf[j]; ++j);
	
	if (*out_len <= zcount + size - j) {
		*out_len = zcount + size - j + 1;
		return false;
	}
	
	if (zcount)
		memset(out, '1', zcount);

	for (i = zcount; j < size; ++i, ++j)
		out[i] = encode_table[buf[j]];

	out[i] = '\0';
	*out_len = i + 1;
	
	return true;
}


static bool _base58_decode_(char* out, size_t* out_len, const char* src, size_t len) {
	size_t _out_len = *out_len;
    //fprintf(stderr, "out_len: %lu\n", _out_len);
	const unsigned char *b58u = (void*) src;
	unsigned char* binu = (unsigned char*) out;
	size_t outisz = (_out_len + sizeof(b58_almostmaxint_t) - 1) / sizeof(b58_almostmaxint_t);
    //fprintf(stderr, "outisz: %lu\n", outisz);

	b58_almostmaxint_t outi[outisz];
	b58_maxint_t t;
	b58_almostmaxint_t c;
	size_t i, j;
	uint8_t bytesleft = _out_len % sizeof(b58_almostmaxint_t);
	b58_almostmaxint_t zeromask = bytesleft ? (b58_almostmaxint_mask << (bytesleft * 8)) : 0;
	unsigned zerocount = 0;

    //fprintf(stderr, "bytesleft: %d\n", bytesleft);
	
	if (!len)
		len = strlen(src);
	
	for (i = 0; i < outisz; ++i) {
		outi[i] = 0;
	}
	
	// Leading zeros, just count
	for (i = 0; i < len && b58u[i] == '1'; ++i)
		++zerocount;
	
	for ( ; i < len; ++i) {
		if (b58u[i] & 0x80)
			// High-bit set on invalid digit
			return false;

		if (decode_table[b58u[i]] == -1)
			// Invalid base58 digit
			return false;

		c = (unsigned) decode_table[b58u[i]];
		for (j = outisz; j--; ) {
			t = ((b58_maxint_t)outi[j]) * 58 + c;
			c = t >> b58_almostmaxint_bits;
			outi[j] = t & b58_almostmaxint_mask;
		}

		if (c)
			// Output number too big (carry to the next int32)
			return false;
		if (outi[0] & zeromask)
			// Output number too big (last int32 filled too far)
			return false;
	}
	
	j = 0;
    int zzz = 0;
    int shift = 0;
	if (bytesleft) {
		for (i = bytesleft; i > 0; --i) {
			//*(binu++) = (outi[0] >> (8 * (i - 1))) & 0xff;
            *(binu) = (outi[0] >> (8 * (i - 1))) & 0xff;
            //fprintf(stderr, "a>>i: %d, %d\n", zzz, (int) *(binu));
            if ((int) *(binu) != 0) {
                //fprintf(stderr, "a>>i: %d, %d\n", zzz, (int) *(binu));
                binu++;
                zzz++;
            }
		}
		++j;
	}
	
	for (; j < outisz; ++j) {
		for (i = sizeof(*outi); i > 0; --i) {
			//*(binu++) = (outi[j] >> (8 * (i - 1))) & 0xff;
            *(binu) = (outi[j] >> (8 * (i - 1))) & 0xff;
            //fprintf(stderr, "b>>i: %d, %d\n", zzz, (int) *(binu));
            if ((int) *(binu) != 0) {
                //fprintf(stderr, "b>>i: %d, %d\n", zzz, (int) *(binu));
                binu++;
                zzz++;
            }
		}
	}

    //fprintf(stderr, "out_len: %lu\n", _out_len);
	
	// Count canonical base58 byte count
	binu = (unsigned char*) out;
	for (i = 0; i < _out_len; ++i) {
		if (binu[i])
			break;
		--*out_len;
	}

    //fprintf(stderr, "out_len: %lu\n", _out_len);

	*out_len += zerocount;

    //fprintf(stderr, "out_len: %lu\n", _out_len);
	
	return true;
}

static ssize_t base58_encode(char* out, const char* src, size_t len) {
    size_t olen = len * 2;
    if (_base58_encode_(out, &olen, src, len)) {
        return olen;
    }
    return -1;
}

char* lib_base58_encode(const char* src, size_t len, size_t* out_len) {

    if (!src || !out_len || len == 0) {
        return NULL;
    }

	size_t olen = len * 2; //base58_encode_len(len);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
    memset(out, 0, olen);
	ssize_t retval = base58_encode(out, src, len);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    *out_len = olen;

	return out;
}

static ssize_t base58_decode(char* out, const char* src, size_t len) {
    //fprintf(stderr, "base58_decode: processing..\n");
    size_t olen = len;// / 2;
    if (_base58_decode_(out, &olen, src, len)) {
        //fprintf(stderr, "base58_decode: success\n");
        return olen;
    }
    //fprintf(stderr, "base58_decode: error\n");
    return -1;
}

char* lib_base58_decode(const char *src, size_t len, size_t* out_len) {

	if (!src || !out_len || len == 0) {
        return NULL;
    }

    //fprintf(stderr, "len: %lu\n", len);

    size_t i = len - 1;
	while (i >= 0 && base58_is_spec(src[i])) {
		if (i == 0) {
			return NULL;
		}
		i--;
		len--;
	}
    //fprintf(stderr, "len: %lu\n", len);


    // Validate chars
    //if (!base58_is_valid(src, len)) {
    //    return NULL;
    //}

    size_t olen = len;// / 2; //base58_decode_len(src);
    char* out = (char*) malloc(olen + 1);
    if (!out) {
        return NULL;
    }
    memset(out, 0, olen);
	ssize_t retval = base58_decode(out, src, len);
    if (retval < 0) {
        free(out);
        return NULL;
    }
    out[olen] = '\0';
    
	return out;
}

// https://github.com/bitcoin/libbase58
// https://bitcoin.stackexchange.com/questions/76480/encode-decode-base-58-c
// https://github.com/martinus/base58/blob/master/source/base58.cpp
// https://medium.com/@raphaeldemoraesdutenkefer/implementing-a-base58-decoder-for-extended-private-key-in-c-d4cc8e9b0792
// https://cplusplus.com/forum/beginner/285815/