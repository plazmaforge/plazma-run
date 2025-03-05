#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <fcntl.h>  /* open */
#include <unistd.h> /* read/write, close */
#endif

#include "uuid.h"

/**
 * Set the following to the number of 100ns ticks of the actual
 * resolution of your system's clock
 */
#define UUIDS_PER_TICK 1024

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

static int inited_random = 0;
static int inited_time   = 0;
static int inited_node   = 0;
  
static uint16_t true_random();

//// OS

#ifdef WIN32

static void get_system_time(lib_uuid_time_t* uuid_time) {

  ULARGE_INTEGER time;

  /* NT keeps time in FILETIME format which is 100ns ticks since
     Jan 1, 1601. UUIDs use time in 100ns ticks since Oct 15, 1582.
     The difference is 17 Days in Oct + 30 (Nov) + 31 (Dec)
     + 18 years and 5 leap days. */
  GetSystemTimeAsFileTime((FILETIME*) &time);
  time.QuadPart +=

    (unsigned __int64) (1000 * 1000 * 10)               // seconds
    * (unsigned __int64) (60 * 60 * 24)                 // days
    * (unsigned __int64) (17 + 30 + 31 + 365 * 18 + 5); // # of days
  *uuid_time = time.QuadPart;
}

/* Sample code, not for use in production; see RFC 1750 */
static void get_random_info(char seed[16]) {
  uint16_t myrand;
  int i;

  i = 0;
  do {
    myrand = true_random();
    seed[i++] = myrand & 0xff;
    seed[i++] = myrand >> 8;
  } while (i < 14);

}

#else

static void get_system_time(lib_uuid_time_t* uuid_time) {

  struct timeval tp;

  gettimeofday(&tp, (struct timezone *) 0);

  /* Offset between UUID formatted times and Unix formatted times.
     UUID UTC base time is October 15, 1582.
     Unix base time is January 1, 1970.*/
  *uuid_time = ((uint64_t)tp.tv_sec * 10000000)
    + ((uint64_t)tp.tv_usec * 10)
    + I64(0x01B21DD213814000);
}

/* Sample code, not for use in production; see RFC 1750 */
static void get_random_info(char seed[16]) {

  int fd;
  uint16_t myrand;
  int i;

  /* we aren't all that picky, and we would rather not block so we
    will use urandom */
  fd = open("/dev/urandom", O_RDONLY);

  if (fd != -1) {
    read(fd, seed, 16);
    close(fd);
    return;
  }

  /* ok, now what? */

  i = 0;
  do {
    myrand = true_random();
    seed[i++] = myrand & 0xff;
    seed[i++] = myrand >> 8;
  } while (i < 14);

}

#endif

//// COMMON

/** 
 * Generate a crypto-quality random number.
 */
static uint16_t true_random() {

  //static int inited_random = 0;
  lib_uuid_time_t time_now;

  if (!inited_random) {
    //fprintf(stderr, "Start init random...\n");
    get_system_time(&time_now);
    time_now = time_now / UUIDS_PER_TICK;
    srand((unsigned int)
	  (((time_now >> 32) ^ time_now) & 0xffffffff));
    inited_random = 1;
  }

  return (uint16_t) rand();
}

/**
 * Get time as 60-bit 100ns ticks since UUID epoch.
 * Compensate for the fact that real clock resolution is less than 100ns.
 */
static void get_current_time(lib_uuid_time_t* timestamp) {

  //static int inited_time = 0;
  static lib_uuid_time_t time_last;
  static uint16_t uuids_this_tick;
  lib_uuid_time_t time_now;

  if (!inited_time) {
    //fprintf(stderr, "Start init time...\n");
    get_system_time(&time_now);
    uuids_this_tick = UUIDS_PER_TICK;
    inited_time = 1;
  }

  for (;;) {
    get_system_time(&time_now);

    /* if clock reading changed since last UUID generated, */
    if (time_last != time_now) {
      /* reset count of uuids gen'd with this clock reading */
      uuids_this_tick = 0;
      time_last = time_now;
      break;
    }

    if (uuids_this_tick < UUIDS_PER_TICK) {
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
static void get_ieee_node_identifier(lib_uuid_node_t* node) {
  //static int inited_node = 0;
  static lib_uuid_node_t saved_node;
  char seed[16];

  if (!inited_node) {
    //fprintf(stderr, "Start init node...\n");
    get_random_info(seed);
    seed[0] |= 0x01;
    memcpy(&saved_node, seed, sizeof saved_node);
  }
  inited_node = 1;

  *node = saved_node;
}

//// FORMAT

/**
 * Make a UUID from the timestamp, clockseq, and node ID
 */
static void format_uuid_v1(lib_uuid_t* uuid, uint16_t clock_seq,
                    lib_uuid_time_t timestamp, lib_uuid_node_t node) {

  /* Construct a version 1 uuid with the information we've gathered
     plus a few constants. */
  uuid->time_low = (unsigned long) (timestamp & 0xFFFFFFFF);
  uuid->time_mid = (unsigned short) ((timestamp >> 32) & 0xFFFF);
  uuid->time_hi_and_version = (unsigned short) ((timestamp >> 48) & 0x0FFF);
  uuid->time_hi_and_version |= (1 << 12);
  uuid->clock_seq_low = clock_seq & 0xFF;
  uuid->clock_seq_hi_and_reserved = (clock_seq & 0x3F00) >> 8;
  uuid->clock_seq_hi_and_reserved |= 0x80;
  memcpy(&uuid->node, &node, sizeof uuid->node);
}

/**
 * Generator a UUID
 */
int lib_uuid_create(lib_uuid_t* uuid) {

  lib_uuid_time_t timestamp;
  uint16_t clockseq;
  lib_uuid_node_t node;

  /* get time, node ID, saved state from non-volatile storage */
  get_current_time(&timestamp);
  get_ieee_node_identifier(&node);

  /* for us clockseq is always to be random as we have no state */
  clockseq = true_random();

  /* stuff fields into the UUID */
  format_uuid_v1(uuid, clockseq, timestamp, node);
  return 1;
}

void lib_uuid_reset() {
  /*static int */ inited_random = 0;
  /*static int */ inited_time   = 0;
  /*static int */ inited_node   = 0;
}

/**
 * Print a UUID
 */
int lib_uuid_print(lib_uuid_t u) {
  int pos = printf("%8.8x-%4.4x-%4.4x-%2.2x%2.2x-", 
     u.time_low,
     u.time_mid,
	 u.time_hi_and_version, 
     u.clock_seq_hi_and_reserved,
	 u.clock_seq_low);
  
  for (int i = 0; i < 6; i++) {
    pos += printf("%2.2x", u.node[i]);
  }    
  
  //pos += printf("\n");
  return pos;
}

/**
 * Print a UUID in the supplied buffer
 */
int lib_uuid_snprint(char* str, size_t size, lib_uuid_t u) {
  
  char *tmp = str;
  if (size < 38) {
    //snprintf(tmp, size, "%s", "uuid string too small");
    return 0;
  }

  /* perhaps this is a trifle optimistic but what the heck */
  int pos = sprintf(tmp,
	  "%8.8x-%4.4x-%4.4x-%2.2x%2.2x-",
	  u.time_low,
	  u.time_mid,
	  u.time_hi_and_version,
	  u.clock_seq_hi_and_reserved,
	  u.clock_seq_low);

  tmp += 24;
  for (int i = 0; i < 6; i++) {
    pos += sprintf(tmp, "%2.2x", u.node[i]);
    tmp += 2;
  }
  *tmp = 0;

  return pos;
}

// https://www.ietf.org/rfc/rfc4122.txt
// https://github.com/zhicheng/uuid/blob/master/uuid.c
// https://github.com/HewlettPackard/netperf/blob/master/src/net_uuid.c