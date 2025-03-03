#ifndef PLAZMA_LIB_BASE32_H
#define PLAZMA_LIB_BASE32_H

#define LIB_BASE32    0
#define LIB_BASE32U   1 /* Base32 (upper)     */
#define LIB_BASE32L   2 /* Base32 (lower)     */
#define LIB_ZBASE32   3 /* Z-Base32           */
#define LIB_BASE32HEX 4 /* Base32Hex          */
#define LIB_CBASE32   5 /* Crockford’s Base32 */

int lib_base32_encode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_encode(const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_decode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_decode(const char* idata, size_t isize, char** odata, size_t* osize);

#endif // PLAZMA_LIB_BASE32_H