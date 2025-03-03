#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "base32.h"

static const char base32u[]   = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567"; /* Base32 (upper)     */
static const char base32l[]   = "abcdefghijklmnopqrstuvwxyz234567"; /* Base32 (lower)     */
static const char zbase32[]   = "ybndrfg8ejkmcpqxot1uwisza345h769"; /* Z-Base32           */
static const char base32hex[] = "0123456789ABCDEFGHIJKLMNOPQRSTUV"; /* Base32Hex          */
static const char cbase32[]   = "0123456789ABCDEFGHJKMNPQRSTVWXYZ"; /* Crockford’s Base32 */

/**
 * Return encode table by type  
 */
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
    if (type == LIB_CBASE32) {
        return cbase32;
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

static char base32_conv_crockford(char c) {
    if (c == 'o' 
        || c == 'O') {
        // oO => 0
        return '0';
    }
    if (c == 'i' 
        || c == 'I'
        || c == 'l'
        || c == 'L') {
        // iIlL => 1
        return '1';
    }
    if (c >= 'a' && c <= 'z') {
        // a..z => A..Z
        return c - 32;
    }
    return c;
}

static bool base32_is_table(int type, const char* encode_table, char c) {
    char* s = (char*) encode_table;
    if (type == LIB_CBASE32) {
        c = base32_conv_crockford(c);
    }
    for (size_t i = 0; i < 32; i++) {
        if (c == *s) {
            return true;
        }
        s++;
    }
    return false;
}

static bool base32_is_valid_char(int type, const char* encode_table, char c) {
    return base32_is_spec(type, c) || base32_is_table(type, encode_table, c);
}

static bool base32_is_valid(int type, const char* encode_table, const char* data, size_t size) {
    char* s = (char*) data;
    for (size_t i = 0; i < size; i++) {
        if (!base32_is_valid_char(type, encode_table, *s)) {
            return false;
        }
        s++;
    }
    return true;
}

static size_t base32_encode_size_fix(size_t size) {
	return 8 * size;
}

static size_t base32_encode_size_type(int type, size_t isize) {
	size_t size = base32_encode_size_fix(isize);
    size_t padding = size % 5;
    if (padding != 0) {
        size += (5 - padding);
    }

    // calculate iterations with step 5
    size_t osize = size / 5;

    if (type == LIB_ZBASE32) {
        return osize;
    }

    if (osize % 8 != 0) {
        size_t count = osize / 8;
        count++;
        osize = count * 8;
    }

    return osize;
}

static size_t base32_encode_size(size_t size) {
    return base32_encode_size_type(LIB_BASE32, size);
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

static int base32_encode_type(int type, const unsigned char* idata, size_t isize, char* odata) {
    if (!idata || !odata) {
        return -1;
    }
    if (isize == 0) {
        return 0;
    }

    const unsigned char* ibuf = idata;
    char* obuf = odata;
    const char* encode_table = base32_table_type(type);


    size_t fsize = 8 * isize;   // Fix size
    size_t asize = 0;           // Add size
    size_t size  = fsize;       // Buf size
    size_t padding = size % 5;
    if (padding != 0) {
        asize = 5 - padding;
        size += asize;
    }

    //fprintf(stderr, "isize = %lu\n", isize);    
    //fprintf(stderr, "fsize = %lu\n", fsize);
    //fprintf(stderr, "asize = %lu\n", asize);
    //fprintf(stderr, "size  = %lu\n", size);
    //fprintf(stderr, "padding = %lu\n", padding);

    char buf[size + 1];
    buf[0] = '\0';

    char byte_buf[9];
    byte_buf[8] = '\0';
    char* tmp = buf;

    for (size_t i = 0; i < isize; i++) {
        //char byte_str[9];
        //sprintf(byte_str, "%08b", data[i]);
        //to_bin8(byte_buf, data[i]);
        to_bin8(byte_buf, *ibuf);

        //byte_buf[8] = '\0';
        //strcat(buf, byte_buf); // very slowly

        memcpy(tmp, byte_buf, 8);
        tmp += 8;
        ibuf++;
    }

    if (padding != 0) {
        //strcat(bin_str, "00000" + padding);
        //char* str = buf + (8 * isize);
        //memset(str, '0', asize);

        memset(buf + fsize, '0', asize);
    }

    buf[size] = '\0';

    //fprintf(stderr, "buf = %s\n", buf);
    //fprintf(stderr, "strlen = %lu\n", strlen(buf));

    char segment_buf[6];
    size_t psize = 0; // Processed size

    for (size_t i = 0; i < size; i += 5) {    
        strncpy(segment_buf, buf + i, 5);
        segment_buf[5] = '\0';
        int index = (int) strtol(segment_buf, NULL, 2);

        //strncat(odata, &encode_table[index], 1); // very slowly

        *obuf = encode_table[index];
        obuf++;
        psize++;
    }

    //fprintf(stderr, "psize = %lu\n", psize);
    asize = 0; // Add size
    if (type != LIB_ZBASE32 && (psize % 8) != 0) {
        size_t count = psize / 8;
        count++;
        size_t psize_new = count * 8;
        asize = psize_new - psize;
        memset(odata + psize, '=', asize);
        psize += asize;
    }

    //fprintf(stderr, "asize = %lu\n", asize);
    //fprintf(stderr, "psize = %lu\n", psize);

    return psize;
}

static int base32_encode(const unsigned char* idata, size_t isize, char* odata) {
    return base32_encode_type(LIB_BASE32, idata, isize, odata);
}

static size_t base32_decode_size_fix(size_t size) {
	return 5 * size;
}

static size_t base32_decode_size(size_t isize) {
	size_t osize = base32_decode_size_fix(isize);

    // calculate iterations with step 8
    return osize / 8;
}

static int base32_decode_type(int type, const char* idata, size_t isize, unsigned char* odata) {
    if (!odata || !idata) {
        return -1;
    }
    if (isize == 0) {
        return 0;
    }

    const char* ibuf = idata;
    unsigned char* obuf = odata;
    const char* encode_table = base32_table_type(type);

    //fprintf(stderr, "isize = %lu\n", isize);
    size_t size = 5 * isize;
    size_t padding = size % 8;

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "size = %lu\n", size);

    char buf[size + 1];
    buf[0] = '\0';

    if (padding != 0) {
        size -= padding;
        //fprintf(stderr, "size = %lu\n", size);
    }

    char index_buf[6];
    index_buf[5] = '\0';
    char* tmp = buf;

    for (size_t i = 0; i < isize; i++) {
        //char c = idata[i];
        char c = *ibuf;
        if (c != '=') {                        
            if (type == LIB_CBASE32) {
                // Converch a char for Crockford's Base32
                c = base32_conv_crockford(c);
            }

            //sprintf(index_string, "%05b", (int) (strchr(encode_table, c) - encode_table));
            to_bin5(index_buf, (int) (strchr(encode_table, c) - encode_table));
            
            //index_buf[5] = '\0';
            //strcat(buf, index_buf); // very slowly

            memcpy(tmp, index_buf, 5);
            tmp += 5;
        }
        ibuf++;
    }

    buf[size] = '\0';

    //fprintf(stderr, "padding = %lu\n", padding);
    //fprintf(stderr, "size = %lu\n", size);
    //fprintf(stderr, "strlen = %lu\n", strlen(buf));

    char byte_buf[9];
    byte_buf[8] = '\0';
    tmp = buf;
    size_t psize = 0; // Processed size

    for (size_t i = 0; i < size; i += 8) {
        //strncpy(byte_buf, buf + i, 8);
        //strncpy(byte_buf, tmp, 8);
        //byte_buf[8] = '\0';
        memcpy(byte_buf, tmp, 8);
        
        //odata[i / 8] = (unsigned char) strtol(byte_buf, NULL, 2);
        //odata[psize] = (unsigned char) strtol(byte_buf, NULL, 2);

        *obuf = (unsigned char) strtol(byte_buf, NULL, 2);
        obuf++;
        psize++;
        tmp += 8;
    }

    //fprintf(stderr, "psize = %lu\n", psize);
    return psize;
}

int base32_decode(const char* idata, size_t isize, unsigned char* odata) {
    return base32_decode_type(LIB_BASE32, idata, isize, odata);
}

////

int lib_base32_encode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize) {
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

	size_t size = base32_encode_size_type(type, isize);
    //fprintf(stderr, "size  : %lu\n", size);
    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }
    memset(buf, 0, size);
	int retval = base32_encode_type(type, (unsigned char*) idata, isize, buf);
    //fprintf(stderr, "retval: %ld\n", retval);
    if (retval < 0) {
        free(buf);
        return -1;
    }
    buf[size] = '\0';
    
    *odata = buf;
    *osize = size;

	return 0;
}

