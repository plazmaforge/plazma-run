#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base58.h"

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

//typedef uint64_t b58_maxint_t;
//typedef uint32_t b58_almostmaxint_t;
//#define b58_almostmaxint_bits (sizeof(b58_almostmaxint_t) * 8)
//static const b58_almostmaxint_t b58_almostmaxint_mask = ((((b58_maxint_t)1) << b58_almostmaxint_bits) - 1);

typedef uint64_t b58_maxint_t;
typedef uint32_t b58_int_t;
#define b58_int_bits (sizeof(b58_int_t) * 8)
static const b58_int_t b58_int_mask = ((((b58_maxint_t)1) << b58_int_bits) - 1);

static size_t base58_zcount(const uint8_t* data, size_t len) {
    size_t count = 0;
    while (count < len && data[count] == 0) {
        ++count;
    }
    return count;
}

static int _base58_encode_(const char* data, size_t len, char* odata, size_t* olen) {
    const uint8_t* ibuf = (uint8_t*) data;

    size_t i      = 0;
    size_t j      = 0;
    int carry     = 0;

    //while (zcount < len && !bin[zcount])
    //    ++zcount;

	// Calculate zero count in input data
    size_t zcount = base58_zcount(ibuf, len);

    //fprintf(stderr, "base58_encode: zcount: %lu\n", zcount);

	// Create tmp buffer
	size_t size = (len - zcount) * 138 / 100 + 1;
	uint8_t buf[size];
	memset(buf, 0, size);
    
    size_t high = size - 1;    // >>ALT
	//for (i = zcount, high = size - 1; i < len; ++i, high = j)	{
    for (i = zcount; i < len; ++i)	{

        carry = ibuf[i]; // >>ALT

		//for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
        for (j = size - 1; (j > high) || carry; --j) {

			carry += 256 * buf[j];
			buf[j] = carry % 58;
			carry /= 58;
			if (!j) {
				// Otherwise j wraps to maxint which is > high
				break;
			}
		}
        high = j;       // >>ALT
	}
	
	//for (j = 0; j < size && !buf[j]; ++j);

	// Calculate zero count in tmp buffer
    j = base58_zcount((const uint8_t*) buf, size);

    //fprintf(stderr, "base58_encode: j: %lu\n", j);
	
	if (*olen <= zcount + size - j) {
		*olen = zcount + size - j + 1;
		return -1;
	}

	// Add '1' to output buffer
	if (zcount) {
        memset(odata, '1', zcount);
    }
	
	// Encode data
	for (i = zcount; j < size; ++i, ++j) {
        odata[i] = encode_table[buf[j]];
    }
		
	odata[i] = '\0';
	*olen = i; // >>ALT + 1; // WHY +1 ?
	
	return 0;
}


static int _base58_decode_(const char* data, size_t len, char* odata, size_t* olen) {

	size_t _olen = *olen;
	//fprintf(stderr, "olen: %lu\n", _olen);
	
	const unsigned char* ibuf = (void*) data;
	unsigned char* obuf = (unsigned char*) odata;

	//size_t outisz = (_olen + sizeof(b58_almostmaxint_t) - 1) / sizeof(b58_almostmaxint_t);
	//fprintf(stderr, "size: %lu\n", size);
	//b58_almostmaxint_t outi[outisz];

	b58_maxint_t t;
	b58_int_t c;
	size_t i, j;
	uint8_t bytesleft = _olen % sizeof(b58_int_t);
	b58_int_t zmask = bytesleft ? (b58_int_mask << (bytesleft * 8)) : 0;
	unsigned zcount = 0;

    //fprintf(stderr, "bytesleft: %d\n", bytesleft);
		
	size_t size = (_olen + sizeof(b58_int_t) - 1) / sizeof(b58_int_t);    
	b58_int_t buf[size];
	memset(buf, 0, size * sizeof(b58_int_t));
	
	//for (i = 0; i < size; ++i) {
	//	buf[i] = 0;
	//}

	// Leading zeros, just count
	for (i = 0; i < len && ibuf[i] == '1'; ++i)
		++zcount;
	
	for ( ; i < len; ++i) {
		if (ibuf[i] & 0x80)
			// High-bit set on invalid digit
			return -1001;

		if (decode_table[ibuf[i]] == -1)
			// Invalid base58 digit
			return -1002;

		c = (unsigned) decode_table[ibuf[i]];
		for (j = size; j--; ) {
			t = ((b58_maxint_t) buf[j]) * 58 + c;
			c = t >> b58_int_bits;
			buf[j] = t & b58_int_mask;
		}

		if (c)
			// Output number too big (carry to the next int32)
			return -1003;

		if (buf[0] & zmask)
			// Output number too big (last int32 filled too far)
			return -1004;
	}
	
	j = 0;
    int rlen = 0; // real len
	bool start = false;
	if (bytesleft) {
		for (i = bytesleft; i > 0; --i) {
			//*(binu++) = (outi[0] >> (8 * (i - 1))) & 0xff;
            *(obuf) = (buf[0] >> (8 * (i - 1))) & 0xff;
            if ((int) *(obuf) != 0 || start) {
                obuf++;
                rlen++;
				start = true;
            }
		}
		++j;
	}
	
	for (; j < size; ++j) {
		for (i = sizeof(*buf); i > 0; --i) {
			//*(binu++) = (outi[j] >> (8 * (i - 1))) & 0xff;
            *(obuf) = (buf[j] >> (8 * (i - 1))) & 0xff;
            if ((int) *(obuf) != 0 || start) {
                obuf++;
                rlen++;
				start = true;
            }
		}
	}

    //fprintf(stderr, "rlen: %lu\n", rlen);

    // Set real len
	*olen = rlen;
	
	// Count canonical base58 byte count
	// May be we don't have this case because we resolve it before
	int zlen = 0;
	obuf = (unsigned char*) odata;
	for (i = 0; i < _olen; ++i) {
		if (obuf[i])
			break;
		--*olen;
		zlen++;
	}

	/*
	// May be need shift first position of out?
	odata += zlen;
	*/

	//fprintf(stderr, "zlen: %lu\n", zlen);
    //fprintf(stderr, "zcount: %lu\n", zcount);

	*olen += zcount;

    //fprintf(stderr, "olen: %lu\n", _olen);
	
	return 0;
}

