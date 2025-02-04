#ifndef PLAZMA_LIB_FSDEF_H
#define PLAZMA_LIB_FSDEF_H

#define LIB_FS_DIR_SEPARATOR_NIX '/'
#define LIB_FS_DIR_SEPARATOR_STR_NIX "/"

#define LIB_FS_DIR_SEPARATOR_WIN '\\'
#define LIB_FS_DIR_SEPARATOR_STR_WIN "\\"

#define LIB_FS_IS_DIR_SEPARATOR_ALL(c) ((c) == LIB_FS_DIR_SEPARATOR_NIX || (c) == LIB_FS_DIR_SEPARATOR_WIN)

#ifdef _WIN32

//#include <windows.h>

/* Stuff missing in std vc6 api */
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES -1
#endif
#ifndef FILE_ATTRIBUTE_DEVICE
#define FILE_ATTRIBUTE_DEVICE 64
#endif

#define LIB_FS_DIR_SEPARATOR '\\'
#define LIB_FS_DIR_SEPARATOR_STR "\\"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR || (c) == '/')

#else

//#include <dirent.h>

#define LIB_FS_DIR_SEPARATOR '/'
#define LIB_FS_DIR_SEPARATOR_STR "/"
#define LIB_FS_IS_DIR_SEPARATOR(c) ((c) == LIB_FS_DIR_SEPARATOR)

#endif


/*
 * File types
 */
#define LIB_FS_UNKNOWN       0
#define LIB_FS_FIFO          1
#define LIB_FS_CHR           2
#define LIB_FS_DIR           4
#define LIB_FS_BLK           6
#define LIB_FS_REG           8
#define LIB_FS_LNK          10
#define LIB_FS_SOCK         12
#define LIB_FS_WHT          14

#ifdef _WIN32

typedef uint32_t      id_t;          /* pid_t, uid_t, or gid_t*/
typedef int32_t       pid_t;         /* Process and group IDs */
typedef uint32_t      gid_t;         /* Process and group IDs */
typedef uint32_t      uid_t;         /* User IDs */

typedef uint64_t      ino_t;         /* Used for inodes */
typedef int32_t       dev_t;         /* dev_t */
typedef uint16_t      mode_t;        /* Some file attributes */

typedef int64_t       off_t;         /* Used for file sizes */
typedef uint16_t      nlink_t;       /* Link count */
typedef int64_t       blkcnt_t;      /* Total blocks */
typedef int32_t       blksize_t;     /* Preferred block size */
typedef unsigned int  fsblkcnt_t;    /* Used by statvfs and fstatvfs */
typedef unsigned int  fsfilcnt_t;    /* Used by statvfs and fstatvfs */

typedef uint32_t      sigset_t;      /* Signal set */
typedef int32_t       suseconds_t;   /* Microseconds */
typedef uint32_t      useconds_t;    /* Microseconds */

#endif

#endif // PLAZMA_LIB_FSDEF_H