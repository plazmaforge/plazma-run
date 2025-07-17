#ifndef PLAZMA_LIB_UUID_H
#define PLAZMA_LIB_UUID_H

#include <stdint.h>

// UUID Type definitions 
#define LIB_UUID_TYPE_NONE       0
#define LIB_UUID_TYPE_TIME       1
#define LIB_UUID_TYPE_SECURITY   2
#define LIB_UUID_TYPE_MD5        3
#define LIB_UUID_TYPE_RANDOM     4
#define LIB_UUID_TYPE_SHA1       5
#define LIB_UUID_TYPE_TIME_V6    6
#define LIB_UUID_TYPE_TIME_V7    7
#define LIB_UUID_TYPE_VENDOR     8

#define LIB_UUID_TIME_FORMAT_LOWER      "%8.8x-%4.4x-%4.4x-%2.2x%2.2x-"
#define LIB_UUID_NODE_FORMAT_LOWER      "%2.2x"

#define LIB_UUID_TIME_FORMAT_UPPER      "%8.8X-%4.4X-%4.4X-%2.2X%2.2X-"
#define LIB_UUID_NODE_FORMAT_UPPER      "%2.2X"

#define LIB_UUID_TIME_FORMAT_LOWER_PACK "%8.8x%4.4x%4.4x%2.2x%2.2x"
#define LIB_UUID_TIME_FORMAT_UPPER_PACK "%8.8X%4.4X%4.4X%2.2X%2.2X"

#define LIB_UUID_FORMAT_TYPE_LOWER      1
#define LIB_UUID_FORMAT_TYPE_UPPER      2
#define LIB_UUID_FORMAT_TYPE_LOWER_PACK 3
#define LIB_UUID_FORMAT_TYPE_UPPER_PACK 4

#define LIB_UUID_FORMAT_TYPE LIB_UUID_FORMAT_TYPE_LOWER

typedef uint64_t lib_uuid_time_t;

typedef struct {
  char node_id[6];
} lib_uuid_node_t;

typedef struct {

  /* Time (64 bits / 8 bytes)  */
  uint32_t  time_low;
  uint16_t  time_mid;
  uint16_t  time_hi_and_version;

  /* Clock (16 bits / 2 bytes) */
  uint8_t   clock_seq_hi_and_reserved;
  uint8_t   clock_seq_low;

  /* Node (48 bits / 6 bytes)  */
  uint8_t   node[6];

  /* Total 128 bits / 16 bytes */

} lib_uuid_t;


/* Name string is a fully-qualified domain name */
static lib_uuid_t NameSpace_DNS = { /* 6ba7b810-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b810,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
};

/* Name string is a URL */
static lib_uuid_t NameSpace_URL = { /* 6ba7b811-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b811,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
};

/* Name string is an ISO OID */
static lib_uuid_t NameSpace_OID = { /* 6ba7b812-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b812,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
};

/* Name string is an X.500 DN (in DER or a text output format) */
static lib_uuid_t NameSpace_X500 = { /* 6ba7b814-9dad-11d1-80b4-00c04fd430c8 */
       0x6ba7b814,
       0x9dad,
       0x11d1,
       0x80, 0xb4, 0x00, 0xc0, 0x4f, 0xd4, 0x30, 0xc8
};

bool lib_uuid_is_lower_format_default();

bool lib_uuid_is_upper_format_default();

bool lib_uuid_is_lower_format(int format);

bool lib_uuid_is_upper_format(int format);

bool lib_uuid_is_pack_format(int format);

size_t lib_uuid_get_format_size(int format);

//// GEN

/**
 * Generate UUID structure
 */
int lib_uuid_gen_uuid_v(lib_uuid_t* uuid, int version);

/**
 * Generate UUID data
 */
int lib_uuid_gen_data_v(unsigned char value[16], int version);

/**
 * Generate UUID string
 */
int lib_uuid_gen_str_v(char* str, int version);

/**
 * Generate UUID string in lower case
 */
int lib_uuid_gen_strl_v(char* str, int version);

/**
 * Generate UUID string in upper case
 */
int lib_uuid_gen_stru_v(char* str, int version);

/**
 * Generate UUID pack (without '-') string
 */
int lib_uuid_gen_pstr_v(char* str, int version);

/**
 * Generate UUID pack (without '-') string in lower case
 */
int lib_uuid_gen_pstrl_v(char* str, int version);

/**
 * Generate UUID pack (without '-') string in upper case
 */
int lib_uuid_gen_pstru_v(char* str, int version);

/**
 * Generate UUID string by format type
 */
int lib_uuid_gen_strf_v(int format, char* str, int version);

//// CREATE

/**
 * Generator a UUID
 */
void lib_uuid_create(lib_uuid_t* uuid);

void lib_uuid_create_v1(lib_uuid_t* uuid);

void lib_uuid_create_v2(lib_uuid_t* uuid); // Not implemnted yet

void lib_uuid_create_v3(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen);

void lib_uuid_create_v4(lib_uuid_t* uuid); // Not implemnted yet

void lib_uuid_create_v5(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen);

void lib_uuid_create_v6(lib_uuid_t* uuid); // Not implemnted yet

void lib_uuid_create_v7(lib_uuid_t* uuid);

////

void lib_uuid_create_md5(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen);

void lib_uuid_create_sha1(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen);

void lib_uuid_reset();

/**
 * Print a UUID
 */
int lib_uuid_print(lib_uuid_t u);

/**
 * Print a UUID by format type
 */
int lib_uuid_printf(int type, lib_uuid_t u);

/**
 * Print a UUID in the supplied buffer
 */

int lib_uuid_sprint(char* str, lib_uuid_t u);

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_sprintf(int type, char* str, lib_uuid_t u);

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_snprint(char* str, size_t size, lib_uuid_t u);

/**
 * Print a UUID in the supplied buffer by format type
 */
int lib_uuid_snprintf(int type, char* str, size_t size, lib_uuid_t u);

void lib_uuid_pack(const lib_uuid_t* uuid, unsigned char value[16]);

void lib_uuid_unpack(const unsigned char value[16], lib_uuid_t* uuid);

#endif // PLAZMA_LIB_UUID_H