int lib_base32_encode(const char* idata, size_t isize, char** odata, size_t* osize) {
    return lib_base32_encode_type(LIB_BASE32, idata, isize, odata, osize);
}

int lib_base32_decode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize) {
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

    while (isize > 0 && base32_is_spec(type, idata[isize - 1])) {
        isize--;
    }

    // Validate chars
    if (!base32_is_valid(type, base32_table_type(type), idata, isize)) {
        return -1;
    }

    size_t size = base32_decode_size(isize);
    //fprintf(stderr, "size  : %lu\n", size);
    char* buf = (char*) malloc(size + 1);
    if (!buf) {
        return -1;
    }
    memset(buf, 0, size);
	int retval = base32_decode_type(type, idata, isize, (unsigned char*) buf);
    //fprintf(stderr, "retval: %ld\n", retval);
    if (retval < 0) {
        free(buf);
        return -1;
    }
    buf[size] = '\0';

    *odata = buf;
    *osize = size;

	return 0;
}

int lib_base32_decode(const char* idata, size_t isize, char** odata, size_t* osize) {
    return lib_base32_decode_type(LIB_BASE32, idata, isize, odata, osize);
}

// https://medium.com/@at.kishor.k/demystifying-base32-an-in-depth-guide-to-this-encoding-standard-697b9426fc25

// Base32, Z-Base32, Base32Hex, Crockford’s Base32
// https://base32encode.com/blog/base32-alternatives/

// https://www.npmjs.com/package/crockford-base32
// https://github.com/dflydev/dflydev-base32-crockford/blob/master/src/Dflydev/Base32/Crockford/Crockford.php