static int base58_encode(const char* data, size_t len, char* odata, size_t* olen) {
    return _base58_encode_(data, len, odata, olen);
}

int lib_base58_encode(const char* data, size_t len, char** odata, size_t* olen) {

    if (!data || !olen || len == 0) {
        return -1;
    }

	size_t blen = len * 2; //base58_encode_len(len);
    char* buf = (char*) malloc(blen + 1);
    if (!buf) {
        return -1;
    }
    memset(buf, 0, blen);
    //fprintf(stderr, "base58_encode: olen (alloc) = %lu\n", olen);
	int retval = base58_encode(data, len, buf, &blen);
    //fprintf(stderr, "base58_encode: olen (trunc) = %lu\n", olen);
    if (retval != 0) {
        free(buf);
        return -1;
    }
    buf[blen] = '\0';

	*odata = buf;
    *olen = blen;

	return 0;
}

static int base58_decode(const char* data, size_t len, char* odata, size_t* olen) {
    return _base58_decode_(data, len, odata, olen);
}

int lib_base58_decode(const char* data, size_t len, char** odata, size_t* olen) {

	if (!data || !olen || len == 0) {
        return -1;
    }

    //fprintf(stderr, "len: %lu\n", len);

	while (len > 0 && base58_is_spec(data[len - 1])) {
		len--;
	}

    //fprintf(stderr, "len: %lu\n", len);

    // Validate chars
    //if (!base58_is_valid(data, len)) {
    //    return -1;
    //}

    size_t blen = len;// / 2; //base58_decode_len(data);
    char* buf = (char*) malloc(blen + 1);
    if (!buf) {
        return -1;
    }
    memset(buf, 0, blen);
    //fprintf(stderr, "base58_decode: olen (alloc) = %lu\n", olen);
	int retval = base58_decode(data, len, buf, &blen);
    //fprintf(stderr, "base58_decode: olen (trunc) = %lu\n", olen);
    if (retval < 0) {
        free(buf);
        return -1;
    }
    buf[blen] = '\0';

	*odata = buf;
	*olen = blen;
    
	return 0;
}

// https://github.com/bitcoin/libbase58
// https://bitcoin.stackexchange.com/questions/76480/encode-decode-base-58-c
// https://github.com/martinus/base58/blob/master/source/base58.cpp
// https://medium.com/@raphaeldemoraesdutenkefer/implementing-a-base58-decoder-for-extended-private-key-in-c-d4cc8e9b0792
// https://cplusplus.com/forum/beginner/285815/

// https://kanga.exchange/university/en/courses/advanced-course/lessons/55-base-58-function-in-cryptocurrencies/
// https://news.ycombinator.com/item?id=18407321

// https://github.com/bitcoin/bitcoin/tree/master/src/crypto

// https://en.bitcoin.it/wiki/Base58Check_encoding

// https://www.vbforums.com/showthread.php?891688-VB6-Base58-encoding

// https://raw.githubusercontent.com/trezor/trezor-crypto/master/base58.c

// https://github.com/trezor/trezor-crypto