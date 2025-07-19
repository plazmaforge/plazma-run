#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <winsock.h>
#else
#include <sys/time.h>
//#include <sys/sysinfo.h>
#include <fcntl.h>  /* open */
#include <unistd.h> /* read/write, close */
#endif

#include "uuid.h"

/**
 * Set the following to the number of 100ns ticks of the actual
 * resolution of your system's clock
 */
#define LIB_UUIDS_PER_TICK 1024

/** 
 * System dependent call to get the current system time.
 * Returned as 100ns ticks since UUID epoch,
 * but resolution may be less than 100ns.
 */
#ifdef WIN32
#define I64(C) C
#else
#define I64(C) C##LL
#endif

#define LIB_UUID_RANDOM_TYPE_BASE   1
#define LIB_UUID_RANDOM_TYPE_NATIVE 2

#define LIB_UUID_RANDOM_TYPE LIB_UUID_RANDOM_TYPE_NATIVE

static int inited_random = 0;
static int inited_time   = 0;
static int inited_node   = 0;
  
/* Context: Time */
static lib_uuid_time_t time_last;
static uint16_t uuids_this_tick;

/* Context: Node */
static lib_uuid_node_t saved_node;

static uint16_t next_random();

bool lib_uuid_is_lower_format_default() {
  return lib_uuid_is_lower_format(LIB_UUID_FORMAT_TYPE);
}

bool lib_uuid_is_upper_format_default() {
  return lib_uuid_is_upper_format(LIB_UUID_FORMAT_TYPE);
}

bool lib_uuid_is_lower_format(int format) {
  return !lib_uuid_is_upper_format(format);
}

bool lib_uuid_is_upper_format(int format) {
  return (format == LIB_UUID_FORMAT_TYPE_UPPER || format == LIB_UUID_FORMAT_TYPE_UPPER_PACK);
}

bool lib_uuid_is_pack_format(int format) {
  return (format == LIB_UUID_FORMAT_TYPE_LOWER_PACK || format == LIB_UUID_FORMAT_TYPE_UPPER_PACK);
}

size_t lib_uuid_get_format_size(int format) {
  return lib_uuid_is_pack_format(format) ? 32 : 36;
}

/**
 * Base random generator
 */
static int get_random_base(char seed[16]) {
  uint16_t value;
  int i = 0;
  do {
    value = next_random();
    seed[i++] = value & 0xff;
    seed[i++] = value >> 8;
  } while (i < 14);
  //fprintf(stderr, "get_random_base\n");
  return 0;
}

/* 
 Difference between FILETIME (Windows) epoch and Unix epoch in seconds

 FILETIME epoch: January 1, 1601
 UNIX epoch:     January 1, 1970

 1970 year - 1601 year = 369 years
 1 year = 365 days
 1 day = 86400 seconds (24 hours * 60 minutes * 60 seconds)
 89 days - leap days

 (369 years * 365 days + 89 days) * 86400 seconds = 11644473600 seconds
*/
const uint64_t FTIME_UTIME_EPOCH_OFFSET_SEC = 11644473600;

/* 
 Difference between Gregorian epoch and Unix epoch in seconds

 Gregorian epoch: October 15, 1582
 UNIX epoch:      January 1, 1970

 1970 year - 1582 year = 388 years - 1 year (1582) = 387 years
 1 year = 365 days
 1 day = 86400 seconds (24 hours * 60 minutes * 60 seconds)
 172 days - leap days

 (387 years * 365 days + 172 days) * 86400 seconds = 12219292800 seconds
*/
const uint64_t GTIME_UTIME_EPOCH_OFFSET_SEC = 12219292800;

/* Nanoseconds per microseconds */
const uint64_t NSEC_PER_USEC = 1000;

/* Microseconds per seconds */
const uint64_t USEC_PER_SEC = 1000000;

/* Nanoseconds per seconds */
const uint64_t NSEC_PER_SEC = 1000000000;

//// OS

#ifdef WIN32

static void gettimeofday(struct timeval *tv, void *tz) {

  FILETIME ftime; // Windows time in 100ns (1 unit of FILETIME is 100ns)
  uint64_t time;  // Unix time in microseconds

  GetSystemTimeAsFileTime(&ftime);

  /* Set Windows time in 100ns */
  time = (((uint64_t) ftime.dwHighDateTime << 32)
  + (uint64_t) ftime.dwLowDateTime);

  if (time) {

    /* Convert Windows time to microseconds */    
    time /= 10; 

    /* Convert Windows time to Unix time */
    time -= FTIME_UTIME_EPOCH_OFFSET_SEC * USEC_PER_SEC; 
  }

  /* Set seconds and microseconds */
  tv->tv_sec = time / USEC_PER_SEC;
  tv->tv_usec = time % USEC_PER_SEC;

}

