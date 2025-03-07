#ifndef PLAZMA_LIB_UUID_H
#define PLAZMA_LIB_UUID_H

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

/**
 * Generator a UUID
 */
int lib_uuid_create(lib_uuid_t* uuid);

void lib_uuid_reset();

/**
 * Print a UUID
 */
int lib_uuid_print(lib_uuid_t u);

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_snprint(char* str, size_t size, lib_uuid_t u);

void lib_uuid_pack(const lib_uuid_t* uuid, unsigned char value[16]);

void lib_uuid_unpack(const unsigned char value[16], lib_uuid_t* uuid);

#endif // PLAZMA_LIB_UUID_H
