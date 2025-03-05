#ifndef PLAZMA_LIB_UUID_H
#define PLAZMA_LIB_UUID_H

typedef uint64_t lib_uuid_time_t;

typedef struct {
  char node_id[6];
} lib_uuid_node_t;

typedef struct {
  uint32_t  time_low;
  uint16_t  time_mid;
  uint16_t  time_hi_and_version;
  uint8_t   clock_seq_hi_and_reserved;
  uint8_t   clock_seq_low;
  uint8_t   node[6];
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

#endif // PLAZMA_LIB_UUID_H
