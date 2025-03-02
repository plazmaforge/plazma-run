#ifndef PLAZMA_LIB_BASE32_H
#define PLAZMA_LIB_BASE32_H

#define LIB_BASE32    0
#define LIB_BASE32U   1 /* Base32 (upper)     */
#define LIB_BASE32L   2 /* Base32 (lower)     */
#define LIB_ZBASE32   3 /* Z-Base32           */
#define LIB_BASE32HEX 4 /* Base32Hex          */
#define LIB_CBASE32   5 /* Crockford’s Base32 */

int lib_base32_encode_type(int type, const char* data, size_t len, char** odata, size_t* out_len);

int lib_base32_encode(const char* data, size_t len, char** odata, size_t* out_len);

int lib_base32_decode_type(int type, const char* data, size_t len, char** odata, size_t* out_len);

int lib_base32_decode(const char* data, size_t len, char** odata, size_t* out_len);

#endif // PLAZMA_LIB_BASE32_H