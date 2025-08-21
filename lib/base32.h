#ifndef PLAZMA_LIB_BASE32_H
#define PLAZMA_LIB_BASE32_H

#define LIB_BASE32    320
#define LIB_BASE32U   321 /* Base32 (upper)     */
#define LIB_BASE32L   322 /* Base32 (lower)     */
#define LIB_ZBASE32   323 /* Z-Base32           */
#define LIB_BASE32HEX 324 /* Base32Hex          */
#define LIB_CBASE32   325 /* Crockford’s Base32 */

#define LIB_BASE32_MIN LIB_BASE32
#define LIB_BASE32_MAX LIB_CBASE32

int lib_base32_encode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_encode(const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_decode_type(int type, const char* idata, size_t isize, char** odata, size_t* osize);

int lib_base32_decode(const char* idata, size_t isize, char** odata, size_t* osize);

#endif // PLAZMA_LIB_BASE32_H