// static void get_system_time(lib_uuid_time_t* uuid_time) {

//   ULARGE_INTEGER time;

//   /* NT keeps time in FILETIME format which is 100ns ticks since
//      Jan 1, 1601. 
     
//      UUIDs use time in 100ns ticks since Oct 15, 1582.
//      The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
//      + 18 years and 5 leap days. */
//   GetSystemTimeAsFileTime((FILETIME*) &time);
//   time.QuadPart +=

//     (unsigned __int64) (1000 * 1000 * 10)               // seconds
//     * (unsigned __int64) (60 * 60 * 24)                 // days
//     * (unsigned __int64) (17 + 30 + 31 + 365 * 18 + 5); // # of days
//   *uuid_time = time.QuadPart;
// }

static int get_random_native(char seed[16]) {
  // TODO
  return -1;
}

// https://www.ietf.org/rfc/rfc4122.txt
// get random info

/*
void get_random_native_2(char seed[16]) {
    //MD5_CTX c;
    struct {
        MEMORYSTATUS m;
        SYSTEM_INFO s;
        FILETIME t;
        LARGE_INTEGER pc;
        DWORD tc;
        DWORD l;
        char hostname[MAX_COMPUTERNAME_LENGTH + 1];
    } r;

    //MD5Init(&c);
    GlobalMemoryStatus(&r.m);
    GetSystemInfo(&r.s);
    GetSystemTimeAsFileTime(&r.t);
    QueryPerformanceCounter(&r.pc);
    r.tc = GetTickCount();
    r.l = MAX_COMPUTERNAME_LENGTH + 1;
    GetComputerName(r.hostname, &r.l);
    //MD5Update(&c, &r, sizeof r);
    //MD5Final(seed, &c);
}
*/

#else

// get random info
static int get_random_native(char seed[16]) {

  /* we aren't all that picky, and we would rather not block so we
    will use urandom */
  int fd = open("/dev/urandom", O_RDONLY);

  if (fd != -1) {
    read(fd, seed, 16);
    close(fd);
    //fprintf(stderr, "get_random_native\n");
    return 0;
  }

  return -1;
}

// https://www.ietf.org/rfc/rfc4122.txt
/*
void get_random_native_2(char seed[16]) {
    //MD5_CTX c;
    struct {
        struct sysinfo s;
        struct timeval t;
        char hostname[257];
    } r;

    //MD5Init(&c);
    sysinfo(&r.s);
    gettimeofday(&r.t, (struct timezone *)0);
    gethostname(r.hostname, 256);
    //MD5Update(&c, &r, sizeof r);
    //MD5Final(seed, &c);
}
*/
#endif

//// COMMON

/**
 * Convert time to nano seconds 
 */
static uint64_t to_time_ns(struct timeval *tv) {
  return ((uint64_t) tv->tv_sec * NSEC_PER_SEC)
    + ((uint64_t) tv->tv_usec * NSEC_PER_USEC);
}

static uint64_t get_time_ns() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return to_time_ns(&tv);
}

static void get_system_time(lib_uuid_time_t* uuid_time) {


  /* Offset between UUID formatted times and Unix formatted times.
     UUID UTC base time is October 15, 1582.
     Unix base time is January 1, 1970.*/

  //struct timeval tv;
  //gettimeofday(&tv, NULL);
  //*uuid_time = ((uint64_t) tv.tv_sec * 10000000)
  //+ ((uint64_t) tv.tv_usec * 10)
  //+ I64(0x01B21DD213814000);

  //fprintf(stderr, "0: %llu\n", *uuid_time);
  //lib_uuid_time_t t = get_time_ns();

  //*uuid_time = to_time_ns(&tv) / 100 + I64(0x01B21DD213814000);
  //*uuid_time = get_time_ns() / 100 + I64(0x01B21DD213814000);
  //fprintf(stderr, "1: %llu\n", *uuid_time);

  //*uuid_time = to_time_ns(&tv) + (GTIME_UTIME_EPOCH_OFFSET_SEC * NSEC_PER_SEC);
  //*uuid_time = get_time_ns() + (GTIME_UTIME_EPOCH_OFFSET_SEC * NSEC_PER_SEC);

  /* Convert Unix time to Gregorian time */
  *uuid_time = get_time_ns() + (GTIME_UTIME_EPOCH_OFFSET_SEC * NSEC_PER_SEC);

  /* Convert to 100ns */
  *uuid_time /= 100; 

  //fprintf(stderr, "2: %llu\n", *uuid_time);
}

/* Sample code, not for use in production; see RFC 1750 */
static int get_random(char seed[16]) {

  // Random Native has priority
  if (LIB_UUID_RANDOM_TYPE == LIB_UUID_RANDOM_TYPE_NATIVE 
      && get_random_native(seed) == 0) {
    return 0;
  }

  return get_random_base(seed);
}

/** 
 * Generate a crypto-quality random number.
 */
static uint16_t next_random() {

  //static int inited_random = 0;
  lib_uuid_time_t time_now;

  if (!inited_random) {
    //fprintf(stderr, "Start init random...\n");
    get_system_time(&time_now);
    time_now = time_now / LIB_UUIDS_PER_TICK;
    srand((unsigned int) (((time_now >> 32) ^ time_now) & 0xffffffff));
    inited_random = 1;
  }
  //fprintf(stderr, "next_random\n");

  return (uint16_t) rand();
}

static void fill_random(uint8_t* value, int num) {
    uint8_t* out = value;
    for (int i = 0; i < num; i++) {
       *out = (uint8_t) next_random();
       out++;
    }
}

/**
 * Get time as 60-bit 100ns ticks since UUID epoch.
 * Compensate for the fact that real clock resolution is less than 100ns.
 */
static void get_current_time(lib_uuid_time_t* timestamp) {

  lib_uuid_time_t time_now;

  if (!inited_time) {
    //fprintf(stderr, "Start init time...\n");
    get_system_time(&time_now);
    uuids_this_tick = LIB_UUIDS_PER_TICK;
    inited_time = 1;
  }

  for (;;) {
    get_system_time(&time_now);
    //fprintf(stderr, "get_system_time-%d-%llu-%llu\n", i, time_now, time_last);

    /* if clock reading changed since last UUID generated, */    
    if (time_last != time_now) {
    //if (time_now - time_last >= 100) { // ALT
      /* reset count of uuids gen'd with this clock reading */
      uuids_this_tick = 0;
      time_last = time_now;
      break;
    }

    // ALT
    // if (uuids_this_tick >= LIB_UUIDS_PER_TICK) {
    //   uuids_this_tick = 0;
    //   time_last = time_now;
    //   break;
    // }

    if (uuids_this_tick < LIB_UUIDS_PER_TICK) {
      uuids_this_tick++;
      break;
    }

    /* going too fast for our clock; spin */
  }

  /* add the count of uuids to low order bits of the clock reading */
  *timestamp = time_now + uuids_this_tick;
}

/**
 * System dependent call to get IEEE node ID.
 * This sample implementation generates a random node ID.
 */
static void get_node_id(lib_uuid_node_t* node) {
  char seed[16];

  if (!inited_node) {
    //fprintf(stderr, "Start init node...\n");
    get_random(seed);
    seed[0] |= 0x01;
    memcpy(&saved_node, seed, sizeof(saved_node));
  }
  inited_node = 1;
  *node = saved_node;
  //node = saved_node; // ALT
}

//// FORMAT

/**
 * Make a UUID from the timestamp, clockseq, and node ID
 */
static void format_uuid_v1(
  lib_uuid_t* uuid,
  uint16_t clock_seq,
  lib_uuid_time_t timestamp,
  lib_uuid_node_t node) {

  /* Construct a version 1 uuid with the information we've gathered
     plus a few constants. */
  uuid->time_low = (unsigned long) (timestamp & 0xFFFFFFFF);
  uuid->time_mid = (unsigned short) ((timestamp >> 32) & 0xFFFF);
  uuid->time_hi_and_version = (unsigned short) ((timestamp >> 48) & 0x0FFF);
  uuid->time_hi_and_version |= (1 << 12);
  
  uuid->clock_seq_low = clock_seq & 0xFF;
  uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
  uuid->clock_seq_hi_and_reserved |= 0x80;
  memcpy(&uuid->node, &node, sizeof(uuid->node));
}

/* Make a UUID from a (pseudo)random 128-bit
   number */
static void format_uuid_v3or5(lib_uuid_t* uuid, unsigned char hash[16], int version) {

	/* convert UUID to local byte order */
	memcpy(uuid, hash, sizeof(*uuid));
	uuid->time_low = ntohl(uuid->time_low);
	uuid->time_mid = ntohs(uuid->time_mid);
	uuid->time_hi_and_version = ntohs(uuid->time_hi_and_version);

	/* put in the variant and version bits */
	uuid->time_hi_and_version &= 0x0FFF;
	uuid->time_hi_and_version |= (version << 12);
	uuid->clock_seq_hi_and_reserved &= 0x3F;
	uuid->clock_seq_hi_and_reserved |= 0x80;
}

//// GEN

/**
 * Generate UUID structure
 */
int lib_uuid_gen_uuid_v(lib_uuid_t* uuid, int version) {
  if (!uuid) {
    return -1;
  }
  if (version <= 0) {
    return -1;
  }
  if (version == 1) {
    lib_uuid_create_v1(uuid);
  }
  if (version == 2) {
    lib_uuid_create_v2(uuid); // Not implemented yet
  }
  if (version == 3) {
    lib_uuid_create_v3(uuid, NameSpace_DNS, "www.widgets.com", 15);
  }
  if (version == 4) {
    lib_uuid_create_v4(uuid); // Not implemented yet
  }
  if (version == 5) {
    lib_uuid_create_v5(uuid, NameSpace_DNS, "www.widgets.com", 15);
  }
  if (version == 6) {
    lib_uuid_create_v6(uuid);  // Not implemented yet
  }
  if (version == 7) {
    lib_uuid_create_v7(uuid);
  }
  return -1;  
}

/**
 * Generate UUID data
 */
int lib_uuid_gen_data_v(unsigned char value[16], int version) {
  if (!value) {
    return -1;
  }
  if (version <= 0) {
    return -1;
  }
  lib_uuid_t uuid;
  int retval = lib_uuid_gen_uuid_v(&uuid, version);
  if (retval != 0) {
    return retval;
  }
  lib_uuid_pack(&uuid, value);
  return 0;
}

/**
 * Generate UUID string
 */
int lib_uuid_gen_str_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE, str, version);
}

/**
 * Generate UUID string in lower case
 */
int lib_uuid_gen_strl_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE_LOWER_PACK, str, version);
}

/**
 * Generate UUID string in upper case
 */
int lib_uuid_gen_stru_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE_UPPER_PACK, str, version);
}

/**
 * Generate UUID pack (without '-') string
 */
int lib_uuid_gen_pstr_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE_LOWER_PACK, str, version);
}

/**
 * Generate UUID pack (without '-') string in lower case
 */
int lib_uuid_gen_pstrl_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE_LOWER_PACK, str, version);
}

/**
 * Generate UUID pack (without '-') string in upper case
 */
int lib_uuid_gen_pstru_v(char* str, int version) {
  return lib_uuid_gen_strf_v(LIB_UUID_FORMAT_TYPE_UPPER_PACK, str, version);
}

/**
 * Generate UUID string by format type
 */
int lib_uuid_gen_strf_v(int format, char* str, int version) {
  if (!str) {
    return -1;
  }
  if (version <= 0) {
    return -1;
  }
  lib_uuid_t uuid;
  int retval = lib_uuid_gen_uuid_v(&uuid, version);
  if (retval != 0) {
    return retval;
  }
  size_t size = lib_uuid_get_format_size(format);
  retval = lib_uuid_snprintf(format, str, size + 1, uuid);
  if (retval <= 0) {
    return -1;
  }
  return 0;
}

//// CREATE

void lib_uuid_create(lib_uuid_t* uuid) {
  lib_uuid_create_v1(uuid); // by default
}

/**
 * Generator a UUID
 */
void lib_uuid_create_v1(lib_uuid_t* uuid) {

  lib_uuid_time_t timestamp;
  uint16_t clock_seq;
  lib_uuid_node_t node;

  /* get time, node ID, saved state from non-volatile storage */
  get_current_time(&timestamp);
  get_node_id(&node);
  //get_node_id(node); // ALT

  /* for us clock_seq is always to be random as we have no state */
  clock_seq = next_random();

  /* stuff fields into the UUID */
  format_uuid_v1(uuid, clock_seq, timestamp, node);
}

void lib_uuid_create_v2(lib_uuid_t* uuid) {
  // Not implemnted yet
} 

void lib_uuid_create_v3(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen) {
  lib_uuid_create_md5(uuid, nsid, name, namelen);
}

void lib_uuid_create_v4(lib_uuid_t* uuid) {
  // Not implemnted yet
} 

void lib_uuid_create_v5(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen) {
  lib_uuid_create_sha1(uuid, nsid, name, namelen);
}

void lib_uuid_create_v6(lib_uuid_t* uuid) {
  // Not implemented yet
}

////

void lib_uuid_create_v7(lib_uuid_t* uuid) {
    uint8_t value[16];
  
    // random bytes
    //int err = getentropy(value, 16);
    //if (err != EXIT_SUCCESS) {
    //    return EXIT_FAILURE;
    //}

    // current timestamp in ms
    //struct timespec ts;
    //int ok = timespec_get(&ts, TIME_UTC);
    //if (ok == 0) {
    //    return EXIT_FAILURE;
    //}
    //uint64_t timestamp = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    // TODO: Stub
    //for (int i = 0; i < 16; i++) {
    //   value[i] = (uint8_t) next_random();
    //}

    lib_uuid_time_t timestamp;
    get_current_time(&timestamp);
    timestamp /= 10000; // convert to ns (?)

    // timestamp
    value[0] = (timestamp >> 40) & 0xFF;
    value[1] = (timestamp >> 32) & 0xFF;
    value[2] = (timestamp >> 24) & 0xFF;
    value[3] = (timestamp >> 16) & 0xFF;
    value[4] = (timestamp >> 8) & 0xFF;
    value[5] = timestamp & 0xFF;

    fill_random(value + 6, 10);

    // version and variant
    value[6] = (value[6] & 0x0F) | 0x70;
    value[8] = (value[8] & 0x3F) | 0x80;
    
    lib_uuid_unpack(value, uuid);

}

static void lib_uuid_hash_v(
  unsigned char* hash,
  lib_uuid_t* nsid, 
  size_t ssize, 
  void* name, 
  size_t nsize,
  int version) {

  //// TODO: Stub
  size_t count = 0;
  if (version == LIB_UUID_TYPE_MD5) {
    count = 16;
  } else if (version == LIB_UUID_TYPE_SHA1) {
    count = 20;
  }
  for (int i = 0; i < count; i++) {
    hash[i] = (unsigned char) next_random();
  }
  ////

  //HASH_CTX c;
	//HASH_Init(&c);
	//HASH_Update(&c, &net_nsid, sizeof(net_nsid));
	//HASH_Update(&c, name, namelen);
	//HASH_Final(hash, &c);

}

static void lib_uuid_create_v3or5(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen, int version) {
	
  size_t count = 0;
  if (version == LIB_UUID_TYPE_MD5) {
    count = 16;
  } else if (version == LIB_UUID_TYPE_SHA1) {
    count = 20;
  }

	unsigned char hash[count];
	lib_uuid_t net_nsid;

	/*
	 * put name space ID in network byte order so it hashes the same no
	 * matter what endian machine we're on
	 */
	net_nsid = nsid;
	net_nsid.time_low = htonl(net_nsid.time_low);
	net_nsid.time_mid = htons(net_nsid.time_mid);
	net_nsid.time_hi_and_version = htons(net_nsid.time_hi_and_version);

  lib_uuid_hash_v(hash, &net_nsid, sizeof(net_nsid), name, namelen, version);

	/* the hash is in network byte order at this point */
	format_uuid_v3or5(uuid, hash, version);
}

/**
 * Create a version 3 (MD5) UUID using a "name" from a "name space"
 */
void lib_uuid_create_md5(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen) {
  lib_uuid_create_v3or5(uuid, nsid, name, namelen, LIB_UUID_TYPE_MD5);	
}

void lib_uuid_create_sha1(lib_uuid_t* uuid, lib_uuid_t nsid, void* name, size_t namelen) {
  lib_uuid_create_v3or5(uuid, nsid, name, namelen, LIB_UUID_TYPE_SHA1);
}

////

void lib_uuid_reset() {
  inited_random = 0;
  inited_time   = 0;
  inited_node   = 0;

  time_last = 0;
  uuids_this_tick = 0;
}

static const char* lib_uuid_get_time_format(int type) {
  if (type == LIB_UUID_FORMAT_TYPE_LOWER) {
    return LIB_UUID_TIME_FORMAT_LOWER;
  }
  if (type == LIB_UUID_FORMAT_TYPE_UPPER) {
    return LIB_UUID_TIME_FORMAT_UPPER;
  }
  if (type == LIB_UUID_FORMAT_TYPE_LOWER_PACK) {
    return LIB_UUID_TIME_FORMAT_LOWER_PACK;
  }
  if (type == LIB_UUID_FORMAT_TYPE_UPPER_PACK) {
    return LIB_UUID_TIME_FORMAT_UPPER_PACK;
  }
  return LIB_UUID_TIME_FORMAT_LOWER;
}

static const char* lib_uuid_get_node_format(int type) {
  if (type == LIB_UUID_FORMAT_TYPE_LOWER
   || type == LIB_UUID_FORMAT_TYPE_LOWER_PACK) {
    return LIB_UUID_NODE_FORMAT_LOWER;
  }
  if (type == LIB_UUID_FORMAT_TYPE_UPPER 
   || type == LIB_UUID_FORMAT_TYPE_UPPER_PACK) {
    return LIB_UUID_NODE_FORMAT_UPPER;
  }
  return LIB_UUID_NODE_FORMAT_LOWER;
}

/**
 * Print a UUID
 */
int lib_uuid_print(lib_uuid_t u) {
  return lib_uuid_printf(LIB_UUID_FORMAT_TYPE, u);
}

/**
 * Print a UUID by format type
 */
int lib_uuid_printf(int type, lib_uuid_t u) {
  const char* time_format = lib_uuid_get_time_format(type);
  const char* node_format = lib_uuid_get_node_format(type);

  //int pos = printf("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-",
  int pos = printf(time_format,
     u.time_low,
     u.time_mid,
     u.time_hi_and_version, 
     u.clock_seq_hi_and_reserved,
     u.clock_seq_low);
  
  for (int i = 0; i < 6; i++) {
    //pos += printf("%2.2x", u.node[i]);
    pos += printf(node_format, u.node[i]);
  }    
  
  //pos += printf("\n");
  return pos;
}

////

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_sprint(char* str, lib_uuid_t u) {
  if (!str) {
    return 0;
  }
  return lib_uuid_snprintf(LIB_UUID_FORMAT_TYPE, str, strlen(str), u);
}

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_sprintf(int type, char* str, lib_uuid_t u) {
  if (!str) {
    return 0;
  }
  return lib_uuid_snprintf(type, str, strlen(str), u);
}

////

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_snprint(char* str, size_t size, lib_uuid_t u) {
  return lib_uuid_snprintf(LIB_UUID_FORMAT_TYPE, str, size, u);
}

/**
 * Print a UUID in the supplied buffer by format type
 */
int lib_uuid_snprintf(int type, char* str, size_t size, lib_uuid_t u) {  
  if (!str) {
    return 0;
  }

  size_t format_size = lib_uuid_get_format_size(type);
  format_size++; // NULL

  if (!str || size < format_size + 1) { // < 38
    return 0;
  }

  const char* time_format = lib_uuid_get_time_format(type);
  const char* node_format = lib_uuid_get_node_format(type);

  char *tmp = str;

  /* perhaps this is a trifle optimistic but what the heck */
  int pos = sprintf(tmp,
	  //"%8.8x-%4.4x-%4.4x-%2.2x%2.2x-",
    time_format,
	  u.time_low,
	  u.time_mid,
	  u.time_hi_and_version,
	  u.clock_seq_hi_and_reserved,
	  u.clock_seq_low);

  tmp += 24;
  for (int i = 0; i < 6; i++) {
    //pos += sprintf(tmp, "%2.2x", u.node[i]);
    pos += sprintf(tmp, node_format, u.node[i]);
    tmp += 2;
  }
  *tmp = 0;

  return pos;
}

void lib_uuid_pack(const lib_uuid_t* uuid, unsigned char value[16]) {
  uint32_t tmp;
  unsigned char* out = value;

  /* time_low */
  tmp = uuid->time_low;
  out[3] = (unsigned char) tmp;
  tmp >>= 8;
  out[2] = (unsigned char) tmp;
  tmp >>= 8;
  out[1] = (unsigned char) tmp;
  tmp >>= 8;
  out[0] = (unsigned char) tmp;

  /* time_mid */
  tmp = uuid->time_mid;
  out[5] = (unsigned char) tmp;
  tmp >>= 8;
  out[4] = (unsigned char) tmp;

  /* time_hi_and_version */
  tmp = uuid->time_hi_and_version;
  out[7] = (unsigned char) tmp;
  tmp >>= 8;
  out[6] = (unsigned char) tmp;

  /* clock_seq_hi_and_reserved */
  tmp = uuid->clock_seq_hi_and_reserved;
  out[8] = (unsigned char) tmp;

  /* clock_seq_low */
  tmp = uuid->clock_seq_low;
  out[9] = (unsigned char) tmp;

  memcpy(out + 10, uuid->node, 6);
}

void lib_uuid_unpack(const unsigned char value[16], lib_uuid_t* uuid) {
  const uint8_t* ptr = value;
  uint32_t tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  tmp = (tmp << 8) | *ptr++;
  tmp = (tmp << 8) | *ptr++;
  uuid->time_low = tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  uuid->time_mid = tmp;

  tmp = *ptr++;
  tmp = (tmp << 8) | *ptr++;
  uuid->time_hi_and_version = tmp;

  tmp = *ptr++;
  uuid->clock_seq_hi_and_reserved = tmp;

  tmp = *ptr++;
  uuid->clock_seq_low = tmp;

  memcpy(uuid->node, ptr, 6);
}

/*

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

int uuidv7(uint8_t* value) {
    // random bytes
    int err = getentropy(value, 16);
    if (err != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    // current timestamp in ms
    struct timespec ts;
    int ok = timespec_get(&ts, TIME_UTC);
    if (ok == 0) {
        return EXIT_FAILURE;
    }
    uint64_t timestamp = (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;

    // timestamp
    value[0] = (timestamp >> 40) & 0xFF;
    value[1] = (timestamp >> 32) & 0xFF;
    value[2] = (timestamp >> 24) & 0xFF;
    value[3] = (timestamp >> 16) & 0xFF;
    value[4] = (timestamp >> 8) & 0xFF;
    value[5] = timestamp & 0xFF;

    // version and variant
    value[6] = (value[6] & 0x0F) | 0x70;
    value[8] = (value[8] & 0x3F) | 0x80;

    return EXIT_SUCCESS;
}

int main() {
    uint8_t uuid_val[16];
    uuidv7(uuid_val);
    for (size_t i = 0; i < 16; i++) {
        printf("%02x", uuid_val[i]);
    }
    printf("\n");
}

*/

/*

// UUID Type definitions 
#define UUID_TYPE_DCE_NIL        0
#define UUID_TYPE_DCE_TIME       1
#define UUID_TYPE_DCE_SECURITY   2
#define UUID_TYPE_DCE_MD5        3
#define UUID_TYPE_DCE_RANDOM     4
#define UUID_TYPE_DCE_SHA1       5
#define UUID_TYPE_DCE_TIME_V6    6
#define UUID_TYPE_DCE_TIME_V7    7
#define UUID_TYPE_DCE_VENDOR     8

////

??: void uuid_generate(uuid_t out);

V1: void uuid_generate_time(uuid_t out);
V2: int uuid_generate_time_safe(uuid_t out);
V3: void uuid_generate_md5(uuid_t out, const uuid_t ns, const char *name, size_t len);
V4: void uuid_generate_random(uuid_t out);
V5: void uuid_generate_sha1(uuid_t out, const uuid_t ns, const char *name, size_t len);
V6: void uuid_generate_time_v6(uuid_t out);
V7: void uuid_generate_time_v7(uuid_t out);

*/

// https://www.ietf.org/rfc/rfc4122.txt
// https://github.com/HewlettPackard/netperf/blob/master/src/net_uuid.c
// https://github.com/zhicheng/uuid/blob/master/uuid.c

// time v6, v7, UUID v3, v5
// https://github.com/util-linux/util-linux/blob/master/libuuid/src/gen_uuid.c
// https://github.com/util-linux/util-linux/blob/master/libuuid/src/uuid_time.c

// WIN32: gettimeofday()
// https://web.archive.org/web/20130406033313/http://suacommunity.com/dictionary/gettimeofday-entry.php
// https://stackoverflow.com/questions/1676036/what-should-i-use-to-replace-gettimeofday-on-windows/68429021#68429021
// https://www.nu42.com/2021/07/windows-c-time-in-nanoseconds